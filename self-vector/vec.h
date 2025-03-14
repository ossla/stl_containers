#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;
    explicit RawMemory(const size_t size) noexcept
            : data_(Allocate(size))
            , capacity_(size) {}

    explicit RawMemory(const RawMemory& other) noexcept
            : data_(Allocate(other.capacity_))
            , capacity_(other.capacity_) {
    }

    size_t GetSize() const noexcept {
        return capacity_;
    }

    T& operator[](const size_t index) noexcept {
        assert(index <= capacity_);
        return data_[index];
    }
    const T& operator[](const size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T* operator+(size_t offset) noexcept {
        assert(offset <= capacity_);
        return data_ + offset;
    }
    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return data_;
    }

    T* GetAddress() noexcept {
        return data_;
    }

    size_t Capacity() const {
        return capacity_;
    }

    ~RawMemory() {
        Deallocate(data_);
    }
private:
    T* data_ = nullptr;
    size_t capacity_ = 0;

private:
    static T* Allocate(size_t nn) {
        return nn == 0 ? nullptr : static_cast<T*>(operator new(sizeof(T) * nn));
    }

    void Deallocate(T* data) {
        operator delete(data);
        data = nullptr;
    }
};

template <typename T>
class Vector {
public:
    Vector() noexcept = default;
    Vector(const size_t size) 
            : data_(size)
            , size_(size) {
        for (size_t i = 0; i < size_; ++i) {
            new (&data_[i]) T();
        }
    }

    Vector(const Vector& other)
            : data_(other.size_)
            , size_(other.size_) {
        for (size_t i = 0; i < size_; ++i) {
            new (&data_[i]) T(other.data_[i]);
        }
    }
    ~Vector() {
        Reset();
    }
public:
    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }
    void Reset() {
        if (size_ != 0) {
            DestroyN(data_, size_);
            size_ = 0;
        }
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    void Reserve(size_t capacity) {

        if (capacity > data_.Capacity()) {
            RawMemory<T> buf(capacity);

            if (size_) {
                for (size_t i = 0; i < size_; ++i) {
                    new (&buf[i]) T(data_[i]);
                }
                DestroyN(data_, size_);
            }
            data_.Swap(buf);
        }
    }

private:
    RawMemory<T> data_;
    size_t size_ = 0;

private:
    static void DestroyN(RawMemory<T>& data, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            data[i].~T();
        }
    }
};
