#include "nlohmann/json.hpp"
#include "phonebook.hpp"

namespace control_02 {
void read_phonebook(PhoneBook *pb, std::istream &in) {
  nlohmann::json j;
  in >> j;
  for (auto it : j.items()) {
    pb->add_phone(it.key(), it.value());
  }
}
void write_phonebook(const PhoneBook *pb, std::ostream &out) {
  nlohmann::json j;
  for (auto it : pb->get_name_to_number_map()) {
    j.emplace(it.first, it.second);
  }
  out << j;
}
} // namespace control_02