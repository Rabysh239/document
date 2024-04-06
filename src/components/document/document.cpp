#include "document.hpp"
#include <boost/json/src.hpp>
#include <utility>
#include <simdjson/json_iterator.h>
#include "varint.hpp"

namespace components::document {

document_t::document_t()
        : allocator_intrusive_ref_counter(nullptr),
          allocator_(nullptr),
          immut_src_(nullptr),
          mut_src_(nullptr),
          element_ind_(nullptr),
          is_root_(false) {}

document_t::~document_t() {
  if (is_root_) {
    mr_delete(allocator_, mut_src_);
    mr_delete(allocator_, immut_src_);
  }
}

document_t::document_t(document_t &&other) noexcept
        : allocator_intrusive_ref_counter(other.allocator_),
          allocator_(other.allocator_),
          immut_src_(other.immut_src_),
          mut_src_(other.mut_src_),
          builder_(std::move(other.builder_)),
          element_ind_(std::move(other.element_ind_)),
          ancestors_(std::move(other.ancestors_)),
          is_root_(other.is_root_) {
  other.allocator_ = nullptr;
  other.mut_src_ = nullptr;
  other.immut_src_ = nullptr;
  other.is_root_ = false;
}

document_t &document_t::operator=(document_t &&other) noexcept {
  if (this == &other) {
    return *this;
  }
  mut_src_ = other.mut_src_;
  immut_src_ = other.immut_src_;
  builder_ = std::move(other.builder_);
  allocator_ = other.allocator_;
  element_ind_ = std::move(other.element_ind_);
  ancestors_ = std::move(other.ancestors_);
  is_root_ = other.is_root_;
  other.allocator_ = nullptr;
  other.mut_src_ = nullptr;
  other.immut_src_ = nullptr;
  other.is_root_ = false;
  return *this;
}

document_t::document_t(document_t::allocator_type *allocator, bool is_root)
        : allocator_intrusive_ref_counter(allocator),
          allocator_(allocator),
          immut_src_(nullptr),
          mut_src_(is_root ? new(allocator_->allocate(sizeof(simdjson::dom::mutable_document))) simdjson::dom::mutable_document(allocator_) : nullptr),
          builder_(allocator_, *mut_src_),
          element_ind_(is_root ? new(allocator_->allocate(sizeof(json_trie_node_element))) json_trie_node_element(allocator_) : nullptr),
          ancestors_(allocator_),
          is_root_(is_root) {}

bool document_t::is_valid() const {
  return allocator_ != nullptr;
}

std::size_t document_t::count(std::string_view json_pointer) const {
  const auto value_ptr = element_ind_->find_node_const(json_pointer);
  if (value_ptr == nullptr) {
    return 0;
  }
  return value_ptr->size();
}

bool document_t::is_exists(std::string_view json_pointer) const {
  return element_ind_->find_node_const(json_pointer) != nullptr;
}

bool document_t::is_null(std::string_view json_pointer) const {
  const auto node_ptr = element_ind_->find_node_const(json_pointer);
  if (node_ptr == nullptr) {
    return false;
  }
  auto first = node_ptr->get_value_first();
  return first != nullptr ? first->is_null() : node_ptr->get_value_second()->is_null();
}

bool document_t::is_bool(std::string_view json_pointer) const { return is_as<bool>(json_pointer); }

bool document_t::is_ulong(std::string_view json_pointer) const { return is_as<uint64_t>(json_pointer); }

bool document_t::is_long(std::string_view json_pointer) const { return is_as<int64_t>(json_pointer); }

bool document_t::is_double(std::string_view json_pointer) const { return is_as<double>(json_pointer); }

bool document_t::is_string(std::string_view json_pointer) const { return is_as<std::string_view>(json_pointer); }

bool document_t::is_array(std::string_view json_pointer) const {
  const auto node_ptr = element_ind_->find_node(json_pointer);
  return node_ptr != nullptr && is_array(*node_ptr);
}

bool document_t::is_dict(std::string_view json_pointer) const {
  const auto node_ptr = element_ind_->find_node(json_pointer);
  return node_ptr != nullptr && is_object(*node_ptr);
}

bool document_t::get_bool(std::string_view json_pointer) const { return get_as<bool>(json_pointer); }

uint64_t document_t::get_ulong(std::string_view json_pointer) const { return get_as<uint64_t>(json_pointer); }

int64_t document_t::get_long(std::string_view json_pointer) const { return get_as<int64_t>(json_pointer); }

double document_t::get_double(std::string_view json_pointer) const { return get_as<double>(json_pointer); }

std::pmr::string document_t::get_string(std::string_view json_pointer) const {
  return std::pmr::string(get_as<std::string_view>(json_pointer), allocator_);
}

document_t::ptr document_t::get_array(std::string_view json_pointer) {
  const auto node_ptr = element_ind_->find_node(json_pointer);
  if (node_ptr == nullptr || !is_array(*node_ptr)) {
    return nullptr; // temporarily
  }
  return new(allocator_->allocate(sizeof(document_t))) document_t({this}, allocator_, node_ptr);
}

document_t::ptr document_t::get_dict(std::string_view json_pointer) {
  const auto node_ptr = element_ind_->find_node(json_pointer);
  if (node_ptr == nullptr || !is_object(*node_ptr)) {
    return nullptr; // temporarily
  }
  return new(allocator_->allocate(sizeof(document_t))) document_t({this}, allocator_, node_ptr);
}

template<class T>
compare_t equals_(const document_t& doc1, const document_t& doc2, std::string_view json_pointer) {
  T v1 = doc1.get_as<T>(json_pointer);
  T v2 = doc2.get_as<T>(json_pointer);
  if (v1 < v2)
    return compare_t::less;
  if (v1 > v2)
    return compare_t::more;
  return compare_t::equals;
}

compare_t document_t::compare(const document_t& other, std::string_view json_pointer) const {
  if (is_valid() && !other.is_valid())
    return compare_t::less;
  if (!is_valid() && other.is_valid())
    return compare_t::more;
  if (!is_valid())
    return compare_t::equals;
  if (is_exists(json_pointer) && !other.is_exists(json_pointer))
    return compare_t::less;
  if (!is_exists(json_pointer) && other.is_exists(json_pointer))
    return compare_t::more;
  if (!is_exists(json_pointer) && !other.is_exists(json_pointer))
    return compare_t::equals;
  if (is_bool(json_pointer) && other.is_bool(json_pointer))
    return equals_<bool>(*this, other, json_pointer);
  if (is_ulong(json_pointer) && other.is_ulong(json_pointer))
    return equals_<uint64_t>(*this, other, json_pointer);
  if (is_long(json_pointer) && other.is_long(json_pointer))
    return equals_<int64_t>(*this, other, json_pointer);
  if (is_double(json_pointer) && other.is_double(json_pointer))
    return equals_<double>(*this, other, json_pointer);
  if (is_string(json_pointer) && other.is_string(json_pointer))
    return equals_<std::string_view>(*this, other, json_pointer);
  return compare_t::equals;
}

document_t::document_t(ptr ancestor, allocator_type *allocator, json_trie_node_element* index)
        : allocator_intrusive_ref_counter(allocator),
          allocator_(allocator),
          immut_src_(nullptr),
          mut_src_(ancestor->mut_src_),
          builder_(allocator_, *mut_src_),
          element_ind_(index),
          ancestors_(std::pmr::vector<ptr>({std::move(ancestor)}, allocator_)),
          is_root_(false) {}

error_code_t document_t::set_array(std::string_view json_pointer) {
  json_trie_node_element *container;
  std::pmr::string key(allocator_);
  auto res = find_container_key(json_pointer, container, key);
  if (res == error_code_t::SUCCESS) {
    container->insert_array(key);
  }
  return res;
}

error_code_t document_t::set_dict(std::string_view json_pointer) {
  json_trie_node_element *container;
  std::pmr::string key(allocator_);
  auto res = find_container_key(json_pointer, container, key);
  if (res == error_code_t::SUCCESS) {
    container->insert_object(key);
  }
  return res;
}

error_code_t document_t::set_deleter(std::string_view json_pointer) {
  json_trie_node_element *container;
  std::pmr::string key(allocator_);
  auto res = find_container_key(json_pointer, container, key);
  if (res == error_code_t::SUCCESS) {
    container->insert_deleter(key);
  }
  return res;
}

error_code_t document_t::remove(std::string_view json_pointer) {
  boost::intrusive_ptr<json_trie_node_element> ignored;
  return remove_(json_pointer, ignored);
}

error_code_t document_t::move(std::string_view json_pointer_from, std::string_view json_pointer_to) {
  boost::intrusive_ptr<json_trie_node_element> value;
  auto res = remove_(json_pointer_from, value);
  if (res != error_code_t::SUCCESS) {
    return res;
  }
  return set_(json_pointer_to, std::move(value));
}

error_code_t document_t::set_(std::string_view json_pointer, const element_from_mutable &value) {
  json_trie_node_element *container;
  std::pmr::string key(allocator_);
  auto res = find_container_key(json_pointer, container, key);
  if (res == error_code_t::SUCCESS) {
    container->insert(key, value);
  }
  return res;
}

error_code_t document_t::set_(std::string_view json_pointer, boost::intrusive_ptr<json_trie_node_element> &&value) {
  json_trie_node_element *container;
  std::pmr::string key(allocator_);
  auto res = find_container_key(json_pointer, container, key);
  if (res == error_code_t::SUCCESS) {
    container->insert(key, std::forward<boost::intrusive_ptr<json_trie_node_element>>(value));
  }
  return res;
}

error_code_t document_t::remove_(std::string_view json_pointer, boost::intrusive_ptr<json_trie_node_element> &node) {
  json_trie_node_element *container;
  std::pmr::string key(allocator_);
  auto res = find_container_key(json_pointer, container, key);
  if (res != error_code_t::SUCCESS) {
    return res;
  }
  if (container->is_array()) {
    return error_code_t::NOT_APPLICABLE_TO_ARRAY;
  }
  node = container->erase(key);
  if (node == nullptr) {
    return error_code_t::NO_SUCH_ELEMENT;
  }
  return error_code_t::SUCCESS;
}

error_code_t document_t::find_container_key(
        std::string_view json_pointer,
        json_trie_node_element *&container,
        std::pmr::string &key
) {
  size_t pos = json_pointer.find_last_of('/');
  if (pos == std::string::npos) {
    container = element_ind_.get();
    key = json_pointer;
  } else {
    auto container_json_pointer = json_pointer.substr(0, pos);
    container = element_ind_->find_node(container_json_pointer);
    key = json_pointer.substr(pos + 1);
  }
  if (container == nullptr || container->is_terminal()) {
    return error_code_t::NO_SUCH_CONTAINER;
  }
  if (is_array(*container)) {
    auto index = std::atol(std::pmr::string(key, allocator_).c_str());
    if (index < 0) {
      return error_code_t::INVALID_INDEX;
    }
    size_t correct_index = std::min(size_t(index), container->size());
    key = create_pmr_string(correct_index, allocator_);
  }
  return error_code_t::SUCCESS;
}

void document_t::build_index(json_trie_node_element &node, const element_from_immutable &value, std::string_view key, allocator_type *allocator) {
  if (value.is_object()) {
    node.insert_object(key);
    const auto obj = value.get_object();
    for (auto &it : obj) {
      build_index(*node.find_node(key), it.value, it.key, allocator);
    }
  } else if (value.is_array()) {
    node.insert_array(key);
    const auto arr = value.get_array();
    int i = 0;
    for (auto it: arr) {
      build_index(*node.find_node(key), it, create_pmr_string(i++, allocator), allocator);
    }
  } else {
    node.insert(key, value);
  }
}

document_t::ptr document_t::document_from_json(const std::string &json, document_t::allocator_type *allocator) {
  auto res = new(allocator->allocate(sizeof(document_t))) document_t(allocator);
  res->immut_src_ = new(allocator->allocate(sizeof(simdjson::dom::immutable_document))) simdjson::dom::immutable_document(allocator);
  if (res->immut_src_->allocate(json.size()) != simdjson::SUCCESS) {
    return nullptr;
  }
  auto tree = boost::json::parse(json);
  simdjson::tape_builder<simdjson::dom::tape_writer_to_immutable> builder(allocator, *res->immut_src_);
  walk_document(builder, tree);
  for (auto &it : res->immut_src_->root().get_object()) {
    build_index(*res->element_ind_, it.value, it.key, allocator);
  }
  return res;
}

document_t::ptr document_t::merge(document_t::ptr &document1, document_t::ptr &document2, document_t::allocator_type *allocator) {
  auto is_root = false;
  auto res = new(allocator->allocate(sizeof(document_t))) document_t(allocator, is_root);
  res->ancestors_.push_back(document1);
  res->ancestors_.push_back(document2);
  res->element_ind_ = json_trie_node_element::merge(document1->element_ind_.get(), document2->element_ind_.get(), *res->allocator_);
  return res;
}

template<typename T, typename K>
bool is_equals_value(simdjson::dom::element<T> *value1, simdjson::dom::element<K> *value2) {
  if (value1->type() != value2->type()) {
    return false;
  }
  if (value1->is_bool()) {
    return value1->get_bool().value() == value2->get_bool().value();
  }
  if (value1->is_uint64()) {
    return value1->get_uint64().value() == value2->get_uint64().value();
  }
  if (value1->is_int64()) {
    return value1->get_int64().value() == value2->get_int64().value();
  }
  if (value1->is_double()) {
    return is_equals(value1->get_double().value(), value2->get_double().value());
  }
  if (value1->is_string()) {
    return value1->get_string().value() == value2->get_string().value();
  }
  return false;
}

bool document_t::is_equals_documents(const document_ptr &doc1, const document_ptr &doc2) {
  return json_trie_node_element::equals(
          doc1->element_ind_.get(),
          doc2->element_ind_.get(),
          &is_equals_value<simdjson::dom::immutable_document, simdjson::dom::immutable_document>,
          &is_equals_value<simdjson::dom::mutable_document, simdjson::dom::mutable_document>,
          &is_equals_value<simdjson::dom::immutable_document, simdjson::dom::mutable_document>
  );
}

bool document_t::is_array(const json_trie_node_element &node) {
  return node.is_array();
}

bool document_t::is_object(const json_trie_node_element &node) {
  return node.is_object();
}

template<typename T>
std::pmr::string value_to_string(simdjson::dom::element<T> *value, std::pmr::memory_resource *allocator) {
  if (value->is_bool()) {
    std::pmr::string tmp(allocator);
    if(value->get_bool()) {
      tmp.append("true");
    } else {
      tmp.append("false");
    }
    return tmp;
  } else if (value->is_uint64()) {
    return create_pmr_string(value->get_uint64().value(), allocator);
  } else if (value->is_int64()) {
    return create_pmr_string(value->get_int64().value(), allocator);
  } else if (value->is_double()) {
    return create_pmr_string(value->get_double().value(), allocator);
  } else if (value->is_string()) {
    std::pmr::string tmp(allocator);
    tmp.append("\"").append(value->get_string().value()).append("\"");
    return tmp;
  }
  return {};
}

std::pmr::string document_t::to_json() const {
  return element_ind_->to_json(&value_to_string<simdjson::dom::immutable_document>, &value_to_string<simdjson::dom::mutable_document>);
}

std::pmr::string serialize_document(const document_ptr &document) { return document->to_json(); }

document_ptr deserialize_document(const std::string &text, document_t::allocator_type *allocator) {
  return document_t::document_from_json(text, allocator);
}

template<typename T>
std::pmr::string create_pmr_string(T value, std::pmr::memory_resource *allocator) {
  std::array<char, sizeof(T)> buffer{};
  auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
  return std::pmr::string(buffer.data(), ptr - buffer.data(), allocator);
}

document_ptr make_document(document_t::allocator_type *allocator) {
  return new(allocator->allocate(sizeof(components::document::document_t))) components::document::document_t(allocator);
}
} // namespace components::document