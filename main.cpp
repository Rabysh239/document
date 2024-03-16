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

  auto otter_doc = components::document::document_t::document_from_json(json);
  std::cout << otter_doc->is_double("/obj/double") << " " << otter_doc->get_double("/obj/double") << std::endl;
  long val = 3;
  otter_doc->set("/obj/long", val);
  std::cout << otter_doc->get_long("/obj/long") << std::endl;
  auto otter_doc_arr = otter_doc->get_array("/obj/arr");
  std::cout << otter_doc_arr->is_string("/0/hello") << " " << otter_doc_arr->get_string("/0/hello") << std::endl;
  return 0;
}