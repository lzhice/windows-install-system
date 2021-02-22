#pragma once

enum WarningDialogClicked {
  UNKNOWN = 0,
  OK_BUTTON = 1,
  CANCEL_BUTTON = 2,
  CLOSE_BUTTON = 3
};

class DlgUninstWarning : public WindowImplBase {
 public:
  DlgUninstWarning(const CDuiString& title, const CDuiString& msg, bool hasCancelButton = false);
  ~DlgUninstWarning();

  static WarningDialogClicked ShowWarning(HWND hParent,
                         const CDuiString& title,
                         const CDuiString& msg,
                         bool hasCancleButton);
 protected:
  CDuiString GetSkinFile() override;
  LPCTSTR GetWindowClassName(void) const override;
  void Notify(TNotifyUI& msg) override;
  LRESULT ResponseDefaultKeyEvent(WPARAM wParam) override;
  void InitWindow() override;

 private:
  CButtonUI* pBtnClose_;
  CButtonUI* pBtnOK_;
  CButtonUI* pBtnCancel_;
  CLabelUI* pLblTitle_;
  CLabelUI* pLblMsg_;

  bool bHasCancelButton_;
  CDuiString strMsg_;
  CDuiString strTitle_;
};
