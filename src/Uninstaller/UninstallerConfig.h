#ifndef UNINSTALLER_CONFIG_H_
#define UNINSTALLER_CONFIG_H_
#pragma once

#include "akali/singleton.hpp"
#include <string>
#include <vector>

struct CoreConfig {
  std::wstring productName;
  std::wstring setupVersion;
};

struct OptionDialogConfig {
  std::wstring title;
};

struct ProgressDialogConfig {
  std::wstring title;
};

struct FinishDialogConfig {
  std::wstring title;
};


class UninstallerConfig : public akali::Singleton<UninstallerConfig> {
 public:
  ~UninstallerConfig();

  CoreConfig GetCoreCfg();
  OptionDialogConfig GetOptionDlgCfg();
  ProgressDialogConfig GetProgressDlgCfg();
  FinishDialogConfig GetFinishDlgCfg();

 private:
   UninstallerConfig();
  void LoadJSON();

  CoreConfig coreConfig_;
  OptionDialogConfig optionDialogConfig_;
  ProgressDialogConfig progressDialogConfig_;
  FinishDialogConfig finishDialogConfig_;

  SINGLETON_CLASS_DECLARE(UninstallerConfig);
};

#endif  // !UNINSTALLER_CONFIG_H_