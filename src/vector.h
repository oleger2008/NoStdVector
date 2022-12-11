#pragma once

#include "raw_memory.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>
#include <utility>

/**
 * @brief Вектор. Контейнер для элементов типа, указанного в шаблонном параметре.
 * Располагает элементы последовательно в линейном участке памяти. Вместимость
 * вектора может меняться.
 * @tparam T Тип элемента вектора.
 */
template <typename T>
class Vector {
public:
    using iterator = T *; //!< Итератор.
    using const_iterator = const T *; //!< Константный итератор.

    /**
     * @brief Получить итератор на начало вектора.
     * @details Даёт строгую гарантию безопасности.
     * @return итератор на начало вектора.
     */
    iterator begin() noexcept;

    //! @overload Vector::begin()
    const_iterator begin() const noexcept;

    //! @overload Vector::begin()
    const_iterator cbegin() const noexcept;

    /**
     * @brief Получить итератор на конец вектора.
     * @details Даёт строгую гарантию безопасности.
     * @return итератор на конец вектора.
     */
    iterator end() noexcept;

    //! @overload Vector::end()
    const_iterator end() const noexcept;

    //! @overload Vector::end()
    const_iterator cend() const noexcept;

    /**
     * @brief Конструирует пустой вектор.
     */
    Vector() = default;

    /**
     * @brief Конструирует вектор с указанным количеством элементов.
     * @param size Количество элементов.
     */
    explicit Vector(size_t size);

    /**
     * @brief Деструктор.
     * @details Освобождает
     */
    ~Vector();

    /**
     * @brief Конструирует объект, копируя переданный.
     * @param other Объект для копирования.
     */
    Vector(const Vector &other);

    /**
     * @brief Конструирует объект, перемещая себе содержимого переданного.
     * @param other Объект для перемещения.
     */
    Vector(Vector&& other) noexcept;

    /**
     * @brief Присваивает объект, копируя себе содержимое переданного.
     * @param rhs Объект для копирования.
     * @return текущий объект.
     */
    Vector& operator=(const Vector &rhs);

    /**
     * @brief Присваивает объект, перемещая себе содержимое переданного.
     * @param rhs Объект для перемещения.
     * @return текущий объект.
     */
    Vector& operator=(Vector&& rhs) noexcept;

    /**
     * @brief Меняет местами содержимое текущего объекта с содержимым переданного.
     * @param other Объект, с которым нужно поменяться содержимым.
     */
    void Swap(Vector& other) noexcept;

    /**
     * @brief Резервирует место под указанное количество элементов.
     * @param new_capacity Новая вместимость вектора.
     */
    void Reserve(size_t new_capacity);

    /**
     * @brief Меняет размер вектора на указанный.
     * @details Если до вызова этого метода в векторе был размер больше переданного, то элементы
     * стоящие на местах превышающих новый размер будут потеряны.
     * Если же до вызова этого метода в векторе был размер меньше переданного, то в вектор
     * будет дозаполнен объектами сконструированными по умолчанию.
     * @param new_size Новый размер вектора.
     */
    void Resize(size_t new_size);

    /**
     * @brief Вставить объект в конец вектора.
     * @tparam Obj Тип объекта для вставки.
     * @param value Объект для вставки.
     */
    template <typename Obj>
    void PushBack(Obj&& value);

    /**
     * @brief Удаляет последний элемент.
     */
    void PopBack() noexcept;

    /**
     * @brief Конструирует и вставляет объект в конец вектора.
     * @tparam Args Типы аргументов для конструирования объекта.
     * @param args Аргументы для конструирования объекта.
     * @return возвращает ссылку на сконструированный объект.
     */
    template <typename ...Args>
    T& EmplaceBack(Args &&...args);

    /**
     * @brief Вставляет объект в вектор перед указанным элементом.
     * @tparam Obj Тип объекта.
     * @param pos Позиция вставки.
     * @param value Объект для вставки.
     * @return итератор на вставленный элемент.
     */
    template <typename Obj>
    iterator Insert(const_iterator pos, Obj&& value);

