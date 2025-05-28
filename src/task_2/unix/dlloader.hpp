#pragma once

#include "idlloader.hpp"

namespace control_02::dlloader {
class DLLoader : public IDLLoader {
 public:
  DLLoader(std::string const &path_to_lib) {}

  ~DLLoader() {}

  void* get_symbol(const std::string& symname) override {}
};

}  // control_02::dlloader
