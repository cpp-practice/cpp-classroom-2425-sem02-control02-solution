#pragma once
#include "idlloader.hpp"
#include <dlfcn.h>
#include <stdexcept>

namespace control_02::dlloader {
class DLLoader : public IDLLoader {
  void *lib;

public:
  DLLoader(std::string const &path_to_lib) {
    lib = dlopen(path_to_lib.c_str(), RTLD_LAZY);
    if (lib == nullptr) {
      throw std::runtime_error("cannot open lib: " + path_to_lib);
    }
  }

  ~DLLoader() {
    dlclose(lib);
    lib = nullptr;
  }

  void *get_symbol(const std::string &symname) override {
    void *sym = dlsym(lib, symname.c_str());
    if (sym == nullptr) {
      throw std::runtime_error("unknown symbol: " + symname);
    }
    return sym;
  }
};

} // namespace control_02::dlloader
