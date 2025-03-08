#include <stdexcept>

template <typename T>
class ArrPtr {
public:
    ArrPtr() : pointer_(nullptr) {}
    ArrPtr(const int size) : size_(size){
        pointer_ = new T[size_];
    }
    ~ArrPtr() {
        delete[] pointer_;
    }

public:
    T& operator[](const int index) {
        if (index > size_-1) {
            return std::bad_alloc("index invalid");
        }
        return pointer_[index];
    }
    const T& operator[](const int index) const {
        return *this[index];
    }
    T& At(const int index) {
        return *this[index];
    }
    const T& At(const int index) const {
        return *this[index];
    }
    T& front() {
        if (size_ == 0) {
            return std::bad_alloc("there is no elems");
        }
        return *this[0];
    }
    const T& front() const {
        return this->front(); 
    }
    T& back() {
        if (size_ == 0) {
            return std::bad_alloc("there is no elems");
        }
        return *this[size_-1];
    }
    const T& back() const {
        return this->back();
    }
private:
    size_t size_ = 0;
    T* pointer_ = nullptr;
};

template <typename T>
class SelfVector {
public:
private:
    ArrPtr<T> raw_array_;
};