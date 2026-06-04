#include <algorithm>
#include <iostream>
#include <vector>

void func1() { std::cout << "hello from func1\n"; }

void func2(int n) { std::cout << "hello from func2\n"; }

int main() {
  std::vector<int> v{1, 3, 2, 5, 9};
  // since it returns void, it passes in void as that parameter; // func() gets
  // executed there
  // std::for_each(begin(v), end(v), func1()); // this is improper
  // std::for_each(begin(v), end(v), [](int n) { func1(); }); // proper way
  // std::for_each(begin(v), end(v), func2); // this is also proper

  int lastResult = -1;
  // [ capture ] ( params ) { body }
  // auto print_v = [lastResult](int n) {}; // value
  // auto print_v = [&lastResult](int n) { lastResult = n; }; // reference
  // auto print_v = [&](int n) { lastResult = n; }; // same thing
  auto print_v = [=](int n) mutable { lastResult = n; }; // value; mutable
  std::for_each(begin(v), end(v), print_v);
  std::cout << lastResult << "\n";
  return 0;
}
