#include "StdAfx.h"
#include "UninstallerConfig.h"
#include "resource.h"
#include "json/json.h"
#include "akali/stringencode.h"

using namespace akali;

UninstallerConfig::UninstallerConfig() {
  LoadJSON();
}

UninstallerConfig::~UninstallerConfig() {}

CoreConfig UninstallerConfig::GetCoreCfg() {
  return coreConfig_;
}

OptionDialogConfig UninstallerConfig::GetOptionDlgCfg() {
  return optionDialogConfig_;
}

ProgressDialogConfig UninstallerConfig::GetProgressDlgCfg() {
  return progressDialogConfig_;
}

FinishDialogConfig UninstallerConfig::GetFinishDlgCfg() {
  return finishDialogConfig_;
}

void UninstallerConfig::LoadJSON() {
  HRSRC hResource = FindResource(GetModuleHandle(NULL),
                                 MAKEINTRESOURCE(IDR_JSON_UNINSTALLER_CONFIG), TEXT("JSON"));
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
  } catch (std::exception& e) {
    TraceMsgA("installer config json parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("ui")) {
      if (root["ui"].isMember("option_dialog")) {
        optionDialogConfig_.title = Utf8ToUnicode(root["ui"]["option_dialog"]["title"].asString());
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
}
