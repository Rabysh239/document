#include <iostream>
#include "src/components/document/document.hpp"

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

//  std::pmr::synchronized_pool_resource allocator;
//  auto otter_doc = components::document::document_t::document_from_json(json, &allocator);
//  std::cout << otter_doc->is_double("/obj/double") << " " << otter_doc->get_double("/obj/double") << std::endl;
//  long val = 3;
//  otter_doc->set("/obj/long", val);
//  std::cout << otter_doc->get_long("/obj/long") << std::endl;
//  auto otter_doc_arr = otter_doc->get_array("/obj/arr");
//  std::cout << otter_doc_arr->is_string("/0/hello") << " " << otter_doc_arr->get_string("/0/hello") << std::endl;
//  std::cout << otter_doc->to_json() << std::endl;
//  //merge
//  auto otter_doc1 = components::document::document_t::document_from_json(json1, &allocator);
//  auto merged_doc = components::document::document_t::merge(otter_doc, otter_doc1, &allocator);
//  std::cout << merged_doc->to_json() << std::endl;
  std::cout << float(std::numeric_limits<uint32_t>::max()) << std::endl;
  std::cout << float(std::numeric_limits<uint64_t>::max()) << std::endl;
  return 0;
}