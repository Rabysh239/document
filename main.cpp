#include <iostream>
#include "include/components/document/document.hpp"

int main() {
  const std::string json = R"(
        {
          "obj": {
            "double": 2.3,
            "arr": [
              {
                "hello": "world"
              }
            ]
          }
        }
    )";

  simdjson::dom::document doc;
  auto otter_doc = components::document::document_t::document_from_json(json);
  std::cout << otter_doc->is_double("/obj/double") << " " << otter_doc->is_double("/obj/double") << std::endl;
  std::cout << otter_doc->is_string("/obj/arr/0/hello") << " " << otter_doc->get_string("/obj/arr/0/hello") << std::endl;
  return 0;
}