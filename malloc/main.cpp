#include <array>
#include <cstddef>

constexpr std::size_t HEAP_SIZE{
    1024 * 1024}; // evaluated at compile time instead of run time

alignas(std::max_align_t) std::array<
    std::byte,
    HEAP_SIZE> heap{}; // aligning in memory so that all data types
                       // can be aligned properly; starts at a memory address
                       // that is a multiple of std::max_align_t

struct Block {
  bool available{true}; // brace initialization (c++11)
  size_t size{};        // size_t is unsigned, ssize_t is signed
  Block *next{};        // creating a linked list
};

Block *head{nullptr}; // beginning of the heap

void initialize_head_of_heap() {
  head = reinterpret_cast<Block *>(
      heap.data()); // done at compile time; treats heap.data() as Block type
                    // instead of std::byte type; static_cast also compile time,
                    // but static_cast is more strict, gives you less freedom;
                    // reinterpret_cast trusts you; .data() only callable on
                    // data that is contiguous
  head->size = HEAP_SIZE - sizeof(Block);
  head->next = nullptr;
}

size_t pad(size_t size, size_t alignment) {
  // size_t remainder = size % alignment;
  // return size + (alignment - remainder);
  return (size + alignment - 1) & ~(alignment - 1); // faster, less operations
}

void *allocate_memory(size_t size) {
  if (size == 0) { // if no memory needs to be allocated
    return nullptr;
  }
  if (!head) { // if head hasn't been initialized yet
    initialize_head_of_heap();
  }
  if (!head->available) {
    return nullptr;
  }
  size = pad(size, alignof(std::max_align_t)); // (c++11)
}

void deallocate_memory(void *pointer) {}

int main() { return 0; }
