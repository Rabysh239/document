#include "document.hpp"
#include <utility>
#include <charconv>
#include "varint.hpp"
#include "string_splitter.hpp"
#include <boost/json/src.hpp>

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
  const auto value_ptr = find_node_const(json_pointer).first;
  if (value_ptr == nullptr) {
    return 0;
  }
  return value_ptr->size();
}

bool document_t::is_exists(std::string_view json_pointer) const {
  return find_node_const(json_pointer).first != nullptr;
}

bool document_t::is_null(std::string_view json_pointer) const {
  const auto node_ptr = find_node_const(json_pointer).first;
  if (node_ptr == nullptr || !node_ptr->is_terminal()) {
    return false;
  }
  auto first = node_ptr->get_value_first();
  return first != nullptr ? first->is_null() : node_ptr->get_value_second()->is_null();
}

bool document_t::is_bool(std::string_view json_pointer) const { return is_as<bool>(json_pointer); }

bool document_t::is_uint(std::string_view json_pointer) const { return is_as<uint32_t>(json_pointer); }

bool document_t::is_ulong(std::string_view json_pointer) const { return is_as<uint64_t>(json_pointer); }

bool document_t::is_int(std::string_view json_pointer) const { return is_as<int32_t>(json_pointer); }

bool document_t::is_long(std::string_view json_pointer) const { return is_as<int64_t>(json_pointer); }

bool document_t::is_hugeint(std::string_view json_pointer) const { return is_as<__int128_t>(json_pointer); }

bool document_t::is_float(std::string_view json_pointer) const { return is_as<float>(json_pointer); }

bool document_t::is_double(std::string_view json_pointer) const { return is_as<double>(json_pointer); }

bool document_t::is_string(std::string_view json_pointer) const { return is_as<std::string_view>(json_pointer); }

bool document_t::is_array(std::string_view json_pointer) const {
  const auto node_ptr = find_node_const(json_pointer).first;
  return node_ptr != nullptr && node_ptr->is_array();
}

bool document_t::is_dict(std::string_view json_pointer) const {
  const auto node_ptr = find_node_const(json_pointer).first;
  return node_ptr != nullptr && node_ptr->is_object();
}

bool document_t::get_bool(std::string_view json_pointer) const { return get_as<bool>(json_pointer); }

uint32_t document_t::get_uint(std::string_view json_pointer) const { return get_as<uint32_t>(json_pointer); }

uint64_t document_t::get_ulong(std::string_view json_pointer) const { return get_as<uint64_t>(json_pointer); }

int32_t document_t::get_int(std::string_view json_pointer) const { return get_as<int32_t>(json_pointer); }

int64_t document_t::get_long(std::string_view json_pointer) const { return get_as<int64_t>(json_pointer); }

__int128_t document_t::get_hugeint(std::string_view json_pointer) const { return get_as<__int128_t>(json_pointer); }

float document_t::get_float(std::string_view json_pointer) const { return get_as<float>(json_pointer); }

double document_t::get_double(std::string_view json_pointer) const { return get_as<double>(json_pointer); }

std::pmr::string document_t::get_string(std::string_view json_pointer) const {
  return std::pmr::string(get_as<std::string_view>(json_pointer), allocator_);
}

document_t::ptr document_t::get_array(std::string_view json_pointer) {
  const auto node_ptr = find_node(json_pointer).first;
  if (node_ptr == nullptr || !node_ptr->is_array()) {
    return nullptr; // temporarily
  }
  return new(allocator_->allocate(sizeof(document_t))) document_t({this}, allocator_, node_ptr);
}

document_t::ptr document_t::get_dict(std::string_view json_pointer) {
  const auto node_ptr = find_node(json_pointer).first;
  if (node_ptr == nullptr || !node_ptr->is_object()) {
    return nullptr; // temporarily
  }
  return new(allocator_->allocate(sizeof(document_t))) document_t({this}, allocator_, node_ptr);
}

template<class T, typename FirstType, typename SecondType>
compare_t equals_(
        const simdjson::dom::element<FirstType> *element1,
        const simdjson::dom::element<SecondType> *element2
) {
  T v1 = element1->template get<T>();
  T v2 = element2->template get<T>();
  if (v1 < v2)
    return compare_t::less;
  if (v1 > v2)
    return compare_t::more;
  return compare_t::equals;
}

