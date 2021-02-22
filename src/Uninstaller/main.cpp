#include "StdAfx.h"
#include "UninstallLogic.h"
#include "akali/cmdline_parse.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR lpCmdLine,
                       int nCmdShow) {
  CoInitialize(NULL);
  bool bSilent = false;
  akali::CmdLineParser clp(GetCommandLineW());
  if (clp.HasKey(L"S")) {
    bSilent = true;
  }

  UninstallLogic uninstallLogic(hInstance, bSilent);
  uninstallLogic.Run();
  ::CoUninitialize();
  return 0;
}