#include <cassert>
#include <cstddef>
#include <format>
#include <iostream>
#include <stdexcept>
#include <vector>

template <typename T> class MyVector {
  private:
    // members are 24 bytes total, sizeof(std::vector) = 24
    T *MyVectorBegin_;
    T *MyVectorEnd_;
    T *MyVectorCapacity_;
    void grow() { // you have to use new and delete
        size_t new_capacity{};
        if (get_capacity() == 0) {
            new_capacity = 1;
        } else {
            // gcc, clang does x2 <-
            // msvc (windows) does x1.5
            new_capacity = get_capacity() * 2;
        }
        T *new_vector = new T[new_capacity]; // reserves on the heap
        for (size_t i = 0; i < get_size(); i++) {
            new_vector[i] = MyVectorBegin_[i];
        }
        size_t vector_size = get_size();
        delete[] MyVectorBegin_;
        MyVectorBegin_ = new_vector;
        // MyVectorEnd_ = &MyVectorBegin_[vector_size];
        // MyVectorCapacity_ = &MyVectorBegin_[new_capacity];
        MyVectorEnd_ = new_vector + vector_size;
        MyVectorCapacity_ = new_vector + new_capacity;
    }

  public:
    MyVector() : MyVectorBegin_{}, MyVectorEnd_{}, MyVectorCapacity_{} {}
    ~MyVector() { delete[] MyVectorBegin_; }
    T &get_element_at(size_t index) const {
        if (index >= get_size()) {
            throw std::logic_error(
                std::format("index {} must be in bounds\n", index));
        }
        return *(MyVectorBegin_ + index);
    }
    size_t get_size() const { return MyVectorEnd_ - MyVectorBegin_; }
    size_t get_capacity() const { return MyVectorCapacity_ - MyVectorBegin_; }
    void push_back(T element) {
        if (MyVectorEnd_ == MyVectorCapacity_) {
            grow();
        }
        assert(MyVectorEnd_ != MyVectorCapacity_);
        // element is stored on the stack, is out of scope after function exits
        // MyVectorEnd_ = &element; // wrong because pass by value as parameter
        *MyVectorEnd_ = element;
        MyVectorEnd_++;
    }
    void print_vector() const {
        std::cout << "{ ";
        for (size_t i = 0; i < get_size(); i++) {
            std::cout << get_element_at(i) << ", ";
        }
        std::cout << "}\n";
    }
};

int main() {
    MyVector<int> my_vector;
    my_vector.push_back(1);
    my_vector.push_back(2);
    my_vector.push_back(3);
    my_vector.print_vector();
    std::cout << "my_vector.get_element_at(2): " << my_vector.get_element_at(2)
              << "\n";
    std::cout << "my_vector.get_size(): " << my_vector.get_size() << "\n";
    // std::vector<int> real_vector;
    // real_vector.push_back(1);
    // real_vector.push_back(1);
    // real_vector.push_back(1);
    // real_vector.push_back(1);
    // real_vector.push_back(1);
    // real_vector.push_back(1);
    // real_vector.push_back(1);
    // real_vector.push_back(1);
    // std::vector<int>::iterator begin_ptr = real_vector.begin();
    // std::vector<int>::iterator end_ptr = real_vector.end();
    // auto capacity_ptr = real_vector.capacity();
    // std::cout << "begin_ptr: " << &(*begin_ptr) << "\n";
    // std::cout << "end_ptr: " << &(*end_ptr) << "\n";
    // std::cout << "end_ptr - begin_ptr: " << end_ptr - begin_ptr << "\n";

    // these are the same
    // std::cout << "sizeof(std::vector): " << sizeof(std::vector<char>) <<
    // "\n"; std::cout << "sizeof(real_vector): " << sizeof(real_vector) <<
    // "\n";
    return 0;
}
