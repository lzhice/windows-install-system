#ifndef INSTALL_RECORD_H_
#define INSTALL_RECORD_H_
#pragma once

#include "akali/singleton.hpp"
#include <string>
#include <vector>

struct RegInfo {
  std::wstring rootKey;
  std::wstring subKey;
  std::wstring item;
  bool WOW6432Node;

  RegInfo() { WOW6432Node = false; }
};

class InstallRecord : public akali::Singleton<InstallRecord> {
 public:
  ~InstallRecord();

  const std::vector<std::wstring>& GetShortcuts();
  const std::vector<std::wstring>& GetFiles();
  const std::vector<RegInfo>& GetRegInfos();
  std::wstring GetUninstallRegKeyName();

  bool RemoveRecordFile();
 private:
  InstallRecord();
  void LoadJSON();

  std::wstring uninstallRegKeyName_;
  std::vector<std::wstring> shortcuts_;
  std::vector<std::wstring> files_;
  std::vector<RegInfo> reginfos_;

  SINGLETON_CLASS_DECLARE(InstallRecord);
};

#endif  // !INSTALL_RECORD_H_