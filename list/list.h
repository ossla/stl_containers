#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };


    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        // Конвертирующий конструктор
        explicit BasicIterator(Node* node)
        : node_(node)
        {}

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept
            : node_(other.node_)
        {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return this->node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            if (node_ != nullptr) {
                node_ = node_->next_node;
            }
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto iter_copy(*this);
            ++(*this); // assert in ++x
            return iter_copy;
        }

        [[nodiscard]] reference operator*() const noexcept {
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;

    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;


    SingleLinkedList(const std::initializer_list<Type>& items) {
        for (auto it = std::rbegin(items); it != std::rend(items); ++it) {
            this->PushFront(*it);
        }
    }

    SingleLinkedList(const SingleLinkedList& other) {
        CreateAndSwap(other);
    }

    SingleLinkedList(SingleLinkedList& other) {
        CreateAndSwap(other);
    }

    void CreateAndSwap(const SingleLinkedList<Type>& other) {
        SingleLinkedList<Type> other_copy;
        Iterator temp_head = other_copy.before_begin();
        for (const auto el : other) {
            temp_head = other_copy.InsertAfter(temp_head, el);
        }
        other_copy.size_ = other.size_;
        this->swap(other_copy);
    }


    SingleLinkedList() = default;

    ~SingleLinkedList() {
        Clear();
    }

    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    void Clear() noexcept {
        Node* temp_node = head_.next_node; // выбираем 1й элемент
        while (temp_node != nullptr) {
            head_.next_node = temp_node->next_node;
            delete temp_node;
            temp_node = head_.next_node;
        }

        size_ = 0;
    }
    
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        SingleLinkedList<Type> rhs_copy(rhs);
        this->swap(rhs_copy);
        return *this;
    }
   
    void swap(SingleLinkedList& rhs) noexcept {
        std::swap(rhs.size_, size_);
        std::swap(rhs.head_.next_node, head_.next_node);
    }

    /*
     * Вставляет элемент value после элемента, на который указывает pos.
     * Возвращает итератор на вставленный элемент
     * Если при создании элемента будет выброшено исключение, список останется в прежнем состоянии
    */
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        Node* insert_it = nullptr;
        try {
            if (pos.node_ == nullptr) {
                pos = ConstIterator(&head_);
            }
            insert_it = new Node(value, pos.node_->next_node);
            pos.node_->next_node = insert_it;
            ++size_;
            return Iterator(insert_it);
        } catch(...) {
            delete insert_it;
            throw;
        }
    }

    void PopFront() noexcept {
        Node* sec_node = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = sec_node;
        --size_;
    }

    /*
     * Удаляет элемент, следующий за pos.
     * Возвращает итератор на элемент, следующий за удалённым
    */
    Iterator EraseAfter(ConstIterator pos) noexcept {
        if (pos.node_ == nullptr) {
            pos = ConstIterator(&head_);
        }
        Node* next_node_ptr = pos.node_->next_node;
        if (next_node_ptr == nullptr) {
            return Iterator(nullptr);
        }
        Node* next_next = next_node_ptr->next_node;
        delete next_node_ptr;
        --size_;
        pos.node_->next_node = next_next;
        return Iterator(next_next);
    }

    // non-constant
    [[nodiscard]] Iterator before_begin() noexcept {
        if (size_ == 0u) {
            return Iterator(nullptr);
        }
        return Iterator(&head_);
    }

    [[nodiscard]] Iterator begin() noexcept {
        if (size_ == 0u) {
            return end();
        }
        return Iterator(head_.next_node);
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator(nullptr);
    }

    // constant
    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return cbegin();
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return cend();
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        if (size_ == 0u) {
            return ConstIterator(nullptr);
        }
        return ConstIterator(const_cast<Node*>(&head_));
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        if (size_ == 0u) {
            return cend();
        }
        return ConstIterator(head_.next_node);
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

private:
    Node head_;
    size_t size_ = 0u;

};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs > rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}
