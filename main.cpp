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
  auto otter_doc = components::document::document_from_json(doc, json);
  auto obj = doc.root().get_object();
  std::cout << obj["a"].is_object() << std::endl;
  auto inner_obj = obj["a"].get_object();
  inner_obj.insert("d", inner_obj["c"]);
  inner_obj.insert("c", inner_obj["b"]);
  for (auto it = inner_obj.begin(); it != inner_obj.end(); ++it) {
    std::cout << it.key() << " " << it.value().get_double() << std::endl;
  }
  std::cout << obj.at_pointer("/a/d").get_double() << std::endl;

  std::cout << otter_doc->is_bool("/b", "c") << " " << otter_doc->get_bool("/b", "c") << std::endl;
  return 0;
}