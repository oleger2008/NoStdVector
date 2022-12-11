#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>
#include <utility>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
    : buffer_(Allocate(capacity))
    , capacity_(capacity) {
    }

    RawMemory(const RawMemory&) = delete;
    RawMemory& operator=(const RawMemory& rhs) = delete;

    RawMemory(RawMemory&& other) noexcept
    : buffer_(nullptr)
    , capacity_(0) {
        std::swap(this->buffer_, other.buffer_);
        std::swap(this->capacity_, other.capacity_);
    }
    RawMemory& operator=(RawMemory&& rhs) noexcept {
        Deallocate(buffer_);
        this->capacity_ = 0;
        std::swap(this->buffer_, rhs.buffer_);
        std::swap(this->capacity_, rhs.capacity_);
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template<typename T>
RawMemory<T>::RawMemory(const size_t capacity)
: buffer_(Allocate(capacity))
, capacity_(capacity) {
}

template<typename T>
RawMemory<T>::RawMemory(RawMemory &&other) noexcept
: buffer_(nullptr)
, capacity_(0U) {
    std::swap(buffer_, other.buffer_);
    std::swap(capacity_, other.capacity_);
}

template<typename T>
RawMemory<T> &RawMemory<T>::operator=(RawMemory &&rhs) noexcept {
    Deallocate(buffer_);
    std::swap(buffer_, rhs.buffer_);
    capacity_ = std::exchange(rhs.capacity_, 0U);
}

template<typename T>
RawMemory<T>::~RawMemory() {
    Deallocate(buffer_);
}

template<typename T>
T *RawMemory<T>::operator+(const size_t offset) noexcept {
    assert(offset <= capacity_);
    return buffer_ + offset;
}

template<typename T>
const T *RawMemory<T>::operator+(const size_t offset) const noexcept {
    return const_cast<RawMemory &>(*this) + offset;
}

template<typename T>
const T &RawMemory<T>::operator[](const size_t index) const noexcept {
    return const_cast<RawMemory&>(*this)[index];
}

template<typename T>
T &RawMemory<T>::operator[](const size_t index) noexcept {
    assert(index < capacity_);
    return buffer_[index];
}

template<typename T>
void RawMemory<T>::Swap(RawMemory &other) noexcept {
    std::swap(buffer_, other.buffer_);
    std::swap(capacity_, other.capacity_);
}

template<typename T>
const T *RawMemory<T>::GetAddress() const noexcept {
    return buffer_;
}

template<typename T>
T *RawMemory<T>::GetAddress() noexcept {
    return buffer_;
}

template<typename T>
size_t RawMemory<T>::Capacity() const {
    return capacity_;
}

template<typename T>
T *RawMemory<T>::Allocate(const size_t n) {
    return (n != 0U) ? static_cast<T *>(operator new(n * sizeof(T))) : nullptr;
}

template<typename T>
void RawMemory<T>::Deallocate(T *buf) noexcept {
    operator delete(buf);
}