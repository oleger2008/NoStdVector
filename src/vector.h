#pragma once

#include "raw_memory.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>
#include <utility>

template <typename T>
class Vector {
public:
    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept {
        return data_.GetAddress();
    }
    iterator end() noexcept {
        return data_ + size_;
    }
    const_iterator begin() const noexcept {
        return cbegin();
    }
    const_iterator end() const noexcept {
        return cend();
    }
    const_iterator cbegin() const noexcept {
        return data_.GetAddress();
    }
    const_iterator cend() const noexcept {
        return data_ + size_;
    }

    Vector() = default;

    explicit Vector(size_t size)
        : data_(size)
        , size_(size)  //
    {
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }

    Vector(const Vector& other)
        : data_(other.size_)
        , size_(other.size_)
    {
        std::uninitialized_copy_n(other.data_.GetAddress(), other.size_, data_.GetAddress());
    }
    Vector(Vector&& other) noexcept
        : data_()
        , size_(0)
    {
        this->Swap(other);
    }

    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            if (rhs.size_ > data_.Capacity()) {
                Vector rhs_copy(rhs);
                this->Swap(rhs_copy);
            } else {
                std::copy_n(rhs.data_.GetAddress(), std::min(size_, rhs.size_), data_.GetAddress());
                if (size_ < rhs.size_) {
                    std::uninitialized_copy_n(rhs.data_ + size_, rhs.size_ - size_, data_ + size_);
                } else {
                    std::destroy_n(data_ + rhs.size_, size_ - rhs.size_);
                }
                size_ = rhs.size_;
            }
        }
        return *this;
    }
    Vector& operator=(Vector&& rhs) noexcept {
        if (this != &rhs) {
            Swap(rhs);
        }
        return *this;
    }

    void Swap(Vector& other) noexcept {
        this->data_.Swap(other.data_);
        std::swap(this->size_, other.size_);
    }

    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= data_.Capacity()) {
            return;
        }
        RawMemory<T> new_data(new_capacity);
        Reallocate(data_.GetAddress(), size_, new_data.GetAddress());
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            std::destroy_n(data_ + new_size, size_ - new_size);
        } else {
            Reserve(new_size);
            std::uninitialized_value_construct_n(data_ + size_, new_size - size_);
        }
        size_ = new_size;
    }

    template <typename Obj>
    void PushBack(Obj&& value) {
        EmplaceBack(std::forward<Obj>(value));
    }

    void PopBack() /* noexcept */ {
        std::destroy_at(data_ + size_ - 1u);
        --size_;
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (size_ == Capacity()) {
            size_t new_capacity = size_ == 0 ? 1u : size_ * 2;
            RawMemory<T> new_data(new_capacity);
            new (new_data + size_) T(std::forward<Args>(args)...);
            try {
                Reallocate(data_.GetAddress(), size_, new_data.GetAddress());
            } catch(...) {
                std::destroy_at(new_data + size_);
                throw;
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
        } else {
            new (data_ + size_) T(std::forward<Args>(args)...);
        }
        ++size_;
        return data_[size_ - 1u];
    }

    template <typename Obj>
    iterator Insert(const_iterator pos, Obj&& value) {
        return Emplace(pos, std::forward<Obj>(value));
    }

    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args) {
        size_t dist = pos - cbegin();
        if (size_ == dist) {
            EmplaceBack(std::forward<Args>(args)...);
            return data_ + dist;
        }
        if (size_ < Capacity()) {
            T temp(std::forward<Args>(args)...);
            new (data_ + size_) T(std::move(data_[size_ - 1u]));
            std::move_backward(data_ + dist, data_ + (size_ - 1u), data_ + size_);
            data_[dist] = std::move(temp);
        } else {
            RawMemory<T> new_data(size_ * 2);
            new (new_data + dist) T(std::forward<Args>(args)...);
            try {
                Reallocate(data_.GetAddress(), dist, new_data.GetAddress());
            } catch (...) {
                std::destroy_at(new_data + dist);
                throw;
            }
            try {
                Reallocate(data_ + dist, size_ - dist, new_data + (dist + 1u));
            } catch (...) {
                std::destroy_n(new_data.GetAddress(), dist + 1u);
                throw;
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
        }
        ++size_;
        return data_ + dist;
    }

    iterator Erase(const_iterator pos) {
        size_t dist = pos - cbegin();
        if (dist + 1u == size_) {
            Resize(dist);
        } else {
            std::move(data_ + (dist + 1u), data_ + size_, data_ + dist);
            std::destroy_at(data_ + (size_ - 1u));
            --size_;
        }
        return data_ + dist;
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

private:
    RawMemory<T> data_;
    size_t size_ = 0;

private:
    void Reallocate(T* from, size_t n, T* to) {
        // constexpr оператор if будет вычислен во время компиляции
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(from, n, to);
        } else {
            std::uninitialized_copy_n(from, n, to);
        }
    }
};
