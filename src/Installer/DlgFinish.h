#ifndef INSTALLER_DLG_FINISH_H_
#define INSTALLER_DLG_FINISH_H_
#pragma once

class DlgFinish : public WindowImplBase {
 public:
  DlgFinish();
  ~DlgFinish();

 protected:
  CDuiString GetSkinFile() override;
  LPCTSTR GetWindowClassName(void) const override;
  void Notify(TNotifyUI& msg) override;
  LRESULT ResponseDefaultKeyEvent(WPARAM wParam) override;
  LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
  void InitWindow() override;

 private:

 private:
  CLabelUI* pLblTitle_;
  CLabelUI* pLblImage_;
  CButtonUI* pBtnMin_;
  CButtonUI* pBtnClose_;
  CButtonUI* pBtnFinish_;
};
#endif  // !INSTALLER_DLG_FINISH_H_