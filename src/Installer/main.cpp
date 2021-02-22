#include "StdAfx.h"
#include "InstallLogic.h"
#include "akali/cmdline_parse.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR lpCmdLine,
                       int nCmdShow) {
  CoInitialize(NULL);
  bool bSilent = false;
  std::wstring strInstallFolder;

  akali::CmdLineParser clp(GetCommandLineW());
  if (clp.HasKey(L"S")) {
    bSilent = true;
  }

  if (clp.HasKey(L"D")) {
    strInstallFolder = clp.GetVal(L"D");
  }
  if (strInstallFolder.length() == 0) {
    strInstallFolder = PARSE(InstallerConfig::Instance()->GetCoreCfg().defaultInstallFolder);
  }

  InstallLogic installLogic(hInstance, bSilent, strInstallFolder);
  installLogic.Run();
  ::CoUninitialize();
  return 0;
}