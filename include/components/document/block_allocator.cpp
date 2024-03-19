#include "block_allocator.hpp"

block_allocator::block_allocator(size_t block_size)
        : block_size_(block_size),
          cur_block_(-1),
          offset_(0) {}

block_allocator::~block_allocator() {
  for (auto &block: blocks_) {
    delete[] block;
  }
}

void *block_allocator::allocate(size_t size) {
  if (cur_block_ == -1 || offset_ >= block_size_) {
    allocate_block();
  }
  void *ptr = blocks_[cur_block_] + offset_;
  offset_ += size;
  return ptr;
}

void block_allocator::allocate_block() {
  blocks_.emplace_back(new uint8_t[block_size_]);
  cur_block_++;
  offset_ = 0;
}