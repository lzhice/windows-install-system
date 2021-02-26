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

struct ExecutorConfig {
  bool waitExit;
  std::wstring cmd;
  std::wstring workingDir;
  std::wstring parameter;

  ExecutorConfig() { waitExit = false; }
};


class UninstallerConfig : public akali::Singleton<UninstallerConfig> {
 public:
  ~UninstallerConfig();

  CoreConfig GetCoreCfg();
  OptionDialogConfig GetOptionDlgCfg();
  ProgressDialogConfig GetProgressDlgCfg();
  FinishDialogConfig GetFinishDlgCfg();
  std::vector<ExecutorConfig> GetExecutorCfgs();

 private:
   UninstallerConfig();
  void LoadJSON();

  CoreConfig coreConfig_;
  OptionDialogConfig optionDialogConfig_;
  ProgressDialogConfig progressDialogConfig_;
  FinishDialogConfig finishDialogConfig_;
  std::vector<ExecutorConfig> executorConfigs_;

  SINGLETON_CLASS_DECLARE(UninstallerConfig);
};

#endif  // !UNINSTALLER_CONFIG_H_