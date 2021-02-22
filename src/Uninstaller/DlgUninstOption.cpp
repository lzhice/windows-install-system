#include "StdAfx.h"
#include "DlgUninstOption.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#include "akali/stringencode.h"
#include "DlgUninstProgress.h"
#include "DlgUninstWarning.h"

DlgUninstOption::DlgUninstOption()
    : pLblTitle_(NULL)
    , pBtnNext_(NULL)
    , pBtnMin_(NULL)
    , pBtnClose_(NULL) {}

DlgUninstOption::~DlgUninstOption() {}

CDuiString DlgUninstOption::GetSkinFile() {
  return TEXT("option.xml");
}

LPCTSTR DlgUninstOption::GetWindowClassName() const {
  return TEXT("UNINSTALLRE_OPTION_DIALOG");
}

void DlgUninstOption::Notify(TNotifyUI& msg) {
  if (msg.sType == DUI_MSGTYPE_WINDOWINIT) {
  }
  else if (msg.sType == DUI_MSGTYPE_CLICK) {
    if (msg.pSender == pBtnClose_) {
      Close(DLG_CLOSE_REASON::CANCEL);
    }
    else if (msg.pSender == pBtnMin_) {
      SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
    }
    else if (msg.pSender == pBtnNext_) {
      Close(DLG_CLOSE_REASON::CONTINUE);
    }
  }
}

LRESULT DlgUninstOption::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (uMsg == WM_CLOSE && wParam == 0 && lParam == 0) {
    Close(DLG_CLOSE_REASON::CANCEL);
    return TRUE;
  }
  return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT DlgUninstOption::ResponseDefaultKeyEvent(WPARAM wParam) {
  if (wParam == VK_RETURN || wParam == VK_ESCAPE)
    return 0;
  return WindowImplBase::ResponseDefaultKeyEvent(wParam);
}

void DlgUninstOption::InitWindow() {
  pLblTitle_ = dynamic_cast<CLabelUI*>(m_PaintManager.FindControl(TEXT("lblTitle")));
  pBtnNext_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnNext")));
  pBtnMin_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnMin")));
  pBtnClose_ = dynamic_cast<CButtonUI*>(m_PaintManager.FindControl(TEXT("btnClose")));

  if (pLblTitle_) {
    pLblTitle_->SetText(PARSE(UninstallerConfig::Instance()->GetOptionDlgCfg().title).c_str());
  }
}