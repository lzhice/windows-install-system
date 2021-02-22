#include "StdAfx.h"
#include "InstallRecord.h"
#include "resource.h"
#include "json/json.h"
#include "akali/process.h"
#include "akali/stringencode.h"

using namespace akali;
#define RECORD_FILE_NAME L"install_record.dat"

InstallRecord::InstallRecord() {
  LoadJSON();
}

InstallRecord::~InstallRecord() {}

const std::vector<std::wstring>& InstallRecord::GetShortcuts() {
  return shortcuts_;
}

const std::vector<std::wstring>& InstallRecord::GetFiles() {
  return files_;
}

const std::vector<RegInfo>& InstallRecord::GetRegInfos() {
  return reginfos_;
}

std::wstring InstallRecord::GetUninstallRegKeyName() {
  return uninstallRegKeyName_;
}

bool InstallRecord::RemoveRecordFile() {
  std::wstring path = akali::Process::GetSelfDir() + RECORD_FILE_NAME;
  if (_waccess_s(path.c_str(), 0) != 0) {
    return true;
  }
  return DeleteFileW(path.c_str());
}

void InstallRecord::LoadJSON() {
  FILE* f = NULL;
  _wfopen_s(&f, (akali::Process::GetSelfDir() + RECORD_FILE_NAME).c_str(), L"rb");
  if (!f) {
    return;
  }
  
  fseek(f, 0, SEEK_END);
  long fileSize = ftell(f);
  fseek(f, 0, SEEK_SET);
  char* pData = (char*)malloc(fileSize);
  if (!pData) {
    fclose(f);
    return;
  }
  size_t hasRead = fread(pData, 1, fileSize, f);
  if (hasRead != fileSize) {
    fclose(f);
    free(pData);
    return;
  }
  fclose(f);

  Json::Value root;
  Json::CharReaderBuilder readerBuilder;
  Json::CharReader* reader = readerBuilder.newCharReader();
  Json::String err;
  if (!reader->parse((const char*)pData, ((const char*)pData + fileSize), &root, &err)) {
    TraceMsgA("install_record.dat parse failed: %s\n", err.c_str());
    free(pData);
    return;
  }

  try {
    if (root.isMember("file") && root["file"].isArray()) {
      for (Json::ArrayIndex i = 0; i < root["file"].size(); i++) {
        files_.push_back(Utf8ToUnicode(root["file"][i].asString()));
      }
    }
  } catch (std::exception& e) {
    TraceMsgA("install_record.dat parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("shortcut") && root["shortcut"].isArray()) {
      for (Json::ArrayIndex i = 0; i < root["shortcut"].size(); i++) {
        shortcuts_.push_back(Utf8ToUnicode(root["shortcut"][i].asString()));
      }
    }
  } catch (std::exception& e) {
    TraceMsgA("install_record.dat parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("register") && root["register"].isArray()) {
      for (Json::ArrayIndex i = 0; i < root["register"].size(); i++) {
        Json::Value tmp = root["register"][i];
        RegInfo ri;
        ri.rootKey = Utf8ToUnicode(tmp["root_key"].asString());
        ri.subKey = Utf8ToUnicode(tmp["sub_key"].asString());
        ri.item = Utf8ToUnicode(tmp["item"].asString());
        ri.WOW6432Node = tmp["WOW6432Node"].asBool();

        reginfos_.push_back(ri);
      }
    }
  } catch (std::exception& e) {
    TraceMsgA("install_record.dat parse exception: %s\n", e.what() ? e.what() : "");
  }

  try {
    if (root.isMember("uninstall")) {
      uninstallRegKeyName_ = Utf8ToUnicode(root["uninstall"]["reg_key_name"].asString());
    }
  } catch (std::exception& e) {
    TraceMsgA("install_record.dat parse exception: %s\n", e.what() ? e.what() : "");
  }

  free(pData);
}