template<typename FirstType, typename SecondType>
compare_t compare_(
        const simdjson::dom::element<FirstType> *element1,
        const simdjson::dom::element<SecondType> *element2
) {
  using simdjson::dom::element_type;

  auto type1 = element1->type();

  if (type1 == element2->type()) {
    switch (type1) {
      case element_type::INT32:
        return equals_<int32_t>(element1, element2);
      case element_type::INT64:
        return equals_<int64_t>(element1, element2);
      case element_type::INT128:
        return equals_<__int128_t>(element1, element2);
      case element_type::UINT32:
        return equals_<uint32_t>(element1, element2);
      case element_type::UINT64:
        return equals_<uint64_t>(element1, element2);
      case element_type::FLOAT:
        return equals_<float>(element1, element2);
      case element_type::DOUBLE:
        return equals_<double>(element1, element2);
      case element_type::STRING:
        return equals_<std::string_view>(element1, element2);
      case element_type::BOOL:
        return equals_<bool>(element1, element2);
      case element_type::NULL_VALUE:
        return compare_t::equals;
    }
  }

  return compare_t::equals;
}

compare_t document_t::compare(const document_t& other, std::string_view json_pointer) const {
  if (is_valid() && !other.is_valid())
    return compare_t::less;
  if (!is_valid() && other.is_valid())
    return compare_t::more;
  if (!is_valid())
    return compare_t::equals;
  auto node = find_node_const(json_pointer).first;
  auto other_node = other.find_node_const(json_pointer).first;
  auto exists = node != nullptr;
  auto other_exists = other_node != nullptr;
  if (exists && !other_exists)
    return compare_t::less;
  if (!exists && other_exists)
    return compare_t::more;
  if (!exists)
    return compare_t::equals;
  auto first = node->get_value_first();
  auto other_first = other_node->get_value_first();
  if (first != nullptr) {
    if (other_first != nullptr) {
      return compare_(first, other_first);
    }
    return compare_(first, other_node->get_value_second());
  }
  auto second = node->get_value_second();
  if (other_first != nullptr) {
    return compare_(second, other_first);
  }
  return compare_(second, other_node->get_value_second());
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
  return set_(json_pointer, special_type::ARRAY);
}

error_code_t document_t::set_dict(std::string_view json_pointer) {
  return set_(json_pointer, special_type::OBJECT);
}

error_code_t document_t::set_deleter(std::string_view json_pointer) {
  return set_(json_pointer, special_type::DELETER);
}

error_code_t document_t::set_null(std::string_view json_pointer) {
  auto next_element = mut_src_->next_element();
  builder_.visit_null_atom();
  return set_(json_pointer, next_element);;
}

error_code_t document_t::remove(std::string_view json_pointer) {
  boost::intrusive_ptr<json_trie_node_element> ignored;
  return remove_(json_pointer, ignored);
}

error_code_t document_t::move(std::string_view json_pointer_from, std::string_view json_pointer_to) {
  boost::intrusive_ptr<json_trie_node_element> node;
  auto res = remove_(json_pointer_from, node);
  if (res != error_code_t::SUCCESS) {
    return res;
  }
  return set_(json_pointer_to, std::move(node));
}

error_code_t document_t::copy(std::string_view json_pointer_from, std::string_view json_pointer_to) {
  json_trie_node_element *container;
  bool is_view_key;
  std::pmr::string key;
  std::string_view view_key;
  auto res = find_container_key(json_pointer_from, container, is_view_key, key, view_key);
  if (res != error_code_t::SUCCESS) {
    return res;
  }
  auto node = container->make_deep_copy(is_view_key ? view_key : key);
  if (node == nullptr) {
    return error_code_t::NO_SUCH_ELEMENT;
  }
  return set_(json_pointer_to, std::move(node));
}

error_code_t document_t::set_(std::string_view json_pointer, const element_from_mutable &value) {
  json_trie_node_element *container;
  bool is_view_key;
  std::pmr::string key;
  std::string_view view_key;
  auto res = find_container_key(json_pointer, container, is_view_key, key, view_key);
  if (res == error_code_t::SUCCESS) {
    container->insert(is_view_key ? view_key : key, value);
  }
  return res;
}

error_code_t document_t::set_(std::string_view json_pointer, boost::intrusive_ptr<json_trie_node_element> &&value) {
  json_trie_node_element *container;
  bool is_view_key;
  std::pmr::string key;
  std::string_view view_key;
  auto res = find_container_key(json_pointer, container, is_view_key, key, view_key);
  if (res == error_code_t::SUCCESS) {
    container->insert(is_view_key ? view_key : key, std::forward<boost::intrusive_ptr<json_trie_node_element>>(value));
  }
  return res;
}

