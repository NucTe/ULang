#ifndef ULANG_UTILITIES_H_
#define ULANG_UTILITIES_H_

#include <iostream>
#include <optional>
#include <memory>

namespace UraniumLang {

  template <typename T>
  using uptr = std::unique_ptr<T>;

}

#endif