#pragma once
#include <algorithm>
#include <type_traits>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <utility>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;
    explicit RawMemory(const size_t size) noexcept
            : data_(Allocate(size))
            , capacity_(size) {}

    /* копирующий конструктор и оператор присваивания не нужны RawMemory, иначе 
     экземпляры будут конкурировать за одну область памяти */
    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory& rhs) = delete;

    RawMemory(RawMemory&& other) noexcept { 
        *this = std::move(other);
    }
    RawMemory& operator=(RawMemory&& rhs) noexcept {
        std::swap(data_, rhs.data_);
        std::swap(capacity_, rhs.capacity_);
        return *this;
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
        if (data_) {
            Deallocate(data_);
        }
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
        std::uninitialized_value_construct_n(data_.GetAddress(), size_);
    }
    Vector(const Vector& other)
            : data_(other.size_)
            , size_(other.size_) {
        std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());
    }
    Vector(Vector&& other) : data_(std::move(other.data_)), size_(other.size_) {}

    ~Vector() {
        Reset();
    }

    using iterator = T*;
    using const_iterator = const T*;
    
    iterator begin() noexcept {
        return data_ + 0;
    }
    iterator end() noexcept {
        return data_ + size_;
    }
    const_iterator begin() const noexcept {
        return const_cast<Vector&>(*this).begin();
    }
    const_iterator end() const noexcept {
        return const_cast<Vector&>(*this).end();
    }
    const_iterator cbegin() const noexcept {
        return begin();
    }
    const_iterator cend() const noexcept {
        return end();
    }
public:
    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }
    void Reset() {
        if (data_.GetAddress()) {
            std::destroy_n(data_.GetAddress(), size_);
        }
        size_ = 0;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    Vector& operator=(Vector&& rhs) noexcept {
        data_ = std::move(rhs.data_);
        std::swap(size_, rhs.size_);
        return *this;
    }

    void Swap(Vector& other) noexcept {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
    }

    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            if (rhs.size_ > data_.Capacity()) {
                /* copy-and-swap идиома */
                Vector rhs_copy(rhs);
                Swap(rhs_copy);
            } else {
                size_t last_initialized_idx = 0;
                /* размер вектора-источника меньше размера вектора-приёмника */
                if (rhs.size_ < size_) {
                    std::destroy_n(data_ + rhs.size_, size_ - rhs.size_);
                    last_initialized_idx = rhs.size_;
                } 
                /* размер вектора-источника больше или равен размеру вектора-приёмника */
                else {
                    std::uninitialized_copy_n(rhs.data_ + size_
                                             , rhs.size_ - size_
                                             , data_ + size_);
                    last_initialized_idx = size_;
                }
                std::copy(rhs.data_.GetAddress(), rhs.data_ + last_initialized_idx
                        , data_.GetAddress());
            }
            size_ = rhs.size_;
        }
        return *this;
    }

    void Reserve(size_t capacity) {

        if (capacity > data_.Capacity()) {
            RawMemory<T> buffer(capacity);
            MoveOrCopyUninitialized(data_.GetAddress(), size_, buffer.GetAddress());

            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(buffer);
        }
    }

    void Resize(size_t new_size) {
        if (!new_size) {
            Reset();
            return;
        } else if (new_size < size_) {
            std::destroy_n(data_.GetAddress(), size_ - new_size);
        } else {
            Reserve(new_size);
            std::uninitialized_value_construct_n(
                    data_ + size_, new_size - size_);
        }
        size_ = new_size;
    }

    /* 
        так же как и в OverflowPush, emplace_back, ...
        это универсальная ссылка, которая позволяет 
        работать с const T& и T&& при помощи схлопывания
    */
    template <typename U>
    void PushBack(U&& value) {
        EmplaceBack(std::forward<U>(value)); 
    }

    void PopBack() {
        if (size_) {
            data_[size_-1].~T();
            --size_;
        }
    }

    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args) {
        size_t dist = pos - begin();

        if (Capacity() == size_) {
            OverflowPush(dist, std::forward<Args>(args)...);
        } else {
            iterator position = begin() + dist;
            if (dist == size_) {
                new (position) T(std::forward<Args>(args)...);
                ++size_;
            } else {
                T* temp = new T(std::forward<Args>(args)...);
                MoveOrCopyBackward(position, end(), end()+1);
                *position = std::move(*temp);
                ++size_;
                delete temp;
            }
        }
        return data_ + dist;
    }
    iterator Erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<T>) {
        size_t dist = pos - data_.GetAddress();
        std::move(data_ + dist + 1, data_ + size_, data_ + dist);
        data_[size_-1].~T();
        --size_;
        return data_ + dist;
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (Capacity() == size_) {
            OverflowPush(size_, std::forward<Args>(args)...);
        } else {
            new (data_ + size_) T(std::forward<Args>(args)...);
            ++size_;
        }
        return data_[size_-1];
    }

    template <typename U>
    iterator Insert(const_iterator pos, U&& value) {
        return Emplace(pos, std::forward<U>(value));
    }

private:
    RawMemory<T> data_;
    size_t size_ = 0;

private:
    void MoveOrCopyUninitialized(iterator src, size_t n, iterator dest) {
        if constexpr (std::is_nothrow_move_constructible_v<T> 
                    || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(src, n, dest);
        } else {
            std::uninitialized_copy_n(src, n, dest);
        }
    }

    void MoveOrCopyBackward(iterator first, iterator last, iterator d_last) {
        if constexpr (std::is_nothrow_move_constructible_v<T> 
                || !std::is_copy_constructible_v<T>) {
            std::move_backward(first, last, d_last);
        } else {
            std::copy_backward(first, last, d_last);
        }
    }

    
    /* Почему не T&&?
    Type&& — это универсальная ссылка (perfect forwarding).
    - Когда передается lvalue, Type выводится 
    как T& или const T&, и получается T& &&, которое схлопывается в T&
    
    - Когда передается rvalue, Type выводится 
    как T&&, и получается T&& &&, которое схлопывается в T&&  */
    template <typename... U>
    void OverflowPush(size_t pos_n, U&&... val) {
        RawMemory<T> buffer(size_ == 0 ? 1 : size_ * 2);
        new (buffer + pos_n) T(std::forward<U>(val)...);
        
        /* до pos */
        MoveOrCopyUninitialized(data_.GetAddress(), pos_n
                                , buffer.GetAddress());
        /* после */
        MoveOrCopyUninitialized(data_ + pos_n, size_-pos_n
                                , buffer + pos_n + 1);
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(buffer);
        ++size_;
    }
};
