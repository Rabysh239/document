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
            "key0": "hello world",
            "key1": true,
            "key2": {
                "key2_0": 123.4
            }
        }
    )";

    simdjson::dom::document doc;
    doc.allocate(json.size() * 2);
    auto el = to_document(doc, json);
    auto obj = el.get_object();
    std::cout << "size: " << obj.size() << std::endl;
    std::cout << obj.at_key("key0").is_string() << " " << obj.at_key("key0").get_string() << std::endl;
    std::cout << obj.at_key("key1").is_bool() << " " << obj.at_key("key1").get_bool() << std::endl;
    std::cout << obj.at_key("key2").is_object() << std::endl;
    auto inner_obj = obj.at_key("key2").get_object();
    std::cout << "size: " << inner_obj.size() << std::endl;
    std::cout << inner_obj.at_key("key2_0").is_double() << " " << inner_obj.at_key("key2_0").get_double() << std::endl;
    return 0;
}
