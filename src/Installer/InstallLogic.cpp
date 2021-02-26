#include "StdAfx.h"
#include "InstallLogic.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include "ZipResource.h"
#include "resource.h"
#include "akali/file_util.h"
#include "akali/process_util.h"
#include "akali/stringencode.h"
#include "akali/registry.h"
#include "akali/shortcut.h"
#include "DlgOption.h"
#include "DlgProgress.h"
#include "DlgFinish.h"

InstallLogic::InstallLogic(HINSTANCE hInst, bool bSilent, const std::wstring& installFolder)
    : hInst_(hInst), bSilent_(bSilent), strInstallFolder_(installFolder) {
  Variable::Instance()->SetSilentMode(bSilent_);
  if (!bSilent_) {
    CPaintManagerUI::SetInstance(hInst_);
    CPaintManagerUI::SetResourceType(UILIB_ZIPRESOURCE);
    CPaintManagerUI::SetResourceZip(IDR_ZIP_RES_SKIN);
  }

  if (strInstallFolder_.length() > 0) {
    if (strInstallFolder_[strInstallFolder_.length() - 1] != L'\\' &&
        strInstallFolder_[strInstallFolder_.length() - 1] != L'/') {
      strInstallFolder_ += L"\\";
    }
  }
}

InstallLogic::~InstallLogic() {
  if (workThread_.joinable()) {
    workThread_.join();
  }
  if (!bSilent_) {
    CPaintManagerUI::Term();
  }
}

void InstallLogic::Run() {
  if (bSilent_) {
    DoSilentInstall();
  }
  else {
    DoUnSilentInstall();
  }
}

void InstallLogic::DoSilentInstall() {
  Variable::Instance()->SetInstallFolder(strInstallFolder_);

  if (!CreateInstallFolder(NULL)) {
    assert(false);
    akali::TraceMsgW(L"CreateInstallFolder failed\n");
    return;
  }
  if (!ExtractFiles(NULL)) {
    assert(false);
    akali::TraceMsgW(L"ExtractFiles failed\n");
    return;
  }
  if (!CreateUninstallInfo(NULL)) {
    assert(false);
    akali::TraceMsgW(L"CreateUninstallInfo failed\n");
  }
  if (!CreateShortcut(NULL)) {
    assert(false);
    akali::TraceMsgW(L"CreateShortcut failed\n");
  }
  if (!CreateRegister(NULL)) {
    assert(false);
    akali::TraceMsgW(L"CreateRegister failed\n");
  }
  if (!WriteInstallRecord(NULL)) {
    assert(false);
    akali::TraceMsgW(L"WriteInstallRecord failed\n");
  }
  if (!ExecuteCmd(NULL)) {
    assert(false);
    akali::TraceMsgW(L"ExecuteCmd failed\n");
  }
}

