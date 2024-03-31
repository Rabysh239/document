#include "generaty.hpp"

void gen_array(int num, const document_ptr& array) {
  for (int i = 0; i < 5; ++i) {
    array->set(std::to_string(i), static_cast<int64_t>(num + i));
  }
}

void gen_dict(int num, const document_ptr& dict) {
  dict->set("odd", num % 2 != 0);
  dict->set("even", num % 2 == 0);
  dict->set("three", num % 3 == 0);
  dict->set("five", num % 5 == 0);
}

std::string gen_id(int num) {
  auto res = std::to_string(num);
  while (res.size() < 24) {
    res = "0" + res;
  }
  return res;
}

document_ptr gen_doc(int num, document_t::allocator_type *allocator) {
  document_ptr doc = make_document(allocator);
  doc->set("_id", gen_id(num));
  doc->set("count", static_cast<int64_t>(num));
  doc->set("countStr", std::to_string(num));
  doc->set("countDouble", float(num) + 0.1);
  doc->set("countBool", num % 2 != 0);
  doc->set_array("countArray");
  gen_array(num, doc->get_array("countArray"));
  doc->set_dict("countDict");
  gen_dict(num, doc->get_dict("countDict"));
  doc->set_array("nestedArray");
  auto array = doc->get_array("nestedArray");
  for (int i = 0; i < 5; ++i) {
    array->set_array(std::to_string(i));
    gen_array(num + i, array->get_array(std::to_string(i)));
  }
  doc->set_array("dictArray");
  array = doc->get_array("dictArray");
  for (int i = 0; i < 5; ++i) {
    auto index = std::to_string(i);
    array->set_dict(index);
    auto dict = array->get_dict(index);
    dict->set("number", static_cast<int64_t>(num + i));
  }
  doc->set_dict("mixedDict");
  auto dict = doc->get_dict("mixedDict");
  for (int i = 0; i < 5; ++i) {
    auto number = std::to_string(num + i);
    dict->set_dict(number);
    gen_dict(num + i, dict->get_dict(number));
  }
  return doc;
}