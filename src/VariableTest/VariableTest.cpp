#include <iostream>
#include "../Variable/variables.h"

int main()
{
  Variable variable;
  std::vector<std::wstring> allVar = variable.GetAllVariableNames();
  for (auto i : allVar) {
    std::wcout << i << std::endl;
  }

  std::wstring strSrc = L"%current_user_desktop%test.lnk";
  std::wcout << variable.Parse(strSrc) << std::endl;

  std::wstring strSrc1 = L"%%all_user_desktop%%%current_user_desktop%test.lnk%test%";
  std::wcout << variable.Parse(strSrc1) << std::endl;
}