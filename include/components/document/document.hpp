#pragma once

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
//#include <components/document/document_id.hpp>
#include "../../simdjson/dom/element-inl.h"
#include "../../simdjson/dom/object-inl.h"
#include "../../../src/generic/stage2/tape_builder.h"
#include <tsl/htrie_map.h>

namespace components::document {
//
//enum class compare_t { less = -1, equals = 0, more = 1 };

class document_t final : public boost::intrusive_ref_counter<document_t> {
public:
  using ptr = boost::intrusive_ptr<document_t>;
//
//  document_t();

  explicit document_t(simdjson::dom::document &&value);
//
//  explicit document_t(bool value);
//
//  explicit document_t(uint64_t value);
//
//  explicit document_t(int64_t value);
//
//  explicit document_t(double value);
//
//  explicit document_t(const std::string &value);
//
//  explicit document_t(std::string_view value);

  template<class T>
  void set(std::string_view json_pointer, T value);
//
//  bool update(const ptr &update);

//  document_id_t id() const;
//
//  bool is_valid() const;
//
//  bool is_dict() const;
//
//  bool is_array() const;
//
//  std::size_t count() const; // no effective on current changeable object

  bool is_exists(std::string_view json_pointer) const;
//
//  bool is_null(const std::string &key) const;
//
//  bool is_null(uint32_t index) const;

  bool is_bool(std::string_view json_pointer) const;

  bool is_ulong(std::string_view json_pointer) const;

  bool is_long(std::string_view json_pointer) const;

  bool is_double(std::string_view json_pointer) const;

  bool is_string(std::string_view json_pointer) const;
//
//  bool is_array(std::string_view key) const;
//
//  bool is_array(uint32_t index) const;
//
//  bool is_dict(std::string_view key) const;
//
//  bool is_dict(uint32_t index) const;

  simdjson::simdjson_result<simdjson::dom::element> get(std::string_view json_pointer) const;

  bool get_bool(std::string_view json_pointer) const;

  uint64_t get_ulong(std::string_view json_pointer) const;

  int64_t get_long(std::string_view json_pointer) const;

  double get_double(std::string_view json_pointer) const;

  std::string get_string(std::string_view json_pointer) const;

//  document_t get_array(std::string_view key) const;
//
//  document_t get_array(uint32_t index) const;
//
//  document_t get_dict(std::string_view key) const;
//
//  document_t get_dict(uint32_t index) const;
//
//  const_value_ptr get_value() const;
//
//  const_value_ptr get_value(std::string_view key) const;
//
//  const_value_ptr get_value(uint32_t index) const;

  template<class T>
  T get_as(std::string_view json_pointer) const {
    const auto value = get(json_pointer);
    if (value.error() == simdjson::SUCCESS) {
      return value.get<T>();
    }
    return T();
  }
//  ::document::impl::dict_iterator_t begin() const;
//
//  compare_t compare(const document_t &other, const std::string &key) const;
//
//  std::string to_json() const;

//  ::document::retained_t<::document::impl::dict_t> to_dict() const;

//  ::document::retained_t<::document::impl::array_t> to_array() const;
//
//  bool operator<(const document_t &rhs) const;
//
//  bool operator>(const document_t &rhs) const;
//
//  bool operator<=(const document_t &rhs) const;
//
//  bool operator>=(const document_t &rhs) const;
//
//  bool operator==(const document_t &rhs) const;
//
//  bool operator!=(const document_t &rhs) const;
//
//  const ::document::impl::value_t *operator*() const;
//
//  const ::document::impl::value_t *operator->() const;
//
//  explicit operator bool() const;

private:
  simdjson::dom::document source_;
  simdjson::SIMDJSON_IMPLEMENTATION::stage2::tape_builder builder_;
  tsl::htrie_map<char, simdjson::dom::element> json_pointer_index_;

  simdjson::error_code set_(std::string_view json_pointer, const simdjson::dom::element &value);
//
//  std::string to_json_dict() const;
//
//  std::string to_json_array() const;

  void build_index(const simdjson::dom::element &value, const std::string& json_pointer);
};

using document_ptr = document_t::ptr;
//
//document_ptr make_document();
//
//document_ptr make_document(const ::document::impl::dict_t *dict);
//
//document_ptr make_document(const ::document::impl::array_t *array);
//
//document_ptr make_document(const ::document::impl::value_t *value);
//
//template<class T>
//document_ptr make_document(const std::string &key, T value);
//
//document_ptr make_upsert_document(const document_ptr &source);

//document_id_t get_document_id(const document_ptr &document);

document_ptr document_from_json(const std::string &json);
//
//std::string document_to_json(const document_ptr &doc);

template<class T>
inline void document_t::set(std::string_view json_pointer, T value) {
  auto next_element_index = builder_.next_tape_index();
  builder_.build((T) value);
  set_(json_pointer, simdjson::dom::element(simdjson::internal::tape_ref(&source_, next_element_index)));
}

template<>
inline void document_t::set(std::string_view json_pointer, const std::string &value) {
  set(json_pointer, std::string_view(value));
}

//template<>
//inline void document_t::set(const std::string &key, std::string_view value) {
//  set_(key, ::document::impl::new_value(value));
//}
//
//template<>
//inline void document_t::set(const std::string &key, document_const_value_t value) {
//  set_(key, value);
//}
//
//template<>
//inline void document_t::set(const std::string &key, document_ptr value) {
//  set_(key, value->value_);
//}
//
//template<class T>
//document_ptr make_document(const std::string &key, T value) {
//  auto document = make_document();
//  document->set(key, value);
//  return document;
//}
//
//std::string serialize_document(const document_ptr &document);
//
//document_ptr deserialize_document(const std::string &text);
//
//bool is_equals_documents(const document_ptr &doc1, const document_ptr &doc2);
//
//std::string to_string(const document_t &doc);
//
//document_t sum(const document_t &value1, const document_t &value2);

} // namespace components::document
