#include "_dir.h"
#include <ShlObj.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

std::wstring GetCurrentUserDesktopDir() {
  wchar_t szDesktopDir[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_DESKTOPDIRECTORY, 0);
  PathAddBackslashW(szDesktopDir);

  return szDesktopDir;
}

std::wstring GetAllUserDesktopDir() {
  wchar_t szDesktopDir[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_COMMON_DESKTOPDIRECTORY, 0);
  PathAddBackslashW(szDesktopDir);

  return szDesktopDir;
}

std::wstring GetCurrentUserProgramsDir() {
  wchar_t szDesktopDir[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_PROGRAMS, 0);
  PathAddBackslashW(szDesktopDir);

  return szDesktopDir;
}

std::wstring GetAllUserProgramsDir() {
  wchar_t szDesktopDir[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_COMMON_PROGRAMS, 0);
  PathAddBackslashW(szDesktopDir);

  return szDesktopDir;
}

std::wstring GetCurrentUserAppDataDir() {
  wchar_t szDesktopDir[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_APPDATA, 0);
  PathAddBackslashW(szDesktopDir);

  return szDesktopDir;
}

std::wstring GetAllUserAppDataDir() {
  wchar_t szDesktopDir[MAX_PATH] = {0};
  SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_COMMON_APPDATA, 0);
  PathAddBackslashW(szDesktopDir);

  return szDesktopDir;
}
