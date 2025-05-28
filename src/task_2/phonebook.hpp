#pragma once

#include <string>
#include <map>
#include <memory>

namespace control_02 {

struct PhoneBook {
  PhoneBook() = default;
  explicit PhoneBook(const std::map<std::string, std::string>& m) : name_to_number(m) {}

  std::string get_number(const std::string &name) const noexcept {
    return name_to_number.at(name);
  }

  bool has_phone(const std::string &name) const noexcept {
    return name_to_number.count(name) > 0;
  }

  void add_phone(const std::string& name, const std::string& phone) {
    name_to_number[name] = phone; // replaces phone if it was already set
  }

  bool operator==(const PhoneBook& rhs) const {
    return name_to_number == rhs.name_to_number;
  }

  const std::map<std::string, std::string>& get_name_to_number_map() const {
    return name_to_number;
  }

 private:
  std::map<std::string, std::string> name_to_number;
};


#if defined(__linux__) || defined(__APPLE__)
extern "C" {
  void read_phonebook(PhoneBook* pb, std::istream &in);
  void write_phonebook(const PhoneBook* pb, std::ostream &out);
}

#endif

#ifdef WIN32
extern "C" {
	__declspec (dllexport) void read_phonebook(PhoneBook* pb, std::istream &in);
	__declspec (dllexport) void write_phonebook(const PhoneBook* pb, std::ostream &out);
}
#endif

}
