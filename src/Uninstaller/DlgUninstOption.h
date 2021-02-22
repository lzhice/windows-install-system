#ifndef UNINSTALLER_DLG_OPTION_H_
#define UNINSTALLER_DLG_OPTION_H_
#pragma once

class DlgUninstOption : public WindowImplBase {
 public:
  DlgUninstOption();
  ~DlgUninstOption();

 protected:
  CDuiString GetSkinFile() override;
  LPCTSTR GetWindowClassName(void) const override;
  void InitWindow() override;
  void Notify(TNotifyUI& msg) override;
  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
  LRESULT ResponseDefaultKeyEvent(WPARAM wParam) override;

 private:
  void UpdateDriverInfo();

 private:
  CLabelUI* pLblTitle_;
  CButtonUI* pBtnNext_;
  CButtonUI* pBtnMin_;
  CButtonUI* pBtnClose_;
};

#endif  // !UNINSTALLER_DLG_OPTION_H_