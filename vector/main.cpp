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
        const size_t vector_capacity = get_capacity();
        const size_t vector_size = get_size();
        // gcc, clang does x2 <-
        // msvc (windows) does x1.5
        size_t new_capacity = (vector_capacity == 0) ? 1 : vector_capacity * 2;
        // basically realloc; copies over the elements
        T *new_vector = new T[new_capacity]; // reserves on the heap
        for (size_t i = 0; i < vector_size; i++) {
            new_vector[i] = MyVectorBegin_[i];
        }
        delete[] MyVectorBegin_;
        MyVectorBegin_ = new_vector;
        // MyVectorEnd_ = &MyVectorBegin_[vector_size];
        // MyVectorCapacity_ = &MyVectorBegin_[new_capacity];
        MyVectorEnd_ = new_vector + vector_size;
        MyVectorCapacity_ = new_vector + new_capacity;
    }

    void check_index(size_t index) const {
        if (index >= get_size()) { // don't need to check < 0, unsigned
            throw std::out_of_range(
                std::format("index {} must be in bounds\n", index));
        }
    }

  public:
    MyVector() : MyVectorBegin_{}, MyVectorEnd_{}, MyVectorCapacity_{} {}

    ~MyVector() { delete[] MyVectorBegin_; }

    MyVector(const MyVector &other) {
        const size_t other_capacity = other.get_capacity();
        const size_t other_size = other.get_size();
        if (other_capacity == 0) {
            // can't use {} initialization syntax because already initialized
            MyVectorBegin_ = nullptr;
            MyVectorEnd_ = nullptr;
            MyVectorCapacity_ = nullptr;
            return;
        }
        MyVectorBegin_ = new (std::nothrow) T[other_capacity];
        if (!MyVectorBegin_) {
            std::cerr << "error with new\n";
            return;
        }
        for (size_t i = 0; i < other_size; i++) {
            MyVectorBegin_[i] = other.MyVectorBegin_[i];
        }
        MyVectorEnd_ = MyVectorBegin_ + other_size;
        MyVectorCapacity_ = MyVectorBegin_ + other_capacity;
    }

    // we return & because without it, it would make another copy
    MyVector &operator=(const MyVector &other) { // we want to do deep copy
        if (this == &other)
            return *this;
        delete[] MyVectorBegin_;
        const size_t other_capacity = other.get_capacity();
        const size_t other_size = other.get_size();
        if (other_capacity == 0) {
            MyVectorBegin_ = nullptr;
            MyVectorEnd_ = nullptr;
            MyVectorCapacity_ = nullptr;
            return *this;
        }
        MyVectorBegin_ = new (std::nothrow) T[other_capacity];
        if (!MyVectorBegin_) {
            std::cerr << "error with new\n";
            return *this;
        }
        for (size_t i = 0; i < other_size; i++) {
            MyVectorBegin_[i] = other.MyVectorBegin_[i];
        }
        MyVectorEnd_ = MyVectorBegin_ + other_size;
        MyVectorCapacity_ = MyVectorBegin_ + other_capacity;
        return *this;
    }

    T &operator[](size_t index) {
        check_index(index);
        return *(MyVectorBegin_ + index);
    }

    const T &operator[](size_t index) const {
        check_index(index);
        return *(MyVectorBegin_ + index);
    }

    T &get_first_element() { return *(MyVectorBegin_); }

    const T &get_first_element() const { return *(MyVectorBegin_); }

    T &get_last_element() { return *(MyVectorEnd_ - 1); }

    const T &get_last_element() const { return *(MyVectorEnd_ - 1); }

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
        if (get_size() == 0) {
            std::cout << "{ }\n";
            return;
        }
        std::cout << "{ ";
        for (size_t i = 0; i < get_size(); i++) {
            if (i == get_size() - 1)
                std::cout << MyVectorBegin_[i] << " }\n";
            else
                std::cout << MyVectorBegin_[i] << ", ";
        }
    }
};

int main() {
    MyVector<int> my_vector;
    std::cout << "my_vector.get_size(): " << my_vector.get_size() << "\n";
    std::cout << "my_vector.get_capacity(): " << my_vector.get_capacity()
              << "\n";
    my_vector.push_back(1);
    my_vector.push_back(2);
    my_vector.push_back(3);
    my_vector.print_vector();
    std::cout << "my_vector.get_size(): " << my_vector.get_size() << "\n";
    std::cout << "my_vector.get_capacity(): " << my_vector.get_capacity()
              << "\n";
    std::cout << "my_vector[2]: " << my_vector[2] << "\n";
    try {
        std::cout << "my_vector[3]: " << my_vector[3] << "\n";
    } catch (const std::out_of_range &oor) {
        std::cerr << "out of range error: " << oor.what();
    }
    MyVector<int> another_vector = my_vector;
    another_vector.print_vector();
    another_vector[1] = 100;
    my_vector.print_vector();
    another_vector.print_vector();
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
