#include "Stdafx.h"
#include "DlgFinish.h"

DlgFinish::DlgFinish() : pLblImage_(NULL), pBtnMin_(NULL), pBtnClose_(NULL), pBtnFinish_(NULL) {}

DlgFinish::~DlgFinish() {}

CDuiString DlgFinish::GetSkinFile() {
  return TEXT("finish.xml");
}

LPCTSTR DlgFinish::GetWindowClassName() const {
  return TEXT("INSTALLER_FINISH_DIALOG");
}

void DlgFinish::Notify(TNotifyUI& msg) {
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

LRESULT DlgFinish::ResponseDefaultKeyEvent(WPARAM wParam) {
  if (wParam == VK_RETURN || wParam == VK_ESCAPE)
    return 0;
  return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

LRESULT DlgFinish::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
  return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

void DlgFinish::InitWindow() {
  pLblTitle_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblTitle")));
  pLblImage_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblImage")));
  pBtnMin_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnMin")));
  pBtnClose_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnClose")));
  pBtnFinish_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnFinish")));

  if (pLblTitle_) {
    pLblTitle_->SetText(PARSE(InstallerConfig::Instance()->GetFinishDlgCfg().title).c_str());
  }
}
