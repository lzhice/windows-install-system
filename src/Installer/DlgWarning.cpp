#include "StdAfx.h"
#include "DlgWarning.h"
#include "resource.h"

DlgWarning::DlgWarning(const CDuiString& title, const CDuiString& msg, bool hasCancelButton)
    : pBtnClose_(NULL)
    , pBtnOK_(NULL)
    , pBtnCancel_(NULL)
    , pLblTitle_(NULL)
    , pLblMsg_(NULL)
    , strMsg_(msg)
    , bHasCancelButton_(hasCancelButton) {}

DlgWarning::~DlgWarning() {}

WarningDialogClicked DlgWarning::ShowWarning(HWND hParent,
                                             const CDuiString& title,
                                             const CDuiString& msg,
                                             bool hasCancleButton) {
  WarningDialogClicked ret = WarningDialogClicked::UNKNOWN;
  DlgWarning dlg_warning(title, msg, hasCancleButton);
  if (dlg_warning.Create(hParent,
                         PARSE(InstallerConfig::Instance()->GetCoreCfg().productName).c_str(),
                         UI_WNDSTYLE_DIALOG, 0)) {
    dlg_warning.SetIcon(IDI_ICON_LOGO);
    dlg_warning.CenterWindow();
    ret = (WarningDialogClicked)dlg_warning.ShowModal();
  }

  return ret;
}

CDuiString DlgWarning::GetSkinFile() {
  return TEXT("warn.xml");
}

LPCTSTR DlgWarning::GetWindowClassName() const {
  return TEXT("INSTALLER_WARNING_DIALOG");
}

void DlgWarning::Notify(TNotifyUI& msg) {
  if (msg.sType == DUI_MSGTYPE_WINDOWINIT) {
  }
  else if (msg.sType == DUI_MSGTYPE_CLICK) {
    if (msg.pSender == pBtnOK_) {
      Close(WarningDialogClicked::OK_BUTTON);
    }
    else if (msg.pSender == pBtnCancel_) {
      Close(WarningDialogClicked::CANCEL_BUTTON);
    }
    else if (msg.pSender == pBtnClose_) {
      Close(WarningDialogClicked::CLOSE_BUTTON);
    }
  }
}

LRESULT DlgWarning::ResponseDefaultKeyEvent(WPARAM wParam) {
  if (wParam == VK_RETURN || wParam == VK_ESCAPE)
    return 0;
  return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

void DlgWarning::InitWindow() {
  pBtnClose_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnClose")));
  pBtnCancel_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnCancel")));
  pBtnOK_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnOK")));

  pLblTitle_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblTitle")));
  pLblMsg_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblMsg")));

  if (pBtnCancel_) {
    pBtnCancel_->SetVisible(bHasCancelButton_);
  }

  if (pLblTitle_) {
    pLblTitle_->SetText(strTitle_);
  }

  if (pLblMsg_) {
    pLblMsg_->SetText(strMsg_);
  }
}
