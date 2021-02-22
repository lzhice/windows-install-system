#include "variables.h"
#include "_dir.h"
#include "_filename.h"

Variable::Variable() : bSilent_(false) {
  InitVars();
}

Variable::~Variable() {}

void Variable::SetInstallFolder(const std::wstring& folder) {
  strInstallFolder_ = folder;
}

void Variable::SetSilentMode(bool bSilent) {
  bSilent_ = bSilent;
}

void Variable::InitVars() {
  allVars_[L"current_user_desktop"] = GetCurrentUserDesktopDir;
  allVars_[L"all_user_desktop"] = GetAllUserDesktopDir;

  allVars_[L"current_user_programs"] = GetCurrentUserProgramsDir;
  allVars_[L"all_user_programs"] = GetAllUserProgramsDir;

  allVars_[L"current_user_appdata"] = GetCurrentUserAppDataDir;
  allVars_[L"all_user_appdata"] = GetAllUserAppDataDir;

  allVars_[L"install_folder"] = [this]() -> std::wstring {
    return strInstallFolder_;
  };

  allVars_[L"cur_fullname"] = GetCurrentExeFullName;
  allVars_[L"cur_filename"] = GetCurrentExeFileName;
  allVars_[L"uninstaller_fullname"] = GetUninstallExeFullName;
}

std::vector<std::wstring> Variable::GetAllVariableNames() {
  std::vector<std::wstring> vNames;
  for (auto& i : allVars_) {
    vNames.push_back(i.first);
  }
  return vNames;
}

bool Variable::IsVariableSupport(const std::wstring& varibaleName) {
  for (auto& i : allVars_) {
    if (i.first == varibaleName) {
      return true;
    }
  }
  return false;
}

bool Variable::GetVariableValue(const std::wstring& variableName, std::wstring& value) {
  for (auto& i : allVars_) {
    if (i.first == variableName) {
      value = i.second();
      return true;
    }
  }

  return false;
}

std::wstring Variable::Parse(const std::wstring& src) {
  std::wstring ret;
  const size_t len = src.length();

  int varBeginPos = -1;

  for (size_t i = 0; i < len; i++) {
    if (src[i] == L'%') {
      if (varBeginPos == -1) {
        varBeginPos = i;
      }
      else {
        std::wstring var = src.substr(varBeginPos + 1, i - varBeginPos - 1);
        std::wstring value;
        if (GetVariableValue(var, value)) {
          ret += value;
        }
        else {
          std::wstring tmp = src.substr(varBeginPos, i - varBeginPos + 1);
          if (tmp == L"%%") {
            ret += L"%";
          }
          else {
            ret += tmp;
          }
        }

        varBeginPos = -1; // reset
      }
    }
    else if (varBeginPos == -1) {
      ret += src[i];
    }
  }

  return ret;
}