error_code_t document_t::set_(std::string_view json_pointer, special_type value) {
  json_trie_node_element *container;
  bool is_view_key;
  std::pmr::string key;
  std::string_view view_key;
  auto res = find_container_key(json_pointer, container, is_view_key, key, view_key);
  if (res == error_code_t::SUCCESS) {
    inserters[static_cast<int>(value)](container, is_view_key ? view_key : key);
  }
  return res;
}

error_code_t document_t::remove_(std::string_view json_pointer, boost::intrusive_ptr<json_trie_node_element> &node) {
  json_trie_node_element *container;
  bool is_view_key;
  std::pmr::string key;
  std::string_view view_key;
  auto res = find_container_key(json_pointer, container, is_view_key, key, view_key);
  if (res != error_code_t::SUCCESS) {
    return res;
  }
  if (container->is_array()) {
    return error_code_t::NOT_APPLICABLE_TO_ARRAY;
  }
  node = container->erase(is_view_key ? view_key : key);
  if (node == nullptr) {
    return error_code_t::NO_SUCH_ELEMENT;
  }
  return error_code_t::SUCCESS;
}

std::pair<document_t::json_trie_node_element *, error_code_t> document_t::find_node(std::string_view json_pointer) {
  auto node_error = find_node_const(json_pointer);
  return {const_cast<json_trie_node_element *>(node_error.first), node_error.second};
}

std::pair<const document_t::json_trie_node_element *, error_code_t> document_t::find_node_const(std::string_view json_pointer) const {
  const auto *current = element_ind_.get();
  if (json_pointer.empty()) {
    return {current, error_code_t::SUCCESS};
  }
  if (json_pointer[0] != '/') {
    return {nullptr, error_code_t::INVALID_JSON_POINTER};
  }
  json_pointer.remove_prefix(1);
  for (auto key: string_splitter(json_pointer, '/')) {
    std::pmr::string unescaped_key;
    bool is_unescaped;
    auto error = unescape_key_(key, is_unescaped, unescaped_key, allocator_);
    if (error != error_code_t::SUCCESS) {
      return {nullptr, error};
    }
    current = current->find(is_unescaped ? unescaped_key : key);
    if (current == nullptr) {
      return {nullptr, error_code_t::NO_SUCH_ELEMENT};
    }
  }
  return {current, error_code_t::SUCCESS};
}

error_code_t document_t::find_container_key(
        std::string_view json_pointer,
        json_trie_node_element *&container,
        bool &is_view_key,
        std::pmr::string &key,
        std::string_view &view_key
) {
  size_t pos = json_pointer.find_last_of('/');
  if (pos == std::string::npos) {
    return error_code_t::INVALID_JSON_POINTER;
  }
  auto container_json_pointer = json_pointer.substr(0, pos);
  auto node_error = find_node(container_json_pointer);
  if (node_error.second == error_code_t::INVALID_JSON_POINTER) {
    return node_error.second;
  }
  if (node_error.second == error_code_t::NO_SUCH_ELEMENT || node_error.first->is_terminal()) {
    return error_code_t::NO_SUCH_CONTAINER;
  }
  container = node_error.first;
  view_key = json_pointer.substr(pos + 1);
  is_view_key = true;
  std::pmr::string unescaped_key;
  bool is_unescaped;
  auto error = unescape_key_(view_key, is_unescaped, unescaped_key, allocator_);
  if (error != error_code_t::SUCCESS) {
    return error;
  }
  if (is_unescaped) {
    key = std::move(unescaped_key);
    is_view_key = false;
  }
  if (container->is_array()) {
    auto index = std::atol(std::pmr::string(is_unescaped ? key : view_key, allocator_).c_str());
    if (index < 0) {
      return error_code_t::INVALID_INDEX;
    }
    size_t correct_index = std::min(size_t(index), container->size());
    is_view_key = false;
    key = create_pmr_string_(correct_index, allocator_);
  }
  return error_code_t::SUCCESS;
}

template<typename T>
void document_t::build_primitive(simdjson::tape_builder<T> &builder, const boost::json::value &value) noexcept {
  // Use the fact that most scalars are going to be either strings or numbers.
  if (value.is_string()) {
    auto &str = value.get_string();
    builder.build(str.c_str(), str.size());
  } else if (value.is_number()) {
    if (value.is_double()) {
      builder.build(value.get_double());
    } else if (value.is_int64()) {
      builder.build(value.get_int64());
    } else if (value.is_uint64()) {
      builder.build(value.get_uint64());
    }
  } else
    // true, false, null are uncommon.
  if (value.is_bool()) {
    builder.build(value.get_bool());
  } else if (value.is_null()) {
    builder.visit_null_atom();
  }
}

