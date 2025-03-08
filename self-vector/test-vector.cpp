#include "self-vector.h"
#include <iostream>

struct C {
    C() noexcept {
        ++def_ctor;
    }
    C(const C& /*other*/) noexcept {
        ++copy_ctor;
    }
    C(C&& /*other*/) noexcept {
        ++move_ctor;
    }
    C& operator=(const C& other) noexcept {
        if (this != &other) {
            ++copy_assign;
        }
        return *this;
    }
    C& operator=(C&& /*other*/) noexcept {
        ++move_assign;
        return *this;
    }
    ~C() {
        ++dtor;
    }

    static void Reset() {
        def_ctor = 0;
        copy_ctor = 0;
        move_ctor = 0;
        copy_assign = 0;
        move_assign = 0;
        dtor = 0;
    }

    inline static size_t def_ctor = 0;
    inline static size_t copy_ctor = 0;
    inline static size_t move_ctor = 0;
    inline static size_t copy_assign = 0;
    inline static size_t move_assign = 0;
    inline static size_t dtor = 0;
};

void Dump() {
    using namespace std;
    cout << "Def ctors: "sv << C::def_ctor              //
         << ", Copy ctors: "sv << C::copy_ctor          //
         << ", Move ctors: "sv << C::move_ctor          //
         << ", Copy assignments: "sv << C::copy_assign  //
         << ", Move assignments: "sv << C::move_assign  //
         << ", Dtors: "sv << C::dtor << endl;
}

int main() {
}