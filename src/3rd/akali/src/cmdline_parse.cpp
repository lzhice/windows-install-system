#include "akali/cmdline_parse.h"
#include "akali/macros.h"
#include <locale>
#include <algorithm>
#if (defined _WIN32 || defined WIN32)
#include <tchar.h>
#endif
#include <map>
#include "akali/string_helper.hpp"
#include "akali/stringencode.h"

namespace akali {
  class CmdLineParser::Impl {
  public:
    Impl() { value_map_.clear(); }

    ~Impl() { value_map_.clear(); }

    ITERPOS findKey(const wstring& key) const {
      wstring s = akali::StringCaseConvert(key, akali::EasyCharToLowerW);
      return value_map_.find(s);
    }

    CmdLineParser::ValsMap value_map_;
  };

  const wchar_t delims[] = L"-/";
  const wchar_t quotes[] = L"\"";
  const wchar_t value_sep[] = L" :";  // don't forget space!!

  CmdLineParser::CmdLineParser(const wstring& cmdline) {
    impl_ = new Impl();

    if (cmdline.length() > 0) {
      Parse(cmdline);
    }
  }

  CmdLineParser::~CmdLineParser() {
    SAFE_DELETE(impl_);
  }

  bool CmdLineParser::Parse(const wstring& cmdline) {
    const wstring sEmpty = L"";
    int nArgs = 0;

    impl_->value_map_.clear();
    cmdline_ = cmdline;

    wstring strW = cmdline_;
    const wchar_t* sCurrent = strW.c_str();

    for (;;) {
      if (sCurrent[0] == L'\0')
        break;

      // 查找任一分隔符
      const wchar_t* sArg = wcspbrk(sCurrent, delims);

      if (!sArg)
        break;

      sArg++;  // 字符指针sArg向后移动一个字符

      if (sArg[0] == L'\0')
        break;  // ends with delim

      const wchar_t* sVal = wcspbrk(sArg, value_sep);

      if (sVal == NULL) {
        wstring Key(sArg);
        Key = akali::StringCaseConvert(Key, akali::EasyCharToLowerW);
        impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, sEmpty));
        break;
      } else if (sVal[0] == L' ' || wcslen(sVal) == 1) {
        // cmdline ends with /Key: or a key with no value
        wstring Key(sArg, (int)(sVal - sArg));

        if (Key.length() > 0) {
          Key = akali::StringCaseConvert(Key, akali::EasyCharToLowerW);
          impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, sEmpty));
        }

        sCurrent = sVal + 1;
        continue;
      } else {
        // key has value
        wstring Key(sArg, (int)(sVal - sArg));
        Key = akali::StringCaseConvert(Key, akali::EasyCharToLowerW);

        sVal++;

        const wchar_t* sQuote = wcspbrk(sVal, quotes);
        const wchar_t* sEndQuote = NULL;

        if (sQuote == sVal) {
          // string with quotes (defined in quotes, e.g. '")
          sQuote = sVal + 1;
          sEndQuote = wcspbrk(sQuote, quotes);
        } else {
          sQuote = sVal;
          sEndQuote = wcschr(sQuote, L' ');
        }

        if (sEndQuote == NULL) {
          // no end quotes or terminating space, take the rest of the string to its end
          wstring csVal(sQuote);

          if (Key.length() > 0) {
            impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
          }

          break;
        } else {
          // end quote
          if (Key.length() > 0) {
            wstring csVal(sQuote, (int)(sEndQuote - sQuote));
            impl_->value_map_.insert(CmdLineParser::ValsMap::value_type(Key, csVal));
          }

          sCurrent = sEndQuote + 1;
          continue;
        }
      }
    }

    return (nArgs > 0);  // TRUE if arguments were found
  }

  bool CmdLineParser::HasKey(const wstring& key) const {
    ITERPOS it = impl_->findKey(key);

    if (it == impl_->value_map_.end())
      return false;

    return true;
  }

  bool CmdLineParser::HasVal(const wstring& key) const {
    ITERPOS it = impl_->findKey(key);

    if (it == impl_->value_map_.end())
      return false;

    if (it->second.length() == 0)
      return false;

    return true;
  }

  wstring CmdLineParser::GetVal(const wstring& key) const {
    ITERPOS it = impl_->findKey(key);

    if (it == impl_->value_map_.end())
      return wstring();

    return it->second;
  }

  int CmdLineParser::GetParamCount() const {
    return impl_->value_map_.size();
  }

  CmdLineParser::ITERPOS CmdLineParser::Begin() const {
    return impl_->value_map_.begin();
  }

  CmdLineParser::ITERPOS CmdLineParser::End() const {
    return impl_->value_map_.end();
  }
}