void InstallLogic::DoUnSilentInstall() {
  DlgOption dlgOption(strInstallFolder_);
  if (dlgOption.Create(NULL, PARSE(InstallerConfig::Instance()->GetCoreCfg().productName).c_str(),
                       UI_WNDSTYLE_DIALOG, 0)) {
    dlgOption.SetIcon(IDI_ICON_LOGO);
    dlgOption.CenterWindow();
    if (dlgOption.ShowModal() == DLG_CLOSE_REASON::CANCEL) {
      return;
    }
  }

  Variable::Instance()->SetInstallFolder(strInstallFolder_);

  DlgProgress dlgProgress([this](HWND hProgressWnd) {
    workThread_ = std::thread([hProgressWnd, this]() {
      if (!CreateInstallFolder(hProgressWnd)) {
        assert(false);
        akali::TraceMsgW(L"CreateInstallFolder failed\n");
        return;
      }
      if (!ExtractFiles(hProgressWnd)) {
        assert(false);
        akali::TraceMsgW(L"ExtractFiles failed\n");
        return;
      }
      if (!ExtractUninst(hProgressWnd)) {
        assert(false);
        akali::TraceMsgW(L"ExtractUninst failed\n");
      }
      if (!CreateUninstallInfo(hProgressWnd)) {
        assert(false);
        akali::TraceMsgW(L"CreateUninstallInfo failed\n");
      }
      if (!CreateShortcut(hProgressWnd)) {
        assert(false);
        akali::TraceMsgW(L"CreateShortcut failed\n");
      }
      if (!CreateRegister(hProgressWnd)) {
        assert(false);
        akali::TraceMsgW(L"CreateRegister failed\n");
      }
      if (!WriteInstallRecord(hProgressWnd)) {
        assert(false);
        akali::TraceMsgW(L"WriteInstallRecord failed\n");
      }
	  if (!ExecuteCmd(hProgressWnd)) {
        assert(false);
        akali::TraceMsgW(L"ExecuteCmd failed\n");
	  }

      assert(hProgressWnd);
      ::PostMessage(hProgressWnd, WM_CLOSE, DLG_CLOSE_REASON::CONTINUE, 0);
    });
  });

  if (dlgProgress.Create(NULL, PARSE(InstallerConfig::Instance()->GetCoreCfg().productName).c_str(),
                         UI_WNDSTYLE_DIALOG, 0)) {
    dlgProgress.SetIcon(IDI_ICON_LOGO);
    dlgProgress.CenterWindow();
    if (dlgProgress.ShowModal() == DLG_CLOSE_REASON::CANCEL) {
      return;
    }
  }

  DlgFinish dlgFinish;
  if (dlgFinish.Create(NULL, PARSE(InstallerConfig::Instance()->GetCoreCfg().productName).c_str(),
                       UI_WNDSTYLE_DIALOG, 0)) {
    dlgFinish.SetIcon(IDI_ICON_LOGO);
    dlgFinish.CenterWindow();
    if (dlgFinish.ShowModal() == DLG_CLOSE_REASON::CANCEL) {
      return;
    }
  }
}

bool InstallLogic::CreateInstallFolder(HWND hProgressWnd) {
  if (!akali::CreateDir(strInstallFolder_.c_str())) {
    if (GetLastError() == 5) {
      TCHAR szPath[MAX_PATH] = {0};
      GetModuleFileName(NULL, szPath, MAX_PATH);
      if (!akali::RunAsAdministrator(szPath, TEXT("-runasadmin"),
                                     GetCurrentProcessDirectory().c_str(), FALSE)) {
        if (hProgressWnd) {
          ::PostMessage(hProgressWnd, WUM_RUN_AS_ADMIN_FAILED, 0, 0);
        }
      }
    }
    else {
      if (hProgressWnd) {
        ::PostMessage(hProgressWnd, WUM_CREATE_INSTALL_FOLDER_FAILED, 0, 0);
      }
    }
    return false;
  }

  return true;
}

bool InstallLogic::ExtractFiles(HWND hProgressWnd) {
  ZipResource zipResource(IDR_ZIP_RES_APP_FILES);
  zipResource.OpenZip();

  Json::Value extractFiles;
  if (!zipResource.ExtractToFolder(
          strInstallFolder_,
          [hProgressWnd, &extractFiles, this](std::wstring item, unsigned int cur, unsigned total) {
            extractFiles.append(akali::UnicodeToUtf8(item));
            if (hProgressWnd) {
              ExtractProgress* pEP = new ExtractProgress();
              pEP->msg = L"Extract: " + item;
              pEP->cur = cur;
              pEP->total = total;

              ::PostMessage(hProgressWnd, WUM_EXTRACT_PROGRESS, (WPARAM)pEP, 0);
            }
          })) {
    if (hProgressWnd) {
      ::PostMessage(hProgressWnd, WUM_EXTRACT_FAILED, 0, 0);
    }
    return false;
  }

  root_["file"] = extractFiles;

  return true;
}

