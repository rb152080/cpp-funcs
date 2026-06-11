#include <array>
#include <cstddef>
#include <iostream>
#include <print>

// evaluated at compile time instead of run time
// this is the actual heap
constexpr std::size_t HEAP_SIZE{1024 * 1024}; // 1 mb/1048576 bytes

// aligns in memory so that all data types can be aligned properly
// starts at a memory address that is a multiple of std::max_align_t
// alignas -> aligns in memory
// std::max_align_t -> largest alignment type (16)
// this array represents the heap; type raw bytes
alignas(std::max_align_t) std::array<std::byte, HEAP_SIZE> heap{};

// struct Block // 24 bytes
struct alignas(std::max_align_t) Block // 32 bytes
{
    bool available{true}; // brace initialization (c++11)
    size_t size{};        // size_t is unsigned, ssize_t is signed
    // creating a linked list; automatically sets it to nullptr
    Block *next{};
};

Block *head{nullptr}; // beginning of the heap

void initialize_head()
{
    // treats heap.data() as Block type instead of std::byte type
    // just changes compiler interpretation of the data/ bytes
    // no constructor is called
    // static_cast also compile time, but static_cast is more strict, gives
    // you less freedom; reinterpret_cast trusts you .data() only callable
    // on data that is contiguous
    head = reinterpret_cast<Block *>(heap.data()); // done at compile time
    // make sure to do this when doing reinterpret_cast
    head->available = true;
    head->size = HEAP_SIZE - sizeof(Block);
    head->next = nullptr;
}

// pass in default alignment since it will typically be that
size_t align_up(size_t size, size_t alignment = alignof(std::max_align_t))
// alignof (c++11)
{
    // return size + alignment - (size % alignment);
    // a % b (modulo) is equivalent to: a & (b - 1)
    // size + alignment - (size & (alignment - 1))
    // a - (a % b) is equivalent to: a & ~(b - 1)
    // alignment + size & ~(alignment - 1)
    // somehow goes to the next line
    return (size + alignment - 1) & ~(alignment - 1);
    // faster, less operations
}

// size is always a multiple of 16 since we only call this func in allocate
void split_available_block(Block *block, size_t size)
{
    // allocates space for the struct Block (its members) + size (data)
    // the struct Block header will store available_, size_, next_
    // payload will store the data in the size (parameter) we allocate
    // block + 1 points to the 33rd byte, the +1 works like +sizeof(Block)
    // the general formula is: type + (int * sizeof(type))
    // if it was just reinterpret_cast<std::byte*>(block), it would just
    // change the type from struct Block to std::byte
    // we cast it to a std::byte* since its multiple of 1, then we can add
    // size to it and it will be multiplied by 1
    // we have to reinterpret_cast it finally back to a Block* so the
    // compiler can treat the next_block variable as type Block
    Block *next_block{reinterpret_cast<Block *>(
        reinterpret_cast<std::byte *>(block + 1) + size)};
    next_block->size = block->size - sizeof(Block) - size;
    // you have to set this to true manually since reinterpret_cast just
    // treats it like its a Block but doesn't call the struct initialization
    // what would call the struct initialization is if you do: new Block()
    // -> then it would automatically set it to true as defined
    next_block->available = true;
    next_block->next = block->next;
    // this is the current block that we are going to allocate
    block->size = size;
    block->next = next_block;
}

// [available] [available] [available] -> [available]
void merge_available_blocks()
{
    Block *current{head};
    while (current && current->next)
    {
        if (current->available && current->next->available)
        {
            current->size += sizeof(Block) + current->next->size;
            current->next = current->next->next;
        }
        else
        {
            current = current->next;
        }
    }
}

void *allocate(size_t size)
{
    if (size == 0) // if no memory needs to be allocated
        return nullptr;

    if (!head) // if head hasn't been initialized yet
        initialize_head();

    size = align_up(size); // makes sure size is multiple of 16

    Block *current{head}; // current is a Block ptr

    while (current) // iterates through the blocks starting from head
    {
        if (current->available && current->size >= size)
        {
            current->available = false;
            split_available_block(current, size);
            // current is the start
            // current + 1 skips the header
            // header: Block { available_, size_, next_ }
            // payload: where the actual data will go
            // it does implicit pointer conversion; currently it is a Block*
            return current + 1; // returns where the payload starts

            // explicit C-style pointer conversion
            // return (void *)(current + 1);

            // explicit C++ style pointer conversion
            // return static_cast<void *>(current + 1);
        }
        current = current->next;
    }
    return nullptr;
}

void deallocate(void *pointer)
{
    if (!pointer)
        return;
    Block *block{reinterpret_cast<Block *>(pointer) - 1};
    block->available = true;
    merge_available_blocks();
}

void print_heap()
{
    size_t index{};
    Block *current{head};
    std::cout << "printing heap...\n";
    while (current)
    {
        std::println("block {}: {}, {} bytes", ++index,
                     current->available ? "available" : "unavailable",
                     current->size);
        current = current->next;
    }
    std::cout << "...finished printing heap\n";
}

int main()
{
    // std::cout << "sizeof(Block): " << sizeof(Block) << "\n"; // 24
    // std::cout << "std::max_align_t: " << alignof(std::max_align_t) << "\n";
    // std::cout << "sizeof(long double): " << sizeof(long double) << "\n";
    print_heap();
    void *ptr1{allocate(8)};
    void *ptr2{allocate(20)};
    print_heap(); // block 1: unavailable, 34 bytes
                  // block 2: unavailable, 44 bytes
                  // block 3: available, 1 mb - 30 bytes
    std::cout << 1048576 - 32 - 32 * 2 - 16 - 32 << "\n";
    deallocate(ptr1);
    deallocate(ptr2);
    print_heap();
    std::cout << 1048576 - 32 << "\n";
    void *ptr3{allocate(30)};
    print_heap();
    std::cout << 1048576 - 32 - 32 - 32 << "\n";
    deallocate(ptr3);
    return 0;
}
