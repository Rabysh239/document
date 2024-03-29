#include <catch2/catch_test_macros.hpp>
#include "generaty.hpp"

using components::document::document_t;

TEST_CASE("document_t::is/get value") {
  auto allocator = std::pmr::new_delete_resource();
  auto doc = gen_doc(1, allocator);

  REQUIRE(doc->is_exists());
  REQUIRE(doc->is_dict());

  REQUIRE(doc->is_exists("count"));
  REQUIRE(doc->is_long("count"));
  REQUIRE(doc->get_ulong("count") == 1);

  REQUIRE(doc->is_exists("countStr"));
  REQUIRE(doc->is_string("countStr"));
  REQUIRE(doc->get_string("countStr") == "1");

  REQUIRE(doc->is_exists("countArray"));
  REQUIRE(doc->is_array("countArray"));

  REQUIRE(doc->is_exists("countDict"));
  REQUIRE(doc->is_dict("countDict"));

  REQUIRE(doc->is_exists("countArray/1"));
  REQUIRE(doc->is_long("countArray/1"));
  REQUIRE(doc->get_ulong("countArray/1") == 2);

  REQUIRE(doc->is_exists("countDict/even"));
  REQUIRE(doc->is_bool("countDict/even"));
  REQUIRE(doc->get_bool("countDict/even") == false);

  REQUIRE_FALSE(doc->is_exists("other"));
  REQUIRE_FALSE(doc->is_exists("countArray/10"));
  REQUIRE_FALSE(doc->is_exists("countDict/other"));
}

TEST_CASE("document_t::set") {
  auto allocator = std::pmr::new_delete_resource();
  auto doc = gen_doc(1, allocator);

  std::string_view key("newValue");
  std::string_view value("new value");
  doc->set(key, value);

  REQUIRE(doc->is_exists(key));
  REQUIRE(doc->is_string(key));
  REQUIRE(doc->get_string(key) == value);

  value = "super new value";
  doc->set(key, value);

  REQUIRE(doc->is_exists(key));
  REQUIRE(doc->is_string(key));
  REQUIRE(doc->get_string(key) == value);
}

//TODO
//TEST_CASE("document_view::update") {
//}

TEST_CASE("document_t::value from json") {
  auto json = R"(
{
  "_id": "000000000000000000000001",
  "count": 1,
  "countBool": true,
  "countDouble": 1.1,
  "countStr": "1",
  "countArray": [1, 2, 3, 4, 5],
  "countDict": {
    "even": false,
    "five": false,
    "odd": true,
    "three": false
  }
}
    )";
  auto allocator = std::pmr::new_delete_resource();
  auto doc = document_t::document_from_json(json, allocator);

  REQUIRE(doc->is_exists());
  REQUIRE(doc->is_exists("count"));
  REQUIRE(doc->is_long("count"));
  REQUIRE(doc->get_long("count") == 1);
}
