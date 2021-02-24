#include "StdAfx.h"
#include "UninstallLogic.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include "resource.h"
#include "akali/registry.h"
#include "akali/process_util.h"
#include "akali/stringencode.h"
#include "DlgUninstOption.h"
#include "DlgUninstProgress.h"
#include "DlgUninstFinish.h"
#include "InstallRecord.h"
#include "akali/filesystem.hpp"

using namespace akali;

UninstallLogic::UninstallLogic(HINSTANCE hInst, bool bSilent) : hInst_(hInst), bSilent_(bSilent) {
  if (!bSilent_) {
    CPaintManagerUI::SetInstance(hInst_);
    CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
    CPaintManagerUI::SetResourceZip(IDR_ZIP_RES_SKIN);
  }
}

UninstallLogic::~UninstallLogic() {
  if (workThread_.joinable()) {
    workThread_.join();
  }
  if (!bSilent_) {
    CPaintManagerUI::Term();
  }
}

void UninstallLogic::Run() {
  if (bSilent_) {
    DoSilentUninstall();
  }
  else {
    DoUnSilentUninstall();
  }
}

void UninstallLogic::DoSilentUninstall() {
  if (!RemoveFile(NULL)) {
  }
  if (!RemoveShortcut(NULL)) {
  }
  if (!RemoveRegister(NULL)) {
  }
  if (!RemoveUninstallInfo(NULL)) {
  }
  if (!RemoveInstallRecord(NULL)) {
  }
  if (!RemoveSelf()) {
    TraceMsgW(L"remove self failed");
  }
}

void UninstallLogic::DoUnSilentUninstall() {
  DlgUninstOption dlgOption;
  if (dlgOption.Create(NULL, PARSE(UninstallerConfig::Instance()->GetOptionDlgCfg().title).c_str(),
                       UI_WNDSTYLE_DIALOG, 0)) {
    dlgOption.SetIcon(IDI_ICON_LOGO);
    dlgOption.CenterWindow();
    if (dlgOption.ShowModal() == DLG_CLOSE_REASON::CANCEL) {
      return;
    }
  }

  DlgUninstProgress dlgProgress([this](HWND hProgressWnd) {
    workThread_ = std::thread([hProgressWnd, this]() {
      if (!RemoveFile(hProgressWnd)) {
        return;
      }
      if (!RemoveShortcut(hProgressWnd)) {
        return;
      }
      if (!RemoveRegister(hProgressWnd)) {
      }
      if (!RemoveUninstallInfo(hProgressWnd)) {
      }
      if (!RemoveInstallRecord(hProgressWnd)) {
      }

      assert(hProgressWnd);
      ::PostMessage(hProgressWnd, WM_CLOSE, DLG_CLOSE_REASON::CONTINUE, 0);
    });
  });

  if (dlgProgress.Create(NULL,
                         PARSE(UninstallerConfig::Instance()->GetProgressDlgCfg().title).c_str(),
                         UI_WNDSTYLE_DIALOG, 0)) {
    dlgProgress.SetIcon(IDI_ICON_LOGO);
    dlgProgress.CenterWindow();
    if (dlgProgress.ShowModal() == DLG_CLOSE_REASON::CANCEL) {
      return;
    }
  }

  DlgUninstFinish dlgFinish;
  if (dlgFinish.Create(NULL, PARSE(UninstallerConfig::Instance()->GetFinishDlgCfg().title).c_str(),
                       UI_WNDSTYLE_DIALOG, 0)) {
    dlgFinish.SetIcon(IDI_ICON_LOGO);
    dlgFinish.CenterWindow();
    if (dlgFinish.ShowModal() == DLG_CLOSE_REASON::CANCEL) {
      return;
    }
  }

  if (!RemoveSelf()) {
    TraceMsgW(L"remove self failed");
  }
}

