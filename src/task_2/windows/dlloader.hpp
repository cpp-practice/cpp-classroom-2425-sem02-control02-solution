#pragma once

#include <sstream>
#include "Windows.h"
#include "idlloader.hpp"

namespace control_02::dlloader {
class DLLoader : public IDLLoader {
 private:
  HMODULE	handle_;
  std::string	path_to_lib_;

 public:
  DLLoader(std::string const &path_to_lib) :
          handle_(nullptr), path_to_lib_(path_to_lib) {
    if (!(handle_ = LoadLibrary(path_to_lib_.c_str()))) {
      std::stringstream ss;
      ss << "Can't open and load " << path_to_lib_;
      throw std::runtime_error(ss.str());
    }
  }

  ~DLLoader() {
    if (FreeLibrary(handle_) == 0) {
      std::cerr << "Can't close " << path_to_lib_ << std::endl;
    }
  }

  void* get_symbol(const std::string& symname) override {
    return GetProcAddress(handle_, symname.c_str());
  }
};
} 