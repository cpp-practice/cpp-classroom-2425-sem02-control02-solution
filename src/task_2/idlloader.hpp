#pragma once

#include <memory>
#include <string>

namespace control_02::dlloader {
/*
** Interface for Dynamic Library Loading (DLLoader)
** API for Unix and Windows. Handling of open, close, validity-check.
*/
class IDLLoader {
 public:
  virtual ~IDLLoader() = default;

  virtual void *get_symbol(const std::string& sym_name) = 0;

  template<class T>
  T get_func(const std::string& sym_name) {
    return reinterpret_cast<T>(get_symbol(sym_name));
  }
};

}  // control_02::dlloader