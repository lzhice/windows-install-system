/*******************************************************************************
 * Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Expect bugs
 *
 * Please use and enjoy. Please let me know of any bugs/improvements
 * that you have found/implemented and I will fix/incorporate them into this
 * file.
 *******************************************************************************/

#ifndef AKALI_CMDLIBE_PARSE_H__
#define AKALI_CMDLIBE_PARSE_H__

#include <map>
#include <string>
using namespace std;
#include "akali/akali_export.h"

namespace akali {
  class AKALI_API CmdLineParser {
  public:
    typedef std::map<wstring, wstring> ValsMap;
    typedef ValsMap::const_iterator ITERPOS;

    explicit CmdLineParser(const wstring& cmdline);
    ~CmdLineParser();

    ITERPOS Begin() const;
    ITERPOS End() const;

    bool HasKey(const wstring& key) const;
    bool HasVal(const wstring& key) const;
    wstring GetVal(const wstring& key) const;

    int GetParamCount() const;

  private:
    bool Parse(const wstring& cmdline);

    wstring cmdline_;
    class Impl;
    Impl* impl_;
  };
}

#endif