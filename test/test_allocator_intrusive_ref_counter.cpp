#include <catch2/catch_test_macros.hpp>
#include <memory_resource>
#include <trompeloeil.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include "allocator_intrusive_ref_counter.hpp"

class mock_memory_resource : public std::pmr::memory_resource {
protected:
  MAKE_MOCK2(do_allocate, void *(size_t, size_t), override);
  MAKE_MOCK3(do_deallocate, void(void *, size_t, size_t), override);
  MAKE_MOCK1(do_is_equal, bool(const std::pmr::memory_resource &), const noexcept);
};

class test_ref_counter : public allocator_intrusive_ref_counter<test_ref_counter> {
public:
  explicit test_ref_counter(std::pmr::memory_resource *allocator) : allocator_(allocator) {}

protected:
  std::pmr::memory_resource *get_allocator() override {
    return allocator_;
  }
private:
  std::pmr::memory_resource *allocator_;
};

TEST_CASE("release deallocate") {
  using trompeloeil::_;

  mock_memory_resource allocator;

  REQUIRE_CALL(allocator, do_deallocate(_, sizeof(test_ref_counter), alignof(std::max_align_t)));

  alignas(test_ref_counter) uint8_t buf[sizeof(test_ref_counter)];
  auto ref_counter = new(buf) test_ref_counter(&allocator);
  boost::intrusive_ptr<test_ref_counter> ptr(ref_counter);
  ptr.~intrusive_ptr();
}