#include <catch2/catch_test_macros.hpp>
#include "generaty.hpp"

using namespace components::document;
//
//TEST_CASE("document_view::json") {
//  auto allocator = std::pmr::new_delete_resource();
//  auto doc = gen_doc(1, allocator);
//  REQUIRE(
//      doc->to_json() ==
//      R"({"_id":"000000000000000000000001","count":1,"countStr":"1","countDouble":1.1,"countBool":true,"countArray":[1,2,3,4,5],"countDict":{"odd":true,"even":false,"three":false,"five":false},"nestedArray":[[1,2,3,4,5],[2,3,4,5,6],[3,4,5,6,7],[4,5,6,7,8],[5,6,7,8,9]],"dictArray":[{"number":1},{"number":2},{"number":3},{"number":4},{"number":5}],"mixedDict":{"1":{"odd":true,"even":false,"three":false,"five":false},"2":{"odd":false,"even":true,"three":false,"five":false},"3":{"odd":true,"even":false,"three":true,"five":false},"4":{"odd":false,"even":true,"three":false,"five":false},"5":{"odd":true,"even":false,"three":false,"five":true}}})");
//}

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
//
//TEST_CASE("document::serialize") {
//  auto allocator = std::pmr::new_delete_resource();
//  auto doc1 = gen_doc(1, allocator);
//  auto ser1 = serialize_document(doc1);
//  auto doc2 = deserialize_document(std::string(ser1), allocator);
//  auto ser2 = serialize_document(doc2);
//  REQUIRE(ser1 == ser2);
//}
