#include <cstdlib>
#include <iostream>
#include <new>

class UDT {
public:
  UDT() { std::cout << "hello from constructor\n"; }
  ~UDT() { std::cout << "hello from destructor\n"; };
};

void *my_new_void(size_t size) {
  std::cout << "allocating memory (" << size << " byte(s))\n";
  void *ptr = std::malloc(size); // malloc returns void pointer
  // TODO: find a way to return a strongly typed pointer
  if (!ptr) {
    throw std::bad_alloc();
  }
  return ptr;
}

my_new_template(size_t size) {}

void my_delete(void *ptr) {
  std::cout << "freeing memory\n";
  std::free(ptr);
}

int main() {
  std::cout << "sizeof(UDT): " << sizeof(UDT) << "\n";
  UDT *ptr1 = (UDT *)my_new(sizeof(UDT)); // this more like standard malloc
  my_delete(ptr1);

  UDT *ptr2 = new UDT();
  delete ptr2;
  return 0;
}
