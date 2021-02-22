#pragma once

#include <string>
#include <functional>

namespace easyzip {
class Unzipper;
}
class ZipResource {
 public:
  ZipResource(int resourceId);
  ~ZipResource();

  void OpenZip();
  void CloseZip();

  bool ExtractToFolder(const std::wstring& strFolder,
                       std::function<void(std::wstring, unsigned int, unsigned int)> cb);

 private:
  int resourceId_;
  void* pResData_;
  unsigned int uResSize_;

  unsigned int uItemTotal_;
  unsigned int uItemCur_;

  easyzip::Unzipper* pUnzipper_;
};