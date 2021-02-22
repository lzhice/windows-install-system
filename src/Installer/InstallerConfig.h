#ifndef INSTALLER_CONFIG_H_
#define INSTALLER_CONFIG_H_
#pragma once

#include "akali/singleton.hpp"
#include <string>
#include <vector>

struct CoreConfig {
  std::wstring productName;
  std::wstring setupVersion;
  std::wstring defaultInstallFolder;
};

struct OptionDialogConfig {
  std::wstring title;
  std::wstring installFolderSuffix;
  std::wstring needDiskSpaceByMB;
};

struct ProgressDialogConfig {
  std::wstring title;
};

struct FinishDialogConfig {
  std::wstring title;
};

struct ShortcutConfig {
  bool uninstallIncluded;
  std::wstring lnkPath;
  std::wstring iconPath;
  std::wstring targetPath;
  std::wstring workingDir;

  ShortcutConfig() {
    uninstallIncluded = true;
  }
};


struct RegValue {
  unsigned int type; // REG_SZ REG_DWORD
  std::wstring v1;
  DWORD v2;

  RegValue() {
    type = REG_NONE;
    v2 = 0;
  }
};

struct RegisterConfig {
  bool uninstallIncluded;
  std::wstring rootKey;
  std::wstring subKey;
  std::wstring item;
  bool WOW6432Node;
  RegValue value;

  RegisterConfig() {
    uninstallIncluded = true;
    WOW6432Node = false;
  }
};

struct UninstallConfig {
  std::wstring regKeyName;
  std::wstring displayName;
  std::wstring publisher;
  std::wstring aboutLinkUrl;
  std::wstring helpLinkUrl;
  std::wstring uninstallExePath;
};

class InstallerConfig : public akali::Singleton<InstallerConfig> {
 public:
  ~InstallerConfig();

  CoreConfig GetCoreCfg();
  OptionDialogConfig GetOptionDlgCfg();
  ProgressDialogConfig GetProgressDlgCfg();
  FinishDialogConfig GetFinishDlgCfg();
  std::vector<ShortcutConfig> GetShortcutConfigs();
  std::vector<RegisterConfig> GetRegisterCfgs();
  
  UninstallConfig GetUninstallCfg();

 private:
  InstallerConfig();
  void LoadJSON();

  CoreConfig coreConfig_;
  OptionDialogConfig optionDialogConfig_;
  ProgressDialogConfig progressDialogConfig_;
  FinishDialogConfig finishDialogConfig_;
  UninstallConfig uninstallConfig_;
  std::vector<ShortcutConfig> shortcutConfigs_;
  std::vector<RegisterConfig> registerConfigs_;

  SINGLETON_CLASS_DECLARE(InstallerConfig);
};

#endif  // !INSTALLER_CONFIG_H_