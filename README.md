# Реализация контейнеров стандартной библиотеки

Для лучшего понимания работы контейнеров стандартной библиотеки была реализована своя версия некоторых из них.

---

## Пространство имен `self`


### `self::Vector`

#### Конструкторы и деструктор:
```cpp
Vector() noexcept = default;
Vector(const size_t size);
Vector(const Vector& other);
Vector(Vector&& other) noexcept;
~Vector();
```

#### Операторы:
```cpp
const T& operator[](size_t index) const noexcept;
T& operator[](size_t index) noexcept;
Vector& operator=(const Vector& rhs);
Vector& operator=(Vector&& rhs) noexcept;
```

#### Итераторы:
```cpp
iterator begin() noexcept;
iterator end() noexcept;
[[nodiscard]] const_iterator begin() const noexcept;
[[nodiscard]] const_iterator end() const noexcept;
[[nodiscard]] const_iterator cbegin() const noexcept;
[[nodiscard]] const_iterator cend() const noexcept;
```

#### Основные методы:
```cpp
[[nodiscard]] size_t Size() const noexcept;
[[nodiscard]] size_t Capacity() const noexcept;
void Reset();
void Swap(Vector& other) noexcept;
void Reserve(size_t capacity);
void Resize(size_t new_size);
```

#### Операции с элементами:
```cpp
template <typename U>
void PushBack(U&& value);
void PopBack();

template <typename... Args>
iterator Emplace(const_iterator pos, Args&&... args);

iterator Erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<T>);

template <typename... Args>
T& EmplaceBack(Args&&... args);

template <typename U>
iterator Insert(const_iterator pos, U&& value);
```

Я посчитал, что отдельные реализации одного и того же метода, принимающие lvalue и rvalue ссылки, лучше сократить до единственной, используя perfect forwarding. Поэтому некоторые методы здесь написаны без перегрузок

В файле vector/vector.h также отдельно реализован класс `RawMemory` для отделения логики работы с сырой памятью

---

### `self::Optional`

#### Конструкторы и деструктор:
```cpp
Optional() = default;
Optional(const T& value);
Optional(T&& value);
Optional(const Optional& other);
Optional(Optional&& other);
~Optional();
```

#### Операторы:
```cpp
Optional& operator=(const T& value);
Optional& operator=(T&& rhs);
Optional& operator=(const Optional& rhs);
Optional& operator=(Optional&& rhs);
explicit operator bool() const noexcept;
```

#### Основные методы:
```cpp
bool HasValue() const;
T&& operator*() &&;
T& operator*() &;
const T& operator*() const&;
T* operator->();
const T* operator->() const;
T&& Value() &&;
T& Value() &;
const T& Value() const&;
void Reset();

template <typename... Args>
void Emplace(Args&&... args);
```

---

### `self::SingleLinkedList`

#### Конструкторы и деструктор:
```cpp
SingleLinkedList();
SingleLinkedList(const std::initializer_list<Type>& items);
SingleLinkedList(const SingleLinkedList& other);
SingleLinkedList(SingleLinkedList&& other);
~SingleLinkedList();
```

#### Операторы:
```cpp
SingleLinkedList& operator=(const SingleLinkedList& rhs);
SingleLinkedList& operator=(SingleLinkedList&& rhs);
```

#### Основные методы:
```cpp
[[nodiscard]] size_t GetSize() const noexcept;
[[nodiscard]] bool IsEmpty() const noexcept;
void PushFront(const Type& value);
void Clear() noexcept;
void swap(SingleLinkedList& rhs) noexcept;

Iterator InsertAfter(ConstIterator pos, const Type& value);
void PopFront() noexcept;
Iterator EraseAfter(ConstIterator pos) noexcept;
```

#### Итераторы:
```cpp
using Iterator = BasicIterator<Type>;
using ConstIterator = BasicIterator<const Type>;

[[nodiscard]] Iterator before_begin() noexcept;
[[nodiscard]] Iterator begin() noexcept;
[[nodiscard]] Iterator end() noexcept;
[[nodiscard]] ConstIterator before_begin() const noexcept;
[[nodiscard]] ConstIterator begin() const noexcept;
[[nodiscard]] ConstIterator end() const noexcept;
[[nodiscard]] ConstIterator cbefore_begin() const noexcept;
[[nodiscard]] ConstIterator cbegin() const noexcept;
[[nodiscard]] ConstIterator cend() const noexcept;
```

#### Вспомогательные функции:
```cpp
template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept;
```

#### Операторы сравнения:
```cpp
template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs);

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs);

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs);

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs);

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs);

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs);
```

### `self::SingleLinkedList::BasicIterator`

#### Конструкторы и операторы:
```cpp
BasicIterator();
BasicIterator(const BasicIterator<Type>& other) noexcept;
BasicIterator& operator=(const BasicIterator& rhs) = default;
```

#### Операторы сравнения:
```cpp
[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept;
[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept;
[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept;
[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept;
```

#### Основные методы:
```cpp
BasicIterator& operator++() noexcept;
BasicIterator operator++(int) noexcept;
[[nodiscard]] reference operator*() const noexcept;
[[nodiscard]] pointer operator->() const noexcept;
```

---

`self::Array` был реализован как обычная обертка для статического c++ массива

#### Основные методы:
```cpp
constexpr size_t Size() const;
T& operator[](size_t index);
const T& operator[](size_t index) const;
T& At(size_t index);
const T& At(size_t index) const;
```
#### Итераторы:
```cpp
using iterator = T*;
using const_iterator = const T*;

iterator begin() noexcept;
iterator end() noexcept;
const_iterator begin() const noexcept;
const_iterator end() const noexcept;
const_iterator cbegin() const noexcept;
const_iterator cend() const noexcept;
```
