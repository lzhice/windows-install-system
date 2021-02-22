#include "Stdafx.h"
#include "DlgUninstFinish.h"

DlgUninstFinish::DlgUninstFinish() : pLblImage_(NULL), pBtnMin_(NULL), pBtnClose_(NULL), pBtnFinish_(NULL) {}

DlgUninstFinish::~DlgUninstFinish() {}

CDuiString DlgUninstFinish::GetSkinFile() {
  return TEXT("finish.xml");
}

LPCTSTR DlgUninstFinish::GetWindowClassName() const {
  return TEXT("UNINSTALLER_FINISH_DIALOG");
}

void DlgUninstFinish::Notify(TNotifyUI& msg) {
  if (msg.sType == DUI_MSGTYPE_WINDOWINIT) {
  }
  else if (msg.sType == DUI_MSGTYPE_CLICK) {
    if (msg.pSender == pBtnClose_) {
      Close(DLG_CLOSE_REASON::CANCEL);
    }
    else if (msg.pSender == pBtnMin_) {
      SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
    }
    else if (msg.pSender == pBtnFinish_) {
      Close(DLG_CLOSE_REASON::CONTINUE);
    }
  }
}

LRESULT DlgUninstFinish::ResponseDefaultKeyEvent(WPARAM wParam) {
  if (wParam == VK_RETURN || wParam == VK_ESCAPE)
    return 0;
  return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

LRESULT DlgUninstFinish::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
  return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

void DlgUninstFinish::InitWindow() {
  pLblTitle_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblTitle")));
  pLblImage_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblImage")));
  pBtnMin_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnMin")));
  pBtnClose_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnClose")));
  pBtnFinish_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnFinish")));

  if (pLblTitle_) {
    pLblTitle_->SetText(PARSE(UninstallerConfig::Instance()->GetFinishDlgCfg().title).c_str());
  }
}
