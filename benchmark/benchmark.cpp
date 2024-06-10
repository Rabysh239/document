#include <benchmark/benchmark.h>
#include <memory_resource>
#include "../src/components/document/document.hpp"
#include "../components/generaty/generaty.hpp"

using components::document::document_t;

void read(benchmark::State &state) {
  auto allocator = std::pmr::unsynchronized_pool_resource();
  auto doc = gen_doc(1000, &allocator);
  std::string_view str_key{"/_id"};
  std::string_view array_int_key{"/countArray/3"};
  std::string_view array_array_key{"/nestedArray/2"};
  std::string_view dict_dict_key{"/mixedDict/1001"};
  std::string_view array_dict_key{"/dictArray/3"};
  std::string_view array_array_int_key{"/nestedArray/2/2"};
  std::string_view array_dict_int_key{"/dictArray/3/number"};
  std::string_view dict_dict_bool_key{"/mixedDict/1001/odd"};

  auto f = [
          &doc,
          str_key,
          array_int_key,
          array_array_key,
          dict_dict_key,
          array_dict_key,
          array_array_int_key,
          array_dict_int_key,
          dict_dict_bool_key
  ]() {
    doc->get_string(str_key);
    doc->get_int(array_int_key);
    doc->get_array(array_array_key);
    doc->get_dict(dict_dict_key);
    doc->get_dict(array_dict_key);
    doc->get_long(array_array_int_key);
    doc->get_long(array_dict_int_key);
    doc->get_bool(dict_dict_bool_key);
  };

  for (auto _: state) {
    for (int i = 0; i < state.range(0); ++i) {
      f();
    }
  }
}
BENCHMARK(read)->Arg(100);

void write(benchmark::State& state) {
  auto allocator = std::pmr::new_delete_resource();
  auto doc = make_document(allocator);

  std::string_view key_simple{"/count"};
  std::string_view key_array{"/countArray/1"};
  std::string_view key_dict{"/countDict/odd"};

  bool value_bool{true};
  int64_t value_int{100};
  double value_double{100.001};
  std::string_view value_str{"100.001"};

  auto f = [&](std::string_view key) {
    doc->set_null(key);
    doc->set(key, value_bool);
    doc->set(key, value_int);
    doc->set(key, value_double);
    doc->set(key, value_str);
  };

  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      f(key_simple);
      f(key_array);
      f(key_dict);
    }
  }
  doc->count();
}
BENCHMARK(write)->Arg(100);

BENCHMARK_MAIN();