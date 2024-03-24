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
  const std::string json1 = R"(
        {
          "obj": {
            "long": 5,
            "bool": true
          }
        }
    )";
  const std::string json2 = R"(
        {
          "obj": {
            "long": 5
          }
        }
    )";

  std::pmr::synchronized_pool_resource allocator;
  auto otter_doc = components::document::document_t::document_from_json(json, &allocator);
  std::cout << otter_doc->is_double("/obj/double") << " " << otter_doc->get_double("/obj/double") << std::endl;
  long val = 3;
  otter_doc->set("/obj/long", val);
  std::cout << otter_doc->get_long("/obj/long") << std::endl;
  auto otter_doc_arr = otter_doc->get_array("/obj/arr");
  std::cout << otter_doc_arr->is_string("/0/hello") << " " << otter_doc_arr->get_string("/0/hello") << std::endl;
  //merge
  auto otter_doc1 = components::document::document_t::document_from_json(json1, &allocator);
  auto merged_doc = components::document::document_t::merge(otter_doc, otter_doc1, &allocator);
  std::cout << merged_doc->get_long("/obj/long") << std::endl;
  std::cout << merged_doc->is_bool("/obj/bool") << std::endl;
  std::cout << merged_doc->get_string("/obj/arr/0/hello") << std::endl;
  //split
  auto otter_doc2 = components::document::document_t::document_from_json(json2, &allocator);
  auto split_doc = components::document::document_t::split(otter_doc1, otter_doc2, &allocator);
  std::cout << split_doc->is_exists("/obj/long") << std::endl;
  std::cout << split_doc->is_bool("/obj/bool") << std::endl;
  return 0;
}