#include "StdAfx.h"
#include "InstallerConfig.h"
#include "resource.h"
#include "json/json.h"
#include "akali/stringencode.h"

using namespace akali;

InstallerConfig::InstallerConfig() {
  LoadJSON();
}

InstallerConfig::~InstallerConfig() {}

CoreConfig InstallerConfig::GetCoreCfg() {
  return coreConfig_;
}

OptionDialogConfig InstallerConfig::GetOptionDlgCfg() {
  return optionDialogConfig_;
}

ProgressDialogConfig InstallerConfig::GetProgressDlgCfg() {
  return progressDialogConfig_;
}

FinishDialogConfig InstallerConfig::GetFinishDlgCfg() {
  return finishDialogConfig_;
}

std::vector<ShortcutConfig> InstallerConfig::GetShortcutConfigs() {
  return shortcutConfigs_;
}

std::vector<RegisterConfig> InstallerConfig::GetRegisterCfgs() {
  return registerConfigs_;
}

std::vector<ExecutorConfig> InstallerConfig::GetExecutorCfgs() {
  return executorConfigs_;
}

UninstallConfig InstallerConfig::GetUninstallCfg() {
  return uninstallConfig_;
}

void InstallerConfig::LoadJSON() {
  HRSRC hResource =
      FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_JSON_INSTALLER_CONFIG), TEXT("JSON"));
  if (!hResource) {
    TraceMsgA("load json resource failed(FindResource), GLE: %d\n", GetLastError());
    return;
  }

  HGLOBAL hg = LoadResource(GetModuleHandle(NULL), hResource);
  if (!hg) {
    TraceMsgA("load json resource failed(LoadResource), GLE: %d\n", GetLastError());
    return;
  }

  void* pData = LockResource(hg);
  DWORD dwSize = SizeofResource(GetModuleHandle(NULL), hResource);

  if (!pData || dwSize == 0) {
    TraceMsgA("load json resource failed(LockResource), GLE: %d\n", GetLastError());
    return;
  }

  std::string strJson;
  strJson.assign((const char*)pData, dwSize);

  Json::Value root;
  Json::CharReaderBuilder readerBuilder;
  Json::CharReader* reader = readerBuilder.newCharReader();
  Json::String err;
  if (!reader->parse((const char*)pData, ((const char*)pData + dwSize), &root, &err)) {
    TraceMsgA("installer config json parse failed: %s\n", err.c_str());
    return;
  }

  try {
    coreConfig_.productName = Utf8ToUnicode(root["core"]["product_name"].asString());
    coreConfig_.setupVersion = Utf8ToUnicode(root["core"]["setup_version"].asString());
    coreConfig_.defaultInstallFolder =
        Utf8ToUnicode(root["core"]["default_install_folder"].asString());
  } catch (std::exception& e) {
    TraceMsgA("installer config json parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("ui")) {
      if (root["ui"].isMember("option_dialog")) {
        if (root["ui"]["option_dialog"].isMember("install_folder_suffix"))
          optionDialogConfig_.installFolderSuffix =
              Utf8ToUnicode(root["ui"]["option_dialog"]["install_folder_suffix"].asString());
        optionDialogConfig_.needDiskSpace =
            Utf8ToUnicode(root["ui"]["option_dialog"]["need_disk_space"].asString());
        optionDialogConfig_.title = Variable::Instance()->Parse(
            Utf8ToUnicode(root["ui"]["option_dialog"]["title"].asString()));
      }

      if (root["ui"].isMember("progress_dialog")) {
        progressDialogConfig_.title =
            Utf8ToUnicode(root["ui"]["progress_dialog"]["title"].asString());
      }

      if (root["ui"].isMember("finish_dialog")) {
        finishDialogConfig_.title = Utf8ToUnicode(root["ui"]["finish_dialog"]["title"].asString());
      }
    }
  } catch (std::exception& e) {
    TraceMsgA("installer config json parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("shortcut") && root["shortcut"].isArray()) {
      for (Json::ArrayIndex i = 0; i < root["shortcut"].size(); i++) {
        Json::Value tmp = root["shortcut"][i];
        ShortcutConfig sc;
        sc.lnkPath = Utf8ToUnicode(tmp["lnk_path"].asString());
        sc.iconPath = Utf8ToUnicode(tmp["icon_path"].asString());
        sc.targetPath = Utf8ToUnicode(tmp["target_path"].asString());
        sc.workingDir = Utf8ToUnicode(tmp["working_dir"].asString());
        if (tmp.isMember("uninstall_included")) {
          sc.uninstallIncluded = tmp["uninstall_included"].asBool();
        }

        shortcutConfigs_.push_back(sc);
      }
    }
  } catch (std::exception& e) {
    TraceMsgA("installer config json parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("register") && root["register"].isArray()) {
      for (Json::ArrayIndex i = 0; i < root["register"].size(); i++) {
        Json::Value tmp = root["register"][i];
        RegisterConfig rc;
        rc.rootKey = Utf8ToUnicode(tmp["root_key"].asString());
        rc.subKey = Utf8ToUnicode(tmp["sub_key"].asString());
        rc.item = Utf8ToUnicode(tmp["item"].asString());
        rc.WOW6432Node = tmp["WOW6432Node"].asBool();
        if (tmp["value"].isString()) {
          rc.value.type = REG_SZ;
          rc.value.v1 = Utf8ToUnicode(tmp["value"].asCString());
        }
        else if (tmp["value"].isNumeric()) {
          rc.value.type = REG_DWORD;
          rc.value.v2 = (DWORD)tmp["value"].asInt64();
        }

        if (tmp.isMember("uninstall_included")) {
          rc.uninstallIncluded = tmp["uninstall_included"].asBool();
        }

        registerConfigs_.push_back(rc);
      }
    }
  } catch (std::exception& e) {
    TraceMsgA("installer config json parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("executor") && root["executor"].isArray()) {
      for (Json::ArrayIndex i = 0; i < root["executor"].size(); i++) {
        Json::Value tmp = root["executor"][i];
        ExecutorConfig ec;
        ec.cmd = Utf8ToUnicode(tmp["cmd"].asString());
        ec.workingDir = Utf8ToUnicode(tmp["working_dir"].asString());
        ec.parameter = Utf8ToUnicode(tmp["parameter"].asString());
        ec.waitExit = tmp["wait_exit"].asBool();
        executorConfigs_.push_back(ec);
      }
    }
  } catch (std::exception& e) {
    TraceMsgA("installer config json parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("uninstall")) {
      uninstallConfig_.regKeyName = Utf8ToUnicode(root["uninstall"]["reg_key_name"].asString());
      uninstallConfig_.displayName = Utf8ToUnicode(root["uninstall"]["display_name"].asString());
      uninstallConfig_.publisher = Utf8ToUnicode(root["uninstall"]["publisher"].asString());
      uninstallConfig_.aboutLinkUrl = Utf8ToUnicode(root["uninstall"]["about_link_url"].asString());
      uninstallConfig_.helpLinkUrl = Utf8ToUnicode(root["uninstall"]["help_link_url"].asString());
      uninstallConfig_.uninstallExePath =
          Utf8ToUnicode(root["uninstall"]["uninstall_exe_path"].asString());
    }
  } catch (std::exception& e) {
    TraceMsgA("installer config json parse exception: %s\n", e.what() ? e.what() : "");
  }
}
