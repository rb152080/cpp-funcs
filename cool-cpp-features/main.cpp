#include <cstdio>
%:include <iostream>
%:include <utility>

// digraphs: introduced in early c when not all keyboards had all characters
namespace Foo <%
struct A <%
    size_t i<%%>;
%>;

void g() <% std::cout << "g()\n"; %>

void f(A a) <%
    // return std::unreachable(); // this works for some reason
    std::cout << a.i << "\n"; // this doesn't even run if unreachable is before
    std::unreachable();       // c++23, does undefined behavior
    // return std::unreachable(); // this works for some reason
    // return printf("printf\n"); // this throws an error (because printf
    // returns an int)
    // return void; // not allowed; void is a type name
    // return g(); // this works; this actually returns void
%>

%> // namespace Foo

int main() <%
    // Foo::f(Foo::A{2});
    f(Foo::A{2}); // you don't need the Foo:: in front of f, because of
                  // argument dependent lookup/ Koenig lookup

    int arr<:2:><%1, 2%>;
    std::cout << "this works\n";
    // the << is overloaded for std::cout from the bitshift <<
    return 0;
%>