    /**
     * @brief Конструирует и вставляет объект в вектор перед указанным элементом.
     * @tparam Args Типы аргументов для конструирования.
     * @param pos Позиция вставки.
     * @param args Аргументы для конструирования.
     * @return итератор на вставленный элемент.
     */
    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args);

    /**
     * @brief Стирает элементом в указанной позиции.
     * @param pos Позиция элемента для стирания.
     * @return итератор на элемент стоящий после стираемого.
     */
    iterator Erase(const_iterator pos);

    /**
     * @brief Получает размер.
     * @return размер.
     */
    [[nodiscard]] size_t Size() const noexcept;

    /**
     * @brief Получает вместимость.
     * @return вместимость.
     */
    [[nodiscard]] size_t Capacity() const noexcept;

    /**
     * @brief Получает доступ к элементу по индексу.
     * @param index Индекс элемента.
     * @return ссылку на элемент.
     */
    T& operator[](size_t index) noexcept;

    //! @overload Vector::operator[](size_t index)
    const T& operator[](size_t index) const noexcept;

private:
    RawMemory<T> data_; //!< Выделенная память под объекты.
    size_t size_ = 0U; //!< Размер.

    /**
     * @brief Релоцирует элементы из одного участка памяти в другой.
     * @param from Начало памяти откуда нужно релоцировать.
     * @param n Количество элементов для релокации.
     * @param to Начало памяти, в которую нужно релоцировать.
     */
    void Reallocate(T* from, size_t n, T* to);
};

template<typename T>
typename Vector<T>::iterator Vector<T>::begin() noexcept {
    return data_.GetAddress();
}

template<typename T>
typename Vector<T>::iterator Vector<T>::end() noexcept {
    return data_ + size_;
}

template<typename T>
typename Vector<T>::const_iterator Vector<T>::cbegin() const noexcept {
    return data_.GetAddress();
}

template<typename T>
typename Vector<T>::const_iterator Vector<T>::cend() const noexcept {
    return data_ + size_;
}

template<typename T>
typename Vector<T>::const_iterator Vector<T>::begin() const noexcept {
    return cbegin();
}

template<typename T>
typename Vector<T>::const_iterator Vector<T>::end() const noexcept {
    return cend();
}
template<typename T>
Vector<T>::Vector(const size_t size)
: data_(size)
, size_(size) {
    std::uninitialized_value_construct_n(data_.GetAddress(), size);
}

template<typename T>
Vector<T>::Vector(const Vector& other)
: data_(other.size_)
, size_(other.size_) {
    std::uninitialized_copy_n(other.data_.GetAddress(), other.size_, data_.GetAddress());
}

template<typename T>
Vector<T>::Vector(Vector&& other) noexcept
: data_()
, size_(0U) {
    Swap(other);
}

template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& rhs) {
    if (this == &rhs) {
        return *this;
    }

    if (rhs.size_ > data_.Capacity()) {
        Vector rhs_copy(rhs);
        Swap(rhs_copy);
    } else {
        std::copy_n(rhs.data_.GetAddress(), std::min(size_, rhs.size_), data_.GetAddress());
        if (size_ < rhs.size_) {
            std::uninitialized_copy_n(rhs.data_ + size_, rhs.size_ - size_, data_ + size_);
        } else {
            std::destroy_n(data_ + rhs.size_, size_ - rhs.size_);
        }
        size_ = rhs.size_;
    }
    return *this;
}

template<typename T>
Vector<T>& Vector<T>::operator=(Vector&& rhs) noexcept {
    if (this != &rhs) {
        Swap(rhs);
    }
    return *this;
}

template<typename T>
Vector<T>::~Vector() {
    std::destroy_n(data_.GetAddress(), size_);
}

template<typename T>
void Vector<T>::Swap(Vector& other) noexcept {
    data_.Swap(other.data_);
    std::swap(size_, other.size_);
}

template<typename T>
void Vector<T>::Reserve(const size_t new_capacity) {
    if (new_capacity <= data_.Capacity()) {
        return;
    }
    RawMemory<T> new_data(new_capacity);
    Reallocate(data_.GetAddress(), size_, new_data.GetAddress());
    std::destroy_n(data_.GetAddress(), size_);
    data_.Swap(new_data);
}

