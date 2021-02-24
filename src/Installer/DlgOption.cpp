#include "StdAfx.h"
#include "DlgOption.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include "akali/stringencode.h"
#include "akali/driver_info.h"
#include "DlgProgress.h"
#include "DlgWarning.h"

DlgOption::DlgOption(std::wstring& strInstallFolder)
    : strInstallFolder_(strInstallFolder)
    , pEditInstallPath_(NULL)
    , pLblTitle_(NULL)
    , pLblDiskInfo_(NULL)
    , pLblInstallTo_(NULL)
    , pBtnBrowser_(NULL)
    , pBtnNext_(NULL)
    , pBtnMin_(NULL)
    , pBtnClose_(NULL) {}

DlgOption::~DlgOption() {}

CDuiString DlgOption::GetSkinFile() {
  return TEXT("option.xml");
}

LPCTSTR DlgOption::GetWindowClassName() const {
  return TEXT("INSTALLRE_OPTION_DIALOG");
}

void DlgOption::Notify(TNotifyUI& msg) {
  if (msg.sType == DUI_MSGTYPE_WINDOWINIT) {
  }
  else if (msg.sType == DUI_MSGTYPE_CLICK) {
    if (msg.pSender == pBtnClose_) {
      Close(DLG_CLOSE_REASON::CANCEL);
    }
    else if (msg.pSender == pBtnMin_) {
      SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
    }
    else if (msg.pSender == pBtnBrowser_) {
      BROWSEINFO bi;
      memset(&bi, 0, sizeof(BROWSEINFO));

      bi.hwndOwner = GetHWND();
      bi.lpszTitle = TEXT("Select Install Directory");
      bi.ulFlags = 0x0040;

      TCHAR szFolderPath[MAX_PATH] = {0};
      LPITEMIDLIST idl = SHBrowseForFolder(&bi);
      if (idl) {
        SHGetPathFromIDList(idl, szFolderPath);
        if (_tcslen(szFolderPath) > 0) {
          PathCombineW(
              szFolderPath, szFolderPath,
              PARSE(InstallerConfig::Instance()->GetOptionDlgCfg().installFolderSuffix).c_str());
          pEditInstallPath_->SetText(szFolderPath);
          UpdateDriverInfo();
        }
      }
    }
    else if (msg.pSender == pBtnNext_) {
      if (!pEditInstallPath_)
        return;
      CDuiString strInstallDir = pEditInstallPath_->GetText().GetData();
      if (strInstallDir.GetLength() > 0) {
        if (strInstallDir[strInstallDir.GetLength() - 1] != L'\\' &&
            strInstallDir[strInstallDir.GetLength() - 1] != L'/') {
          strInstallDir += L"\\";
        }
      }

      Close(DLG_CLOSE_REASON::CONTINUE);
    }
  }
}

LRESULT DlgOption::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_CLOSE && wParam == 0 && lParam == 0) {
    Close(DLG_CLOSE_REASON::CANCEL);
    return TRUE;
  }
  return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT DlgOption::ResponseDefaultKeyEvent(WPARAM wParam) {
  if (wParam == VK_RETURN || wParam == VK_ESCAPE)
    return 0;
  return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

void DlgOption::InitWindow() {
  pEditInstallPath_ = dynamic_cast<CEditUI*>(m_PaintManager.FindControl(TEXT("edtPath")));
  pLblTitle_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblTitle")));
  pLblDiskInfo_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblDiskInfo")));
  pLblInstallTo_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblInstallTo")));
  pBtnBrowser_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnBrowser")));
  pBtnNext_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnNext")));
  pBtnMin_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnMin")));
  pBtnClose_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnClose")));

  if (pLblTitle_) {
    pLblTitle_->SetText(PARSE(InstallerConfig::Instance()->GetOptionDlgCfg().title).c_str());
  }

  if (pEditInstallPath_) {
    pEditInstallPath_->SetText(UnicodeToTCHAR(strInstallFolder_).c_str());
  }
  UpdateDriverInfo();
}

void DlgOption::UpdateDriverInfo() {
  if (!pEditInstallPath_ || !pLblDiskInfo_)
    return;

  int driverAvaliableMB = 0;
  std::wstring strAaliable;
  std::wstring folder = TCHARToUnicode(pEditInstallPath_->GetText().GetData());
  if (folder.length() > 0) {
    int driver = akali::DriveInfo::GetDrive(folder.c_str());
    driverAvaliableMB = (int)akali::DriveInfo::GetFreeMB(driver);

    if (driverAvaliableMB >= 1048576) {  // 1T
      float tb = (float)driverAvaliableMB / 1048576.f;
      wchar_t szTMP[30];
      StringCchPrintfW(szTMP, 30, L"%.1fT", tb);
      strAaliable = szTMP;
    }
    else if (driverAvaliableMB >= 1024) {  // 1G
      float tb = (float)driverAvaliableMB / 1024;
      wchar_t szTMP[30];
      StringCchPrintfW(szTMP, 30, L"%.1fG", tb);
      strAaliable = szTMP;
    }
    else {
      wchar_t szTMP[30];
      StringCchPrintfW(szTMP, 30, L"%dG", driverAvaliableMB);
      strAaliable = szTMP;
    }
  }

  WCHAR szInfo[MAX_PATH];
  StringCchPrintfW(szInfo, MAX_PATH, L"(%s/%s)",
                   PARSE(InstallerConfig::Instance()->GetOptionDlgCfg().needDiskSpace).c_str(),
                   strAaliable.c_str());
  pLblDiskInfo_->SetText(szInfo);
}
