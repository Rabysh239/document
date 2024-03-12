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
  auto otter_doc = components::document::document_from_json(json);
  std::cout << otter_doc->is_bool("/b/c") << " " << otter_doc->get_bool("/b/c") << std::endl;
  otter_doc->set("/a/d", 3.4);
  std::cout << otter_doc->is_double("/a/d") << " " << otter_doc->get_double("/a/d") << std::endl;
  auto a_obj = otter_doc->get("/a").get_object();
  for (auto it = a_obj.begin(); it != a_obj.end(); ++it) {
    std::cout << it.key() << " " << it.value().get_double() << std::endl;
  }
  std::string_view val = "world!";
  otter_doc->set("/b/hello", val);
  std::cout << otter_doc->is_string("/b/hello") << " " << otter_doc->get_string("/b/hello") << std::endl;
  return 0;
}