bool UninstallLogic::RemoveFile(HWND hProgressWnd) {
  bool ret = true;
  const std::vector<std::wstring>& files = InstallRecord::Instance()->GetFiles();
  std::vector<std::wstring> dirs;
  unsigned int total = files.size();
  unsigned int cur = 0L;

  for (const auto& i : files) {
    if (_waccess_s(i.c_str(), 0) != 0) {
      cur++;
      continue;
    }

    std::error_code ec;
    if (akali::filesystem::is_directory(i, ec)) {
      dirs.push_back(i);
    }
    else {
      if (!DeleteFileW(i.c_str())) {
        TraceMsgW(L"remove file failed: %s\n", i.c_str());
        ret = false;
        break;
      }
      else {
        if (hProgressWnd) {
          RemoveProgress* pRP = new RemoveProgress();
          pRP->msg = L"Remove: " + i;
          pRP->cur = ++cur;
          pRP->total = total;
          ::PostMessage(hProgressWnd, WUM_REMOVE_PROGRESS, (WPARAM)pRP, 0);
        }
      }
    }
  }

  for (const auto& i : dirs) {
    if (_waccess_s(i.c_str(), 0) != 0) {
      cur++;
      continue;
    }
    std::error_code ec;
    if (!akali::filesystem::remove_all(i.c_str(), ec)) {
      TraceMsgW(L"remove dir failed: %s\n", i.c_str());
      ret = false;
      break;
    }
    else {
      if (hProgressWnd) {
        RemoveProgress* pRP = new RemoveProgress();
        pRP->msg = L"Remove: " + i;
        pRP->cur = ++cur;
        pRP->total = total;
        ::PostMessage(hProgressWnd, WUM_REMOVE_PROGRESS, (WPARAM)pRP, 0);
      }
    }
  }

  return ret;
}

bool UninstallLogic::RemoveShortcut(HWND hProgressWnd) {
  bool ret = true;
  const std::vector<std::wstring>& files = InstallRecord::Instance()->GetShortcuts();
  for (const auto& i : files) {
    if (_waccess_s(i.c_str(), 0) != 0) {
      continue;
    }
    if (!DeleteFileW(i.c_str())) {
      TraceMsgW(L"remove shortcut file failed: %s\n", i.c_str());
      ret = false;
      break;
    }
  }
  return ret;
}

bool UninstallLogic::RemoveRegister(HWND hProgressWnd) {
  bool ret = true;
  const std::vector<RegInfo>& regs = InstallRecord::Instance()->GetRegInfos();
  for (const auto& i : regs) {
    HKEY hKey = NULL;
    if (i.rootKey == L"HKEY_CURRENT_USER") {
      hKey = HKEY_CURRENT_USER;
    }
    else if (i.rootKey == L"HKEY_CLASSES_ROOT") {
      hKey = HKEY_CLASSES_ROOT;
    }
    else if (i.rootKey == L"HKEY_LOCAL_MACHINE") {
      hKey = HKEY_LOCAL_MACHINE;
    }
    else if (i.rootKey == L"HKEY_USERS") {
      hKey = HKEY_USERS;
    }
    else if (i.rootKey == L"HKEY_CURRENT_CONFIG") {
      hKey = HKEY_CURRENT_CONFIG;
    }

    assert(hKey);
    if (!hKey) {
      TraceMsgW(L"root key not support: %s\n", i.rootKey.c_str());
      ret = false;
      break;
    }

    if (!akali::RegKey::DeleteKey(hKey, i.subKey.c_str(), i.item.c_str(), !i.WOW6432Node)) {
      TraceMsgW(L"delete root key failed: %s %s %s\n", i.rootKey.c_str(), i.subKey.c_str(),
                i.item.c_str());
      ret = false;
      break;
    }
  }
  return ret;
}

bool UninstallLogic::RemoveUninstallInfo(HWND hProgressWnd) {
  std::wstring strKeyName = InstallRecord::Instance()->GetUninstallRegKeyName();
  if (akali::RegKey::DeleteSubKeys(
          HKEY_CURRENT_USER,
          (L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + strKeyName).c_str(),
          false)) {
    return true;
  }
  TraceMsgW(L"remove uninstall reg failed: %s\n", strKeyName.c_str());
  return false;
}

bool UninstallLogic::RemoveInstallRecord(HWND hProgressWnd) {
  return InstallRecord::Instance()->RemoveRecordFile();
}

bool UninstallLogic::RemoveSelf() {
  wchar_t szPath[MAX_PATH] = {0};
  GetModuleFileNameW(NULL, szPath, MAX_PATH);

  wchar_t szSystemDir[MAX_PATH] = {0};
  if (GetSystemDirectoryW(szSystemDir, MAX_PATH) > 0) {
    PathAddBackslashW(szSystemDir);
  }

  wchar_t szFullCmd[MAX_PATH] = {0};
  StringCchPrintfW(szFullCmd, MAX_PATH, L"%scmd.exe /C choice /C Y /N /D Y /T 2 & Del \"%s\"",
                   szSystemDir, szPath);
  STARTUPINFOW si = {sizeof(STARTUPINFOW)};
  PROCESS_INFORMATION pi;
  if (CreateProcessW(NULL, szFullCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, szSystemDir, &si,
                     &pi)) {
    if (pi.hThread)
      CloseHandle(pi.hThread);
    if (pi.hProcess)
      CloseHandle(pi.hProcess);
    return true;
  }
  return false;
}
