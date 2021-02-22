#include "_filename.h"
#include <windows.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

std::wstring GetCurrentExeFullName() {
  wchar_t szPath[MAX_PATH] = {0};
  GetModuleFileNameW(NULL, szPath, MAX_PATH);
  PathStripPath(szPath);
  return szPath;
}

std::wstring GetCurrentExeFileName() {
  wchar_t szPath[MAX_PATH] = {0};
  GetModuleFileNameW(NULL, szPath, MAX_PATH);
  PathStripPath(szPath);
  PathRemoveExtension(szPath);

  return szPath;
}

std::wstring GetUninstallExeFullName() {
  return L"Uninst.exe";
}