void document_t::build_index(
        const boost::json::value &value,
        json_trie_node_element *node,
        std::string_view current_key,
        simdjson::tape_builder<simdjson::dom::tape_writer_to_immutable> &builder,
        simdjson::dom::immutable_document *immut_src,
        allocator_type *allocator
) {
  if (value.is_object()) {
    auto next = node->insert_object(current_key);
    const auto &obj = value.get_object();
    for (auto const &[key, val] : obj) {
      build_index(val, next, key, builder, immut_src, allocator);
    }
  } else if (value.is_array()) {
    auto next = node->insert_array(current_key);
    const auto &arr = value.get_array();
    int i = 0;
    for (const auto& it: arr) {
      build_index(it, next, create_pmr_string_(i++, allocator), builder, immut_src, allocator);
    }
  } else {
    auto element = immut_src->next_element();
    build_primitive(builder, value);
    node->insert(current_key, element);
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
  for (auto &[key, val] : tree.get_object()) {
    build_index(val, res->element_ind_.get(), key, builder, res->immut_src_, allocator);
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
  using simdjson::dom::element_type;

  auto type1 = value1->type();

  if (type1 != value2->type()) {
    return false;
  }

  switch (type1) {
    case element_type::INT32:
      return value1->get_int32().value() == value2->get_int32().value();
    case element_type::INT64:
      return value1->get_int64().value() == value2->get_int64().value();
    case element_type::INT128:
      return value1->get_int128().value() == value2->get_int128().value();
    case element_type::UINT32:
      return value1->get_uint32().value() == value2->get_uint32().value();
    case element_type::UINT64:
      return value1->get_uint64().value() == value2->get_uint64().value();
    case element_type::FLOAT:
      return is_equals(value1->get_float().value(), value2->get_float().value());
    case element_type::DOUBLE:
      return is_equals(value1->get_double().value(), value2->get_double().value());
    case element_type::STRING:
      return value1->get_string().value() == value2->get_string().value();
    case element_type::BOOL:
      return value1->get_bool().value() == value2->get_bool().value();
    case element_type::NULL_VALUE:
      return true;
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

template<typename T>
std::pmr::string value_to_string(simdjson::dom::element<T> *value, std::pmr::memory_resource *allocator) {
  using simdjson::dom::element_type;

  switch (value->type()) {
    case element_type::INT32:
      return create_pmr_string_(value->get_int32().value(), allocator);
    case element_type::INT64:
      return create_pmr_string_(value->get_int64().value(), allocator);
    case element_type::INT128:
      return {"hugeint", allocator}; //TODO support value
    case element_type::UINT32:
      return create_pmr_string_(value->get_uint32().value(), allocator);
    case element_type::UINT64:
      return create_pmr_string_(value->get_uint64().value(), allocator);
    case element_type::FLOAT:
      return create_pmr_string_(value->get_float().value(), allocator);
    case element_type::DOUBLE:
      return create_pmr_string_(value->get_double().value(), allocator);
    case element_type::STRING: {
      std::pmr::string tmp(allocator);
      tmp.append("\"").append(value->get_string().value()).append("\"");
      return tmp;
    }
    case element_type::BOOL: {
      std::pmr::string tmp(allocator);
      if (value->get_bool()) {
        tmp.append("true");
      } else {
        tmp.append("false");
      }
      return tmp;
    }
    case element_type::NULL_VALUE:
      return {"null", allocator};
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
std::pmr::string create_pmr_string_(T value, std::pmr::memory_resource *allocator) {
  std::array<char, sizeof(T)> buffer{};
  auto [ptr, ec] = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
  return std::pmr::string(buffer.data(), ptr - buffer.data(), allocator);
}

document_ptr make_document(document_t::allocator_type *allocator) {
  return new(allocator->allocate(sizeof(components::document::document_t))) components::document::document_t(allocator);
}

error_code_t unescape_key_(
        std::string_view key,
        bool &is_unescaped,
        std::pmr::string &unescaped_key,
        document_t::allocator_type *allocator
) {
  size_t escape = key.find('~');
  if (escape != std::string_view::npos) {
    is_unescaped = true;
    std::pmr::string unescaped(key, allocator);
    do {
      switch (unescaped[escape + 1]) {
        case '0':
          unescaped.replace(escape, 2, "~");
          break;
        case '1':
          unescaped.replace(escape, 2, "/");
          break;
        default:
          return error_code_t::INVALID_JSON_POINTER;
      }
      escape = unescaped.find('~', escape+1);
    } while (escape != std::string::npos);
    unescaped_key = std::move(unescaped);
    return error_code_t::SUCCESS;
  }
  is_unescaped = false;
  return error_code_t::SUCCESS;
}
} // namespace components::document