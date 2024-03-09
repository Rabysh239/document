#pragma once

#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <components/document/core/value.hpp>
#include <components/document/document_id.hpp>
#include <components/document/support/ref_counted.hpp>

namespace document::impl {
class dict_t;
class array_t;
} // namespace document::impl

namespace components::document {

enum class compare_t { less = -1, equals = 0, more = 1 };

using document_value_t = ::document::retained_t<::document::impl::value_t>;
using document_const_value_t = ::document::retained_const_t<::document::impl::value_t>;

class document_t final : public boost::intrusive_ref_counter<document_t> {
public:
  using ptr = boost::intrusive_ptr<document_t>;

  document_t();

  explicit document_t(const ::document::impl::value_t *value);

  explicit document_t(bool value);

  explicit document_t(uint64_t value);

  explicit document_t(int64_t value);

  explicit document_t(double value);

  explicit document_t(const std::string &value);

  explicit document_t(std::string_view value);

  explicit document_t(document_value_t value);

  explicit document_t(ptr document); //TODO ?
  explicit document_t(document_t &&document) noexcept; //TODO ?

  template<class T>
  void set(const std::string &key, T value);

  bool update(const ptr &update);

  using const_value_ptr = const ::document::impl::value_t *;

  document_id_t id() const;

  ptr get_ptr() const;

  bool is_valid() const;

  bool is_dict() const;

  bool is_array() const;

  std::size_t count() const;

  bool is_exists(const std::string &key) const;

  bool is_exists(std::string_view key) const;

  bool is_exists(uint32_t index) const;

  bool is_null(const std::string &key) const;

  bool is_null(uint32_t index) const;

  bool is_bool(const std::string &key) const;

  bool is_bool(uint32_t index) const;

  bool is_ulong(const std::string &key) const;

  bool is_ulong(uint32_t index) const;

  bool is_long(const std::string &key) const;

  bool is_long(uint32_t index) const;

  bool is_double(const std::string &key) const;

  bool is_double(uint32_t index) const;

  bool is_string(const std::string &key) const;

  bool is_string(uint32_t index) const;

  bool is_array(std::string_view key) const;

  bool is_array(uint32_t index) const;

  bool is_dict(std::string_view key) const;

  bool is_dict(uint32_t index) const;

  const_value_ptr get(const std::string &key) const;

  const_value_ptr get(std::string_view key) const;

  const_value_ptr get(uint32_t index) const;

  bool get_bool(const std::string &key) const;

  uint64_t get_ulong(const std::string &key) const;

  int64_t get_long(const std::string &key) const;

  double get_double(const std::string &key) const;

  std::string get_string(const std::string &key) const;

  document_t get_array(std::string_view key) const;

  document_t get_array(uint32_t index) const;

  document_t get_dict(std::string_view key) const;

  document_t get_dict(uint32_t index) const;

  const_value_ptr get_value() const;

  const_value_ptr get_value(std::string_view key) const;

  const_value_ptr get_value(uint32_t index) const;

  template<class T>
  T get_as(const std::string &key) const {
    const auto *value = get(key);
    if (value) {
      return value->as<T>();
    }
    return T();
  }

  template<class T>
  T get_as(uint32_t index) const {
    const auto *value = get(index);
    if (value) {
      return value->as<T>();
    }
    return T();
  }

  ::document::impl::dict_iterator_t begin() const;

  compare_t compare(const document_t &other, const std::string &key) const;

  std::string to_json() const;

  const ::document::impl::dict_t *as_dict() const;

  const ::document::impl::array_t *as_array() const;

  ::document::retained_t<::document::impl::dict_t> to_dict() const;

  ::document::retained_t<::document::impl::array_t> to_array() const;

  bool operator<(const document_t &rhs) const;

  bool operator>(const document_t &rhs) const;

  bool operator<=(const document_t &rhs) const;

  bool operator>=(const document_t &rhs) const;

  bool operator==(const document_t &rhs) const;

  bool operator!=(const document_t &rhs) const;

  const ::document::impl::value_t *operator*() const;

  const ::document::impl::value_t *operator->() const;

  explicit operator bool() const;

private:
  document_value_t value_;

  void set_(const std::string &key, const document_const_value_t &value);

  void set_(std::string_view key, const document_const_value_t &value);

  std::string to_json_dict() const;

  std::string to_json_array() const;

  friend class document_view_t;
};

using document_ptr = document_t::ptr;

document_ptr make_document();

document_ptr make_document(const ::document::impl::dict_t *dict);

document_ptr make_document(const ::document::impl::array_t *array);

document_ptr make_document(const ::document::impl::value_t *value);

template<class T>
document_ptr make_document(const std::string &key, T value);

document_ptr make_upsert_document(const document_ptr &source);

document_id_t get_document_id(const document_ptr &document);

document_ptr document_from_json(const std::string &json);

std::string document_to_json(const document_ptr &doc);

template<class T>
void document_t::set(const std::string &key, T value) {
  set_(key, ::document::impl::new_value(value));
}

template<>
inline void document_t::set(const std::string &key, const std::string &value) {
  set_(key, ::document::impl::new_value(value));
}

template<>
inline void document_t::set(const std::string &key, std::string_view value) {
  set_(key, ::document::impl::new_value(value));
}

template<>
inline void document_t::set(const std::string &key, document_const_value_t value) {
  set_(key, value);
}

template<>
inline void document_t::set(const std::string &key, document_ptr value) {
  set_(key, value->value_);
}

template<class T>
document_ptr make_document(const std::string &key, T value) {
  auto document = make_document();
  document->set(key, value);
  return document;
}

std::string serialize_document(const document_ptr &document);

document_ptr deserialize_document(const std::string &text);

bool is_equals_documents(const document_ptr &doc1, const document_ptr &doc2);

std::string to_string(const document_t &doc);

document_t sum(const document_t &value1, const document_t &value2);

} // namespace components::document
