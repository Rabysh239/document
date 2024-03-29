#pragma once

#include "json_trie_node.hpp"
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <utility>
#include <memory_resource>
//#include <components/document/document_id.hpp>
#include <simdjson/dom/document-inl.h>
#include <simdjson/dom/element-inl.h>
#include <simdjson/dom/array-inl.h>
#include <simdjson/dom/object-inl.h>
#include <simdjson/tape_builder.h>
#include <allocator_intrusive_ref_counter.hpp>

namespace components::document {

enum class compare_t { less = -1, equals = 0, more = 1 };

enum class error_t {
  SUCCESS,
  NO_SUCH_CONTAINER,
  INVALID_INDEX,
};

class document_t final : public allocator_intrusive_ref_counter {
public:
  using ptr = boost::intrusive_ptr<document_t>;
  using allocator_type = std::pmr::memory_resource;

  document_t();

  ~document_t() override;

  document_t(document_t &&) noexcept;

  document_t(const document_t &) = delete;

  document_t &operator=(document_t &&) noexcept;

  document_t &operator=(const document_t &) = delete;

  explicit document_t(allocator_type *, bool = true);
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
  error_t set(std::string_view json_pointer, T value);

  error_t set_array(std::string_view json_pointer);

  error_t set_dict(std::string_view json_pointer);
//
//  bool update(const ptr &update);

//  document_id_t id() const;

  bool is_valid() const;

  std::size_t count(std::string_view json_pointer = "") const;

  bool is_exists(std::string_view json_pointer) const;

  bool is_null(std::string_view json_pointer) const;

  bool is_bool(std::string_view json_pointer) const;

  bool is_ulong(std::string_view json_pointer) const;

  bool is_long(std::string_view json_pointer) const;

  bool is_double(std::string_view json_pointer) const;

  bool is_string(std::string_view json_pointer) const;

  bool is_array(std::string_view json_pointer = "") const;

  bool is_dict(std::string_view json_pointer = "") const;

  bool get_bool(std::string_view json_pointer) const;

  uint64_t get_ulong(std::string_view json_pointer) const;

  int64_t get_long(std::string_view json_pointer) const;

  double get_double(std::string_view json_pointer) const;

  std::pmr::string get_string(std::string_view json_pointer) const;

  ptr get_array(std::string_view json_pointer);

  ptr get_dict(std::string_view json_pointer);

  template<class T>
  bool is_as(std::string_view json_pointer) const {
    const auto node_ptr = element_ind_->find_node_const(json_pointer);
    if (node_ptr == nullptr || !node_ptr->is_terminal()) {
      return false;
    }
    auto first = node_ptr->get_value_first();
    return first != nullptr ? first->is<T>() : node_ptr->get_value_second()->is<T>();
  }

  template<class T>
  T get_as(std::string_view json_pointer) const {
    const auto node_ptr = element_ind_->find_node_const(json_pointer);
    if (node_ptr == nullptr || !node_ptr->is_terminal()) {
      return T();
    }
    auto first = node_ptr->get_value_first();
    if (first != nullptr) {
      return first->is<T>() ? first->get<T>().value() : T();
    }
    auto second = node_ptr->get_value_second();
    return second->is<T>() ? second->get<T>().value() : T();
  }
//  ::document::impl::dict_iterator_t begin() const;

  compare_t compare(const document_t &other, std::string_view json_pointer) const;

  std::pmr::string to_json() const;

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

  static ptr document_from_json(const std::string &json, document_t::allocator_type *allocator);

  static ptr merge(ptr &document1, ptr &document2, document_t::allocator_type *allocator);

  static ptr split(ptr &document1, ptr &document2, document_t::allocator_type *allocator);

private:
  using element_from_immutable = simdjson::dom::element<simdjson::dom::immutable_document>;
  using element_from_mutable = simdjson::dom::element<simdjson::dom::mutable_document>;
  using json_trie_node_element = json_trie_node<element_from_immutable, element_from_mutable>;

  document_t(ptr ancestor, allocator_type *allocator, json_trie_node_element* index);

  allocator_type *allocator_;
  simdjson::dom::immutable_document *immut_src_;
  simdjson::dom::mutable_document *mut_src_;
  simdjson::tape_builder<simdjson::dom::tape_writer_to_mutable> builder_{};
  boost::intrusive_ptr<json_trie_node_element> element_ind_;
  std::pmr::vector<ptr> ancestors_{};
  bool is_root_;

  error_t process_json_pointer_set(
          std::string_view json_pointer,
          const std::function<void(json_trie_node_element *, std::string_view)> &value_inserter
  );

  error_t set_(std::string_view json_pointer, const simdjson::dom::element<simdjson::dom::mutable_document> &value);

  static void build_index(json_trie_node_element& node, const element_from_immutable &value, std::string_view key, allocator_type *allocator);

  static bool is_array(const json_trie_node_element &node);
  static bool is_object(const json_trie_node_element &node);
};

using document_ptr = document_t::ptr;

document_ptr make_document(document_t::allocator_type *allocator);
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

template<class T>
inline error_t document_t::set(std::string_view json_pointer, T value) {
  auto next_element = mut_src_->next_element();
  builder_.build(value);
  return set_(json_pointer, next_element);
}

template<>
inline error_t document_t::set(std::string_view json_pointer, const std::string &value) {
  return set(json_pointer, std::string_view(value));
}

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

std::pmr::string serialize_document(const document_ptr &document);

document_ptr deserialize_document(const std::string &text, document_t::allocator_type *allocator);
//
//bool is_equals_documents(const document_ptr &doc1, const document_ptr &doc2);
//
//std::string to_string(const document_t &doc);
//
//document_t sum(const document_t &value1, const document_t &value2);

template<typename T>
std::pmr::string create_pmr_string(T value, std::pmr::memory_resource *allocator);

} // namespace components::document
