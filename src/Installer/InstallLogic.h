#pragma once

#include <string>
#include <thread>
#include "json/json.h"

class InstallLogic {
public:
  InstallLogic(HINSTANCE hInst, bool bSilent, const std::wstring &installFolder);
  ~InstallLogic();

  void Run();
private:
  void DoSilentInstall();
  void DoUnSilentInstall();

  bool CreateInstallFolder(HWND hProgressWnd);
  bool ExtractFiles(HWND hProgressWnd);
  bool ExtractUninst(HWND hProgressWnd);
  bool CreateUninstallInfo(HWND hProgressWnd);
  bool CreateShortcut(HWND hProgressWnd);
  bool CreateRegister(HWND hProgressWnd);
  bool WriteInstallRecord(HWND hProgressWnd);
  bool ExecuteCmd(HWND hProgressWnd);
  bool IsUrl(const std::wstring &str);
private:
  HINSTANCE hInst_;
  bool bSilent_;
  std::wstring strInstallFolder_;
  std::thread workThread_;
  Json::Value root_;
};