bool InstallLogic::ExtractUninst(HWND hProgressWnd) {
  HRSRC hResource =
      FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_EXE_UNINSTALLER), TEXT("EXE"));
  if (!hResource) {
    return false;
  }
  HGLOBAL hg = LoadResource(GetModuleHandle(NULL), hResource);
  if (!hg) {
    return false;
  }
  LPVOID lpData = LockResource(hg);
  DWORD dwDataSize = SizeofResource(GetModuleHandle(NULL), hResource);
  FILE* f = NULL;
  _wfopen_s(&f, (strInstallFolder_ + L"Uninst.exe").c_str(), L"wb+");
  if (!f) {
    return false;
  }
  size_t written = fwrite(lpData, 1, dwDataSize, f);
  fclose(f);
  if (written != dwDataSize) {
    return false;
  }
  return true;
}

bool InstallLogic::CreateUninstallInfo(HWND hProgressWnd) {
  std::wstring strRegKeyName = PARSE(InstallerConfig::Instance()->GetUninstallCfg().regKeyName);
  akali::RegKey reg(
      HKEY_CURRENT_USER,
      (L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\" + strRegKeyName).c_str());
  if (ERROR_SUCCESS == reg.Open(KEY_ALL_ACCESS | KEY_WOW64_32KEY, true)) {
    HRESULT hr = S_OK;
    hr = reg.SetSZValue(L"DisplayName",
                        PARSE(InstallerConfig::Instance()->GetUninstallCfg().displayName));
    assert(hr == S_OK);

    hr = reg.SetSZValue(L"DisplayVersion",
                        PARSE(InstallerConfig::Instance()->GetCoreCfg().setupVersion));
    assert(hr == S_OK);

    hr = reg.SetSZValue(L"UninstallString",
                        PARSE(InstallerConfig::Instance()->GetUninstallCfg().uninstallExePath));
    assert(hr == S_OK);

    hr = reg.SetSZValue(L"DisplayIcon",
                        PARSE(InstallerConfig::Instance()->GetUninstallCfg().uninstallExePath));
    assert(hr == S_OK);

    hr = reg.SetSZValue(L"Publisher",
                        PARSE(InstallerConfig::Instance()->GetUninstallCfg().publisher));
    assert(hr == S_OK);

    hr = reg.SetSZValue(L"URLInfoAbout",
                        PARSE(InstallerConfig::Instance()->GetUninstallCfg().aboutLinkUrl));
    assert(hr == S_OK);

    hr = reg.SetSZValue(L"HelpLink",
                        PARSE(InstallerConfig::Instance()->GetUninstallCfg().helpLinkUrl));
    assert(hr == S_OK);

    reg.Close();

    Json::Value uninstallInfo;
    uninstallInfo["reg_key_name"] = akali::UnicodeToUtf8(strRegKeyName);
    root_["uninstall"] = uninstallInfo;

    return true;
  }

  return false;
}

bool InstallLogic::CreateShortcut(HWND hProgressWnd) {
  bool ret = true;
  Json::Value shortcuts;

  CoInitialize(NULL);
  for (auto& i : InstallerConfig::Instance()->GetShortcutConfigs()) {
    akali::ShortcutProperties sp;
    sp.SetIcon(PARSE(i.iconPath), 0);
    sp.SetTarget(PARSE(i.targetPath));
    sp.SetWorkingDir(PARSE(i.workingDir));

    std::wstring strLnkPath = PARSE(i.lnkPath);
    if (akali::CreateOrUpdateShortcutLink(strLnkPath, sp, akali::SHORTCUT_CREATE_ALWAYS)) {
      if (i.uninstallIncluded) {
        shortcuts.append(akali::UnicodeToUtf8(strLnkPath));
      }
    }
    else {
      akali::TraceMsgW(L"create shortcut failed: %s\n", i.lnkPath.c_str());
      ret = false;
      break;
    }
  }
  ::CoUninitialize();

  root_["shortcut"] = shortcuts;

  return ret;
}

bool InstallLogic::CreateRegister(HWND hProgressWnd) {
  bool ret = true;
  Json::Value registers;

  for (auto& i : InstallerConfig::Instance()->GetRegisterCfgs()) {
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
      akali::TraceMsgW(L"root key not support: %s\n", i.rootKey.c_str());
      ret = false;
      break;
    }

    std::wstring strSubKey = PARSE(i.subKey);
    std::wstring strItem = PARSE(i.item);

    akali::RegKey reg(hKey, strSubKey.c_str());
    REGSAM regsam = KEY_ALL_ACCESS;
    if (i.WOW6432Node)
      regsam |= KEY_WOW64_32KEY;
    if (ERROR_SUCCESS != reg.Open(regsam, true)) {
      akali::TraceMsgW(L"open register failed: %s %s\n", i.rootKey.c_str(), i.subKey.c_str());
      ret = false;
      break;
    }

    HRESULT hr = S_OK;
    if (i.value.type == REG_SZ) {
      hr = reg.SetSZValue(strItem.c_str(), PARSE(i.value.v1));
    }
    else if (i.value.type == REG_DWORD) {
      hr = reg.SetDWORDValue(strItem.c_str(), i.value.v2);
    }
    else {
      assert(false);
      akali::TraceMsgW(L"reg value type not support: %d\n", i.value.type);
      ret = false;
      break;
    }
    reg.Close();

    if (hr != S_OK) {
      akali::TraceMsgW(L"set register value failed: %s %s %s\n", i.rootKey.c_str(),
                       strSubKey.c_str(), strItem.c_str());
      ret = false;
      break;
    }

    if (i.uninstallIncluded) {
      Json::Value tmp;
      tmp["root_key"] = akali::UnicodeToUtf8(i.rootKey);
      tmp["sub_key"] = akali::UnicodeToUtf8(strSubKey);
      tmp["WOW6432Node"] = i.WOW6432Node;
      tmp["item"] = akali::UnicodeToUtf8(strItem);
      registers.append(tmp);
    }
  }

  root_["register"] = registers;

  return ret;
}

bool InstallLogic::ExecuteCmd(HWND hProgressWnd) {
  bool ret = true;

  for (auto& i : InstallerConfig::Instance()->GetExecutorCfgs()) {
    TCHAR szCmd[MAX_PATH];
    StringCchPrintfW(szCmd, MAX_PATH, L"%s %s", PARSE(i.cmd).c_str(), PARSE(i.parameter).c_str());

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {NULL};
    BOOL bCPRet =
        CreateProcess(NULL, szCmd, NULL, NULL, FALSE, 0, NULL, PARSE(i.workingDir).c_str(), &si, &pi);
    assert(bCPRet);
    if (!bCPRet) {
      ret = false;
      akali::TraceMsgW(L"create process failed: %s, GLE: %d\n", szCmd, GetLastError());
      break;
    }

    if (i.waitExit) {
      WaitForSingleObject(pi.hProcess, INFINITE);
    }
    SAFE_CLOSE(pi.hThread);
    SAFE_CLOSE(pi.hProcess);
  }

  return ret;
}

bool InstallLogic::WriteInstallRecord(HWND hProgressWnd) {
  FILE* f = NULL;
  _wfopen_s(&f, (strInstallFolder_ + L"install_record.dat").c_str(), L"w+");
  if (!f) {
    return false;
  }

  std::string strJSON = root_.toStyledString();
  size_t written = fwrite(strJSON.c_str(), 1, strJSON.length(), f);
  if (written != strJSON.length()) {
    fclose(f);
    return false;
  }
  fclose(f);
  return true;
}

bool InstallLogic::IsUrl(const std::wstring& str) {
  // TODO
  assert(0);
  return false;
}
