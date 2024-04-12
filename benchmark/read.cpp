#include <benchmark/benchmark.h>
#include <memory_resource>
#include "../src/components/document/document.hpp"
#include "../components/generaty/generaty.hpp"

using components::document::document_t;

static void document_read(benchmark::State& state) {
  state.PauseTiming();
  auto allocator = std::pmr::unsynchronized_pool_resource();
  auto doc = gen_doc(1000, &allocator);
  std::string key_int{"/count"};
  std::string key_str{"/countStr"};
  std::string key_double{"/countDouble"};
  std::string key_bool{"/countBool"};
  std::string key_array{"/countArray"};
  std::string key_dict{"/countDict"};

  auto f = [&doc](const std::string& key) {
    doc->is_exists(key);
    doc->is_null(key);
    doc->is_bool(key);
    doc->is_ulong(key);
    doc->is_long(key);
    doc->is_double(key);
    doc->is_string(key);
    doc->is_array(key);
    doc->is_dict(key);

    doc->get_bool(key);
    doc->get_ulong(key);
    doc->get_long(key);
    doc->get_double(key);
    doc->get_string(key);
    doc->get_array(key);
    doc->get_dict(key);
  };

  state.ResumeTiming();
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      f(key_int);
      f(key_str);
      f(key_double);
      f(key_bool);
      f(key_array);
      f(key_dict);
    }
  }
}
BENCHMARK(document_read)->Arg(10000);

BENCHMARK_MAIN();