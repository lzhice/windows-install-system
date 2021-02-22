#ifndef VARIABLES_H_
#define VARIABLES_H_
#pragma once

// All variables is:
// 1. lowercase
// 2. %% stand by %

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "akali/singleton.hpp"

class Variable : public akali::Singleton<Variable>  {
public:
  ~Variable();

  void SetInstallFolder(const std::wstring &folder);
  void SetSilentMode(bool bSilent);

  std::vector<std::wstring> GetAllVariableNames();
  bool IsVariableSupport(const std::wstring& varibaleName);
  bool GetVariableValue(const std::wstring& variableName, std::wstring& value);
  std::wstring Parse(const std::wstring& src);
private:
  void InitVars();
private:
  Variable();
  typedef std::function<std::wstring()> VarFunc;

  bool bSilent_;
  std::wstring strInstallFolder_;
  std::map<std::wstring, VarFunc> allVars_;

  SINGLETON_CLASS_DECLARE(Variable);
};

#define PARSE(x) Variable::Instance()->Parse(x)

#endif // !VARIABLES_H_