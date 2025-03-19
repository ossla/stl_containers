#include <memory>

template <typename T, size_t N>
class Array {
public:
    Array() = default;
public:
    constexpr size_t Size() const { return N; }
    T& operator[](size_t index) {
        assert(index < N);
        return data_[index];
    }
    const T& operator[](size_t index) const {
        return const_cast<Array&>(*this)[index];
    }
    T& At(size_t index) {
        assert(index < N);
        return data_[index];
    }
    const T& At(size_t index) const {
        return const_cast<Array&>(*this)[index];
    }

    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept {
        return data_ + 0;
    }
    iterator end() noexcept {
        return data_ + N;
    }
    const_iterator begin() const noexcept {
        return const_cast<Array&>(*this).begin();
    }
    const_iterator end() const noexcept {
        return const_cast<Array&>(*this).end();
    }
    const_iterator cbegin() const noexcept {
        return begin();
    }
    const_iterator cend() const noexcept {
        return end();
    }
    
private:
    T data_[N];
};

int main() {
    Array<int, 5> arr;
    return 0;
}