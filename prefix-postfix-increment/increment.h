#pragma once

#include <iostream>

class Increment
{
  private:
    int value_;
    int increment_;

  public:
    Increment(int value = 0, int increment = 1)
        : value_(value), increment_(increment)
    {}

    ~Increment() {}

    Increment& operator++()
    {
        value_ += increment_;
        return *this;
    }

    Increment operator++(int)
    {
        Increment temp = *this;
        ++(*this); // calls the above function
        return temp;
    }

    void print() const
    {
        std::cout << value_ << '\n';
    }
};
