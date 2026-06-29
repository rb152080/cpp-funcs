#include <cstdlib>
#include <iostream>
#include <new>

class UDT
{ // user defined type
  public:
    UDT()
    {
        std::cout << "hello from constructor\n";
    }
    ~UDT()
    {
        std::cout << "hello from destructor\n";
    };
};

void* operator new(size_t size)
// behaves more like malloc; returns a void*, can't return a typed ptr
{
    void* ptr = std::malloc(size);
    if (ptr == nullptr)
        throw std::bad_alloc();
    return ptr;
}

void operator delete(void* ptr) noexcept
// it will never throw an exception
{
    std::free(ptr);
}

// strongly typed pointer
template <typename T>
// cant do T* operator new(); C++ requires that it returns void*
T* alloc()
{
    void* raw_ptr = ::operator new(sizeof(T));
    // T* typed_ptr = reinterpret_cast<T*>(raw_ptr); // not needed
    return ::new (raw_ptr) T(); // placement new
    // this is needed when you do reinterpret_cast
    // reinterpret_cast doesn't call the constructor
}

template <typename T>
void dealloc(T* ptr)
{
    if (ptr == nullptr)
        return;
    ptr->~T(); // call the destructor on its type
    ::operator delete(ptr);
}

int main()
{
    std::cout << "sizeof(UDT): " << sizeof(UDT) << "\n";
    UDT* ptr = alloc<UDT>(); // this more like standard malloc
    dealloc(ptr);
    int* int_ptr = alloc<int>();
    dealloc(int_ptr);
    return 0;
}
