#include <array>
#include <cstddef>

// evaluated at compile time instead of run time
constexpr std::size_t HEAP_SIZE{1024 * 1024};

// aligning in memory so that all data types can be aligned properly; starts at
// a memory address that is a multiple of std::max_align_t
alignas(std::max_align_t) std::array<std::byte, HEAP_SIZE> heap{};

struct Block {
  bool available_{true}; // brace initialization (c++11)
  size_t size_{};        // size_t is unsigned, ssize_t is signed
  Block *next_{};        // creating a linked list
};

Block *head{nullptr}; // beginning of the heap

void initialize_head_of_heap() {
  // done at compile time; treats heap.data() as Block type instead of std::byte
  // type; static_cast also compile time, but static_cast is more strict, gives
  // you less freedom; reinterpret_cast trusts you; .data() only callable on
  // data that is contiguous
  head = reinterpret_cast<Block *>(heap.data());
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
  // allocates space for the struct Block (its attributes) + the size
  // the struct Block will store available_, size_, next_
  auto *new_block{reinterpret_cast<Block *>(
      reinterpret_cast<std::byte *>(block + 1) + size)};
}

void *allocate_memory(size_t size) {
  if (size == 0) { // if no memory needs to be allocated
    return nullptr;
  }
  if (!head) { // if head hasn't been initialized yet
    initialize_head_of_heap();
  }
  size = align_up(size, alignof(std::max_align_t)); // alignof (c++11)

  auto *current{head};

  while (current) {
    if (current->available_ && current->size_ >= size) {
      current->available_ = false;
      split_available_block(current, size);
      return void *;
    }
    current = current->next_;
  }
}

void deallocate_memory(void *pointer) {}

int main() { return 0; }
