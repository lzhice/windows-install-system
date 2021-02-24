#include "Stdafx.h"
#include "DlgUninstProgress.h"
#include "DlgUninstWarning.h"
#include <Shlwapi.h>
#include <ShlObj.h>

DlgUninstProgress::DlgUninstProgress(std::function<void(HWND)> wndCreatedCB)
    : wndCreatedCB_(wndCreatedCB)
    , pProgress_(NULL)
    , pLblProgress_(NULL)
    , pLblDetail_(NULL)
    , pLblImage_(NULL) {}

DlgUninstProgress::~DlgUninstProgress() {}

CDuiString DlgUninstProgress::GetSkinFile() {
  return TEXT("progress.xml");
}

LPCTSTR DlgUninstProgress::GetWindowClassName() const {
  return TEXT("UNINSTALLER_PROGRESS_DIALOG");
}

void DlgUninstProgress::Notify(TNotifyUI& msg) {
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

LRESULT DlgUninstProgress::ResponseDefaultKeyEvent(WPARAM wParam) {
  if (wParam == VK_RETURN || wParam == VK_ESCAPE)
    return 0;
  return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

LRESULT DlgUninstProgress::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_CLOSE && wParam == 0 && lParam == 0) {
    OnUserCancelInstall();
    return TRUE;
  }
  return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT DlgUninstProgress::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
  if (uMsg == WUM_REMOVE_PROGRESS) {
    bHandled = true;

    RemoveProgress* pRP = (RemoveProgress*)wParam;
    if (pRP) {
      if (pLblProgress_) {
        TCHAR szProgress[50];
        StringCchPrintf(szProgress, 50, TEXT("%d/%d"), pRP->cur, pRP->total);
        pLblProgress_->SetText(szProgress);
      }

      if (pProgress_) {
        pProgress_->SetValue((int)((float)pRP->cur * 100.f / (float)pRP->total));
      }

      if (pLblDetail_) {
        pLblDetail_->SetText(pRP->msg.c_str());
      }

      SAFE_DELETE(pRP);
    }
  }
  else if (uMsg == WUM_RUN_AS_ADMIN_FAILED) {
    bHandled = true;
    DlgUninstWarning::ShowWarning(GetHWND(), TEXT("Uninstall Failed"),
                            TEXT("Please run the installer with administrator privileges!"), false);
  }
  else if (uMsg == WUM_REMOVE_FILE_FAILED) {
    bHandled = true;
    DlgUninstWarning::ShowWarning(GetHWND(), TEXT("Uninstall Failed"), TEXT("Extract file to disk failed!"),
                            false);
  }
  else if (uMsg == WUM_REMOVE_REGISTER_FAILED) {
    bHandled = true;
  }
  else if (uMsg == WUM_REMOVE_SHORTCUT_FAILED) {
    bHandled = true;
  }
  return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

void DlgUninstProgress::InitWindow() {
  pLblTitle_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblTitle")));
  pProgress_ = dynamic_cast<CProgressUI*>(m_PaintManager.FindControl(TEXT("progress")));
  pLblProgress_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblProgress")));
  pLblDetail_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblDetail")));
  pLblImage_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblImage")));
  pBtnMin_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnMin")));
  pBtnClose_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnClose")));

  if (pLblTitle_) {
    pLblTitle_->SetText(PARSE(UninstallerConfig::Instance()->GetProgressDlgCfg().title).c_str());
  }

  if (wndCreatedCB_) {
    wndCreatedCB_(GetHWND());
  }
}

void DlgUninstProgress::OnUserCancelInstall() {
  if (DlgUninstWarning::ShowWarning(
          GetHWND(), PARSE(UninstallerConfig::Instance()->GetCoreCfg().productName).c_str(),
          TEXT("Are you sure to exit?"), true) == WarningDialogClicked::OK_BUTTON) {
    // TODO: Stop Remove

    //if (pDlgMain_) {
    //  pDlgMain_->Close();
    //}

    Close(DLG_CLOSE_REASON::CANCEL);
  }
}