template<typename T>
void Vector<T>::Resize(const size_t new_size) {
    if (new_size <= size_) {
        std::destroy_n(data_ + new_size, size_ - new_size);
    } else {
        Reserve(new_size);
        std::uninitialized_value_construct_n(data_ + size_, new_size - size_);
    }
    size_ = new_size;
}

template<typename T>
template<typename Obj>
void Vector<T>::PushBack(Obj&& value) {
    EmplaceBack(std::forward<Obj>(value));
}

template<typename T>
void Vector<T>::PopBack() noexcept {
    std::destroy_at(data_ + size_ - 1U);
    --size_;
}
template<typename T>
template <typename... Args>
T& Vector<T>::EmplaceBack(Args&&... args) {
    if (size_ == Capacity()) {
        const size_t new_capacity = (size_ == 0U) ? 1U : (size_ * 2U);
        RawMemory<T> new_data(new_capacity);
        new (new_data + size_) T(std::forward<Args>(args)...);
        try {
            Reallocate(data_.GetAddress(), size_, new_data.GetAddress());
        } catch (...) {
            std::destroy_at(new_data + size_);
            throw;
        }
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    } else {
        new (data_ + size_) T(std::forward<Args>(args)...);
    }
    ++size_;
    return data_[size_ - 1U];
}

template<typename T>
template <typename Obj>
typename Vector<T>::iterator Vector<T>::Insert(const const_iterator pos, Obj&& value) {
    return Emplace(pos, std::forward<Obj>(value));
}

template<typename T>
template <typename... Args>
typename Vector<T>::iterator Vector<T>::Emplace(const const_iterator pos, Args&&... args) {
    const size_t dist = pos - cbegin();
    if (size_ == dist) {
        EmplaceBack(std::forward<Args>(args)...);
        return data_ + dist;
    }

    if (size_ < Capacity()) {
        T temp(std::forward<Args>(args)...);
        new (data_ + size_) T(std::move(data_[size_ - 1U]));
        std::move_backward(data_ + dist, data_ + (size_ - 1U), data_ + size_);
        data_[dist] = std::move(temp);
    } else {
        RawMemory<T> new_data(size_ * 2U);
        new (new_data + dist) T(std::forward<Args>(args)...);
        try {
            Reallocate(data_.GetAddress(), dist, new_data.GetAddress());
        } catch (...) {
            std::destroy_at(new_data + dist);
            throw;
        }
        try {
            Reallocate(data_ + dist, size_ - dist, new_data + (dist + 1U));
        } catch (...) {
            std::destroy_n(new_data.GetAddress(), dist + 1U);
            throw;
        }
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }
    ++size_;
    return data_ + dist;
}

template<typename T>
typename Vector<T>::iterator Vector<T>::Erase(const const_iterator pos) {
    const size_t dist = pos - cbegin();
    if ((dist + 1U) == size_) {
        Resize(dist);
    } else {
        std::move(data_ + (dist + 1U), data_ + size_, data_ + dist);
        std::destroy_at(data_ + (size_ - 1U));
        --size_;
    }
    return data_ + dist;
}

template<typename T>
size_t Vector<T>::Size() const noexcept {
    return size_;
}

template<typename T>
size_t Vector<T>::Capacity() const noexcept {
    return data_.Capacity();
}

template<typename T>
const T& Vector<T>::operator[](const size_t index) const noexcept {
    return const_cast<Vector&>(*this)[index];
}

template<typename T>
T& Vector<T>::operator[](const size_t index) noexcept {
    assert(index < size_);
    return data_[index];
}

template<typename T>
void Vector<T>::Reallocate(T *const from, const size_t n, T *const to) {
    if constexpr (std::is_nothrow_move_constructible_v<T> || (!std::is_copy_constructible_v<T>)) {
        std::uninitialized_move_n(from, n, to);
    } else {
        std::uninitialized_copy_n(from, n, to);
    }
}
