#include <iostream>

struct test{
    void * charmsg;
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    test *t = new test();
    char* msg = "hello world";
    t->charmsg = msg;
    std::cout << "msg = " << (char*)t->charmsg << std::endl;
    return 0;
}
