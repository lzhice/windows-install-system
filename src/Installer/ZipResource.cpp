#include "StdAfx.h"
#include "ZipResource.h"
#include "easyzip/easyzip.h"

ZipResource::ZipResource(int resourceId)
    : resourceId_(resourceId)
    , pResData_(NULL)
    , uResSize_(0)
    , pUnzipper_(NULL)
    , uItemTotal_(0)
    , uItemCur_(0) {
  HRSRC hResource =
      FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId_), TEXT("ZIPRES"));
  if (hResource) {
    HGLOBAL hg = LoadResource(GetModuleHandle(NULL), hResource);
    if (hg) {
      pResData_ = LockResource(hg);
      uResSize_ = SizeofResource(GetModuleHandle(NULL), hResource);
    }
  }
}

ZipResource::~ZipResource() {
  CloseZip();
}

void ZipResource::OpenZip() {
  try {
    pUnzipper_ = new easyzip::Unzipper(pResData_, uResSize_);
    uItemTotal_ = pUnzipper_->entries().size();
  } catch (std::exception& e) {
    akali::TraceMsgA("open zip exception: %s\n", e.what() ? e.what() : "");
  }
}

void ZipResource::CloseZip() {
  if (pUnzipper_) {
    pUnzipper_->close();
    delete pUnzipper_;
    pUnzipper_ = NULL;
  }
}

bool ZipResource::ExtractToFolder(
    const std::wstring& strFolder,
    std::function<void(std::wstring, unsigned int, unsigned int)> cb) {
  return pUnzipper_->extract(strFolder, [cb, this](std::wstring item) {
    if (cb) {
      cb(item, ++uItemCur_, uItemTotal_);
    }
  });
}
