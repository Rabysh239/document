#pragma once

#include <memory_resource>
#include <atomic>
#include <mr_utils.hpp>

class allocator_intrusive_ref_counter {
public:
  explicit allocator_intrusive_ref_counter(std::pmr::memory_resource *allocator);

  virtual ~allocator_intrusive_ref_counter() = default;

  allocator_intrusive_ref_counter(const allocator_intrusive_ref_counter &) = delete;

  allocator_intrusive_ref_counter(allocator_intrusive_ref_counter &&) noexcept = delete;

  allocator_intrusive_ref_counter& operator=(const allocator_intrusive_ref_counter &) = delete;

  allocator_intrusive_ref_counter& operator=(allocator_intrusive_ref_counter &&) noexcept = delete;

  friend void intrusive_ptr_add_ref(allocator_intrusive_ref_counter* p);

  friend void intrusive_ptr_release(allocator_intrusive_ref_counter* p);

private:
  std::atomic<std::size_t> ref_count_;
  std::pmr::memory_resource *allocator_;
};

inline allocator_intrusive_ref_counter::allocator_intrusive_ref_counter(std::pmr::memory_resource *allocator)
        : ref_count_(0),
          allocator_(allocator) {}

inline void intrusive_ptr_add_ref(allocator_intrusive_ref_counter *p) {
  p->ref_count_.fetch_add(1, std::memory_order_relaxed);
}

inline void intrusive_ptr_release(allocator_intrusive_ref_counter *p) {
  if (p->ref_count_.fetch_sub(1, std::memory_order_release) == 1) {
    std::atomic_thread_fence(std::memory_order_acquire);
    mr_delete(p->allocator_, p);
  }
}
