#ifndef INSTALLER_DLG_OPTION_H_
#define INSTALLER_DLG_OPTION_H_
#pragma once

class DlgOption : public WindowImplBase {
 public:
  DlgOption(std::wstring& strInstallFolder);
  ~DlgOption();

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
  CEditUI* pEditInstallPath_;
  CLabelUI* pLblTitle_;
  CLabelUI* pLblDiskInfo_;
  CLabelUI* pLblInstallTo_;
  CButtonUI* pBtnBrowser_;
  CButtonUI* pBtnNext_;
  CButtonUI* pBtnMin_;
  CButtonUI* pBtnClose_;

  std::wstring& strInstallFolder_;
};

#endif  // !INSTALLER_DLG_OPTION_H_