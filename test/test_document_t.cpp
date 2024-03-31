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

TEST_CASE("document_t::merge") {
  auto target = R"(
{
  "_id": "000000000000000000000001",
  "title": "Goodbye!",
  "author" : {
    "givenName" : "John",
    "familyName" : "Doe"
  },
  "tags":[ "example", "sample" ],
  "content": "This will be unchanged"
}
  )";

  auto patch = R"(
{
  "title": "Hello!",
  "phoneNumber": "+01-123-456-7890",
  "author": {},
  "tags": [ "example" ]
}
  )";
  auto allocator = std::pmr::new_delete_resource();
  auto target_doc = document_t::document_from_json(target, allocator);
  auto patch_doc = document_t::document_from_json(patch, allocator);

  patch_doc->set_deleter("author/familyName");

  auto res = document_t::merge(target_doc, patch_doc, allocator);

  REQUIRE(res->is_exists());
  REQUIRE(res->count() == 6);

  REQUIRE(res->is_exists("_id"));
  REQUIRE(res->is_string("_id"));
  REQUIRE(res->get_string("_id") == "000000000000000000000001");

  REQUIRE(res->is_exists("title"));
  REQUIRE(res->is_string("title"));
  REQUIRE(res->get_string("title") == "Hello!");

  REQUIRE(res->is_exists("author"));
  REQUIRE(res->is_dict("author"));
  REQUIRE(res->count("author") == 1);

  REQUIRE(res->is_exists("author/givenName"));
  REQUIRE(res->is_string("author/givenName"));
  REQUIRE(res->get_string("author/givenName") == "John");

  REQUIRE_FALSE(res->is_exists("author/familyName"));

  REQUIRE(res->is_exists("tags"));
  REQUIRE(res->is_array("tags"));
  REQUIRE(res->count("tags") == 1);

  REQUIRE(res->is_exists("tags/0"));
  REQUIRE(res->is_string("tags/0"));
  REQUIRE(res->get_string("tags/0") == "example");

  REQUIRE(res->is_exists("content"));
  REQUIRE(res->is_string("content"));
  REQUIRE(res->get_string("content") == "This will be unchanged");

  REQUIRE(res->is_exists("phoneNumber"));
  REQUIRE(res->is_string("phoneNumber"));
  REQUIRE(res->get_string("phoneNumber") == "+01-123-456-7890");
}

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
