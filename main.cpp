#include <iostream>
#include "include/components/document/document.hpp"

int main() {
  const std::string json = R"(
        {
            "a": {
                "b": 1,
                "c": 2.3
            },
            "b": {
                "c": true
            }
        }
    )";

  simdjson::dom::document doc;
  auto otter_doc = components::document::document_t::document_from_json(json);
  std::cout << otter_doc->is_bool("/b/c") << " " << otter_doc->get_bool("/b/c") << std::endl;
  otter_doc->set("/a/d", 3.4);
  std::cout << otter_doc->is_double("/a/d") << " " << otter_doc->get_double("/a/d") << std::endl;
  return 0;
}