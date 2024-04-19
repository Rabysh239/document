#pragma once

#include <memory_resource>
#include <boost/smart_ptr/intrusive_ptr.hpp>

template<typename FirstType, typename SecondType>
class json_trie_node;
template<typename FirstType, typename SecondType>
class json_array;
template<typename FirstType, typename SecondType>
class json_object;

#define _usually_false(VAL) __builtin_expect(VAL, false)
