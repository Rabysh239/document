#pragma once

#include <iostream>
#include <vector>

class block_allocator {
public:
  explicit block_allocator(size_t block_size = 1024);

  ~block_allocator();

  void *allocate(size_t size);

private:
  std::vector<uint8_t *> blocks_;
  size_t block_size_;
  int cur_block_;
  size_t offset_;

  void allocate_block();
};
