#include <array>
#include <cstddef>
#include <iostream>
#include <print>

// evaluated at compile time instead of run time
// this is the actual heap
constexpr std::size_t HEAP_SIZE{1024 * 1024}; // 1 mb/1048576 bytes

// aligning in memory so that all data types can be aligned properly; starts at
// a memory address that is a multiple of std::max_align_t
alignas(std::max_align_t) std::array<std::byte, HEAP_SIZE> heap{};

struct Block {
  bool available_{true}; // brace initialization (c++11)
  size_t size_{};        // size_t is unsigned, ssize_t is signed
  Block *next_{}; // creating a linked list; automatically sets it to nullptr
};

Block *head{nullptr}; // beginning of the heap

void initialize_head_of_heap() {
  // done at compile time; treats heap.data() as Block type instead of std::byte
  // type; static_cast also compile time, but static_cast is more strict, gives
  // you less freedom; reinterpret_cast trusts you; .data() only callable on
  // data that is contiguous
  head = reinterpret_cast<Block *>(heap.data());
  head->available_ = true; // make sure to do this when doing reinterpret_cast
  head->size_ = HEAP_SIZE - sizeof(Block);
  head->next_ = nullptr;
}

size_t align_up(size_t size, size_t alignment) {
  // return size + alignment - (size % alignment);
  // a % b (modulo) is equivalent to: a & (b - 1)
  // size + alignment - (size & (alignment - 1))
  // a - (a % b) is equivalent to: a & ~(b - 1)
  // alignment + size & ~(alignment - 1)
  // somehow goes to the next line
  return (size + alignment - 1) & ~(alignment - 1); // faster, less operations
}

void split_available_block(Block *block, size_t size) {
  // allocates space for the struct Block (its attributes) + the size (data)
  // the struct Block will store available_, size_, next_
  // we cast it to a std::byte* first since its multiple of 1, then we can do
  // the (block + 1) + size calculation properly
  Block *next_block{reinterpret_cast<Block *>(
      reinterpret_cast<std::byte *>(block + 1) + size)};
  next_block->size_ = block->size_ - sizeof(Block) - size;
  // you have to set this to true manually since reinterpret_cast just treats it
  // like its a Block but doesn't call the struct initialization
  // what would call the struct initialization is if you do: new Block() -> then
  // it would automatically set it to true as defined
  next_block->available_ = true;
  next_block->next_ = block->next_;
  // this is the current block that we are going to allocate
  block->size_ = size;
  block->next_ = next_block;
}

void *allocate(size_t size) {
  if (size == 0) { // if no memory needs to be allocated
    return nullptr;
  }
  if (!head) { // if head hasn't been initialized yet
    initialize_head_of_heap();
  }
  size = align_up(size, alignof(std::max_align_t)); // alignof (c++11)

  Block *current{head}; // current is a Block ptr

  while (current) {
    if (current->available_ && current->size_ >= size) {
      current->available_ = false;
      split_available_block(current, size);
      // current is the start
      // current + 1 skips the header
      // header: Block { available_, size_, next_ }
      // payload: where the actual data will go
      return current + 1; // returns where the payload starts
      // it does implicit pointer conversion; currently it is a Block*
      // return (void *)(current + 1); // explicit C-style pointer conversion
      // return static_cast<void *>(current + 1); // explicit C++ style pointer
      // conversion
    }
    current = current->next_;
  }
  return nullptr;
}

// [available] [available] [available] -> [available]
void merge_available_blocks() {
  Block *current{head};
  while (current && current->next_) {
    if (current->available_ && current->next_->available_) {
      current->size_ += sizeof(Block) + current->next_->size_;
      current->next_ = current->next_->next_;
    } else {
      current = current->next_;
    }
  }
}

void deallocate(void *pointer) {
  if (!pointer) {
    return;
  }
  Block *block{reinterpret_cast<Block *>(pointer) - 1};
  block->available_ = true;
  merge_available_blocks();
}

void print_heap() {
  size_t index{};
  Block *current{head};
  std::cout << "printing heap...\n";
  while (current) {
    std::println("block {}: {}, {} bytes", ++index,
                 current->available_ ? "available" : "unavailable",
                 current->size_);
    current = current->next_;
  }
  std::cout << "finished printing heap\n";
}

int main() {
  // std::cout << sizeof(Block) << "\n";
  print_heap();
  void *ptr1{allocate(10)};
  void *ptr2{allocate(20)};
  print_heap(); // block 1: unavailable, 34 bytes
                // block 2: unavailable, 44 bytes
                // block 3: available, 1 mb - 30 bytes
  deallocate(ptr1);
  deallocate(ptr2);
  print_heap();
  void *ptr3{allocate(30)};
  print_heap();
  return 0;
}
