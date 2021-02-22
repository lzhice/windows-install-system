#include "Stdafx.h"
#include "DlgProgress.h"
#include "DlgWarning.h"
#include "DlgOption.h"
#include <Shlwapi.h>
#include <ShlObj.h>

DlgProgress::DlgProgress(std::function<void(HWND)> wndCreatedCB)
    : wndCreatedCB_(wndCreatedCB)
    , pProgress_(NULL)
    , pLblProgress_(NULL)
    , pLblDetail_(NULL)
    , pLblImage_(NULL) {}

DlgProgress::~DlgProgress() {}

CDuiString DlgProgress::GetSkinFile() {
  return TEXT("progress.xml");
}

LPCTSTR DlgProgress::GetWindowClassName() const {
  return TEXT("INSTALLER_PROGRESS_DIALOG");
}

void DlgProgress::Notify(TNotifyUI& msg) {
  if (msg.sType == DUI_MSGTYPE_WINDOWINIT) {
  }
  else if (msg.sType == DUI_MSGTYPE_CLICK) {
    if (msg.pSender == pBtnClose_) {
      OnUserCancelInstall();
    }
    else if (msg.pSender == pBtnMin_) {
      SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
    }
  }
}

LRESULT DlgProgress::ResponseDefaultKeyEvent(WPARAM wParam) {
  if (wParam == VK_RETURN || wParam == VK_ESCAPE)
    return 0;
  return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

LRESULT DlgProgress::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_CLOSE && wParam == 0 && lParam == 0) {
    OnUserCancelInstall();
    return TRUE;
  }
  return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT DlgProgress::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
  if (uMsg == WUM_EXTRACT_PROGRESS) {
    bHandled = true;

    ExtractProgress* pEP = (ExtractProgress*)wParam;
    if (pEP) {
      if (pLblProgress_) {
        TCHAR szProgress[50];
        StringCchPrintf(szProgress, 50, TEXT("%d/%d"), pEP->cur, pEP->total);
        pLblProgress_->SetText(szProgress);
      }

      if (pProgress_) {
        pProgress_->SetValue((int)((float)pEP->cur * 100.f / (float)pEP->total));
      }

      if (pLblDetail_) {
        WCHAR szDetail[251];
        StringCchPrintfW(szDetail, 251, TEXT("Extract: %s"), pEP->item.c_str());
        pLblDetail_->SetText(szDetail);
      }

      delete pEP;
      pEP = NULL;
    }
  }
  else if (uMsg == WUM_RUN_AS_ADMIN_FAILED) {
    bHandled = true;
    DlgWarning::ShowWarning(GetHWND(), TEXT("Install Failed"),
                            TEXT("Please run the installer with administrator privileges!"), false);
  }
  else if (uMsg == WUM_CREATE_INSTALL_FOLDER_FAILED) {
    bHandled = true;
    DlgWarning::ShowWarning(GetHWND(), TEXT("Install Failed"),
                            TEXT("Create install folder failed!"), false);
  }
  else if (uMsg == WUM_EXTRACT_FAILED) {
    bHandled = true;
    DlgWarning::ShowWarning(GetHWND(), TEXT("Install Failed"), TEXT("Extract file to disk failed!"),
                            false);
  }
  else if (uMsg == WUM_CREATE_UNINSTALLER_FAILED) {
    bHandled = true;
  }
  else if (uMsg == WUM_CREATE_SHORTCUT_FAILED) {
    bHandled = true;
  }
  return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

void DlgProgress::InitWindow() {
  pLblTitle_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblTitle")));
  pProgress_ = dynamic_cast<CProgressUI*>(m_PaintManager.FindControl(TEXT("progress")));
  pLblProgress_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblProgress")));
  pLblDetail_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblDetail")));
  pLblImage_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblImage")));
  pBtnMin_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnMin")));
  pBtnClose_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnClose")));

  if (pLblTitle_) {
    pLblTitle_->SetText(PARSE(InstallerConfig::Instance()->GetProgressDlgCfg().title).c_str());
  }

  if (wndCreatedCB_) {
    wndCreatedCB_(GetHWND());
  }
}

void DlgProgress::OnUserCancelInstall() {
  if (DlgWarning::ShowWarning(GetHWND(), PARSE(InstallerConfig::Instance()->GetCoreCfg().productName).c_str(), TEXT("Are you sure to exit?"), true) ==
      WarningDialogClicked::OK_BUTTON) {
    // TODO: Stop Extract

    //if (pDlgMain_) {
    //  pDlgMain_->Close();
    //}

    Close(DLG_CLOSE_REASON::CANCEL);
  }
}
