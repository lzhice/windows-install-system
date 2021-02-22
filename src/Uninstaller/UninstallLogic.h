#pragma once

class UninstallLogic {
public:
  UninstallLogic(HINSTANCE hInst, bool bSilent);
  ~UninstallLogic();

  void Run();
private:
  void DoSilentUninstall();
  void DoUnSilentUninstall();

  bool RemoveFile(HWND hProgressWnd);
  bool RemoveShortcut(HWND hProgressWnd);
  bool RemoveRegister(HWND hProgressWnd);
  bool RemoveUninstallInfo(HWND hProgressWnd);
  bool RemoveInstallRecord(HWND hProgressWnd);
  bool RemoveSelf();
private:
  HINSTANCE hInst_;
  bool bSilent_;
  std::thread workThread_;
};