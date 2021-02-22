#ifndef INSTALLER_DLG_PROGRESS_H_
#define INSTALLER_DLG_PROGRESS_H_
#pragma once

#include <functional>

#define WUM_EXTRACT_PROGRESS (WM_USER + 1001)
#define WUM_RUN_AS_ADMIN_FAILED (WM_USER + 1002)
#define WUM_CREATE_INSTALL_FOLDER_FAILED (WM_USER + 1003)
#define WUM_EXTRACT_FAILED (WM_USER + 1004)
#define WUM_CREATE_UNINSTALLER_FAILED (WM_USER + 1005)
#define WUM_CREATE_SHORTCUT_FAILED (WM_USER + 1006)

struct ExtractProgress {
  std::wstring item;
  unsigned int cur;
  unsigned int total;
};

class DlgWarning;
class DlgOption;
class DlgProgress : public WindowImplBase {
 public:
  DlgProgress(std::function<void(HWND)> wndCreatedCB);
  ~DlgProgress();

 protected:
  CDuiString GetSkinFile() override;
  LPCTSTR GetWindowClassName(void) const override;
  void Notify(TNotifyUI& msg) override;
  LRESULT ResponseDefaultKeyEvent(WPARAM wParam) override;
  LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
  LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) override;
  void InitWindow() override;

 private:
  void OnUserCancelInstall();

 private:
  CLabelUI* pLblTitle_;
  CProgressUI* pProgress_;
  CLabelUI* pLblProgress_;
  CLabelUI* pLblDetail_;
  CLabelUI* pLblImage_;
  CButtonUI* pBtnMin_;
  CButtonUI* pBtnClose_;

  std::function<void(HWND)> wndCreatedCB_;
};
#endif  // !INSTALLER_DLG_PROGRESS_H_