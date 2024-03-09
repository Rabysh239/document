#include <iostream>
#include <boost/json/src.hpp>
#include "include/simdjson/dom/document-inl.h"
#include "src/generic/stage2/tape_builder.h"

simdjson::simdjson_result<simdjson::dom::element> to_document(simdjson::dom::document &doc, const std::string &json) {
  auto tree = boost::json::parse(json);
  simdjson::SIMDJSON_IMPLEMENTATION::stage2::tape_builder::parse_document(doc, tree);
  return doc.root();
}

int main() {
  const std::string json = R"(
        {
            "a": {
                "b": 1,
                "c": 2.3
            }
        }
    )";

  simdjson::dom::document doc;
  doc.allocate(json.size());
  auto el = to_document(doc, json);
  auto obj = el.get_object();
  std::cout << obj["a"].is_object() << std::endl;
  auto inner_obj = obj["a"].get_object();
  inner_obj.insert("d", inner_obj["c"]);
  inner_obj.insert("c", inner_obj["b"]);
  for (auto it = inner_obj.begin(); it != inner_obj.end(); ++it) {
    std::cout << it.key() << " " << it.value().get_double() << std::endl;
  }
  std::cout << obj.at_pointer("/a/d").get_double() << std::endl;
  return 0;
}