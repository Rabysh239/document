#pragma once

#include <radix_tree.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <optional>
#include "../../simdjson/dom/element.h"
#include "../../simdjson/dom/object-inl.h"
#include "../../simdjson/dom/array-inl.h"

class element_index : public boost::intrusive_ref_counter<element_index> {
public:
  explicit element_index(const simdjson::dom::element &value);

  std::optional<simdjson::dom::element> get(std::string_view key) const;

  void update_or_insert(std::string_view key, const simdjson::dom::element &value);

private:
  using element_tree = radix_tree<std::string, simdjson::dom::element>;
  element_tree data_;

  void build_index(const simdjson::dom::element &value, const std::string& json_pointer);
};

class prefix_index {
public:
  using index_ptr = boost::intrusive_ptr<element_index>;

  prefix_index(std::string_view prefix, index_ptr &ind_ptr);

  std::optional<simdjson::dom::element> get(std::string_view key) const;

  void update_or_insert(std::string_view key, const simdjson::dom::element &value);
private:
  std::string prefix_;
  index_ptr &ind_ptr_;
};
