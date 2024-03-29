#include <catch2/catch_test_macros.hpp>
#include "generaty.hpp"

using namespace components::document;

TEST_CASE("document::json") {
  auto allocator = std::pmr::new_delete_resource();
  auto doc = gen_doc(1, allocator);
  auto json = doc->to_json();
  auto doc2 = document_t::document_from_json(std::string(json), allocator);
  REQUIRE(doc->get_string("_id") == doc2->get_string("_id"));
  REQUIRE(doc->get_ulong("count") == doc2->get_ulong("count"));
  REQUIRE(doc->get_array("countArray")->count() == doc2->get_array("countArray")->count());
  REQUIRE(doc->get_array("countArray")->get_as<uint64_t>("1") == doc2->get_array("countArray")->get_as<uint64_t>("1"));
  REQUIRE(doc->get_dict("countDict")->count() == doc2->get_dict("countDict")->count());
  REQUIRE(doc->get_dict("countDict")->get_bool("odd") == doc2->get_dict("countDict")->get_bool("odd"));
}

TEST_CASE("document::serialize") {
  auto allocator = std::pmr::new_delete_resource();
  auto doc1 = gen_doc(1, allocator);
  auto ser1 = serialize_document(doc1);
  auto doc2 = deserialize_document(std::string(ser1), allocator);
  REQUIRE(doc1->get_string("_id") == doc2->get_string("_id"));
  REQUIRE(doc1->get_ulong("count") == doc2->get_ulong("count"));
  REQUIRE(doc1->get_array("countArray")->count() == doc2->get_array("countArray")->count());
  REQUIRE(doc1->get_array("countArray")->get_as<uint64_t>("1") == doc2->get_array("countArray")->get_as<uint64_t>("1"));
  REQUIRE(doc1->get_dict("countDict")->count() == doc2->get_dict("countDict")->count());
  REQUIRE(doc1->get_dict("countDict")->get_bool("odd") == doc2->get_dict("countDict")->get_bool("odd"));
}
