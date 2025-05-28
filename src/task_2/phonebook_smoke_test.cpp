#include "phonebook.hpp"
#include "phonebook.hpp"

#if defined(__linux__) || defined(__APPLE__)  
  #include "unix/dlloader.hpp"
#endif

#ifdef WIN32
  #include "windows/dlloader.hpp"
#endif


#include <sstream>
#include <stdexcept>
#include <vector>

#include "../simple_test.h"


class PhoneBookTestRunner {
  std::map<std::string, std::string> format_to_lib;
  std::string lib_dir = PARSER_LIBRARY_DIR;


 public:

  PhoneBookTestRunner() {
#if defined(__APPLE__)
    format_to_lib["json"] = lib_dir + "/libjson-parser.dylib";
#endif

#if defined(__linux__)
    format_to_lib["json"] = lib_dir + "/libjson-parser.so";
#endif

#ifdef WIN32
    // if running on windows, change this to actual dll paths, not sure these are correct
    format_to_lib["json"] = lib_dir + "\\libjson-parser.dll";
#endif
  }

  void test_write(const std::string& format, const control_02::PhoneBook& original_obj) {
    std::stringstream ss;

    control_02::dlloader::DLLoader loader(format_to_lib[format]);

    using readtype = void(*)(control_02::PhoneBook*, std::istream&);
    using writetype = void(*)(const control_02::PhoneBook*, std::ostream&);

    auto read_phonebook = loader.get_func<readtype>("read_phonebook");
    auto write_phonebook = loader.get_func<writetype>("write_phonebook");
    write_phonebook(&original_obj, ss);
    control_02::PhoneBook restored;

    read_phonebook(&restored, ss);

    EXPECT_EQ(original_obj, restored);
  }
};


TEST(PhoneBookTest, test_empty) {
  PhoneBookTestRunner runner;
  runner.test_write("json", control_02::PhoneBook{});
}

TEST(PhoneBookTest, test_filled) {
  PhoneBookTestRunner runner;
  runner.test_write("json", control_02::PhoneBook{{{"name1", "123456"}, {"name2", "535353"}}});
}


TESTING_MAIN()
