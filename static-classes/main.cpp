#include <iostream>

struct API {
    API() {}
    ~API() {}

    // declaration
    // static int MAJOR;
    const static int MAJOR; // this works
    // const static int MAJOR = 7; // this also works
    // static int MAJOR =
    //     7; // can't do this, if non const, must be declared outside

    // static int GetMajor() {
    //     return MAJOR;
    // } // this works; static functions can only use static variables
};

// definition
// int MAJOR = 7;
const int API::MAJOR = 7;

int main() {
    // API instance;
    // instance.MAJOR = 7; // can't do this
    std::cout << API::MAJOR << "\n"; // static variables in a class exist on
                                     // their own, they don't need an instance
                                     // the scope is within the class
    // std::cout << API::GetMajor() << "\n";
    return 0;
}
