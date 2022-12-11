#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <new>
#include <utility>

/**
 * @brief Простой аллокатор памяти.
 * @tparam T Тип объекта, который будет размещаться в памяти.
 */
template <typename T>
class RawMemory {
public:
    /**
     * @brief Конструирует по умолчанию объект без выделенной памяти.
     */
    RawMemory() = default;

    /**
     * @brief Конструирует объект, который выделяет память с указанной вместимостью.
     * @param capacity Вместимость памяти.
     */
    explicit RawMemory(size_t capacity);

    //! Запрет на копирование.
    RawMemory(const RawMemory &) = delete;
    //! Запрет на копирование.
    RawMemory &operator=(const RawMemory &rhs) = delete;

    /**
     * @brief Конструирует объект, перемещая содержимое переданного в конструктор объекта.
     * @param other Объект, содержимое, которого нужно переместить.
     */
    RawMemory(RawMemory &&other) noexcept;

    /**
     * @brief Присваивает значение объекта справа, перемещая его содержимое себе.
     * @param rhs Присваиваемый объект.
     * @return Возвращает текущий объект.
     */
    RawMemory &operator=(RawMemory &&rhs) noexcept;

    /**
     * @brief Освобождает выделенную память.
     */
    ~RawMemory();

    /**
     * @brief Получает указатель на элемент, отстоящий в памяти от первого на переданное число.
     * @param offset Отступ.
     * @return указатель на элемент
     */
    T *operator+(size_t offset) noexcept;

    //! @overload RawMemory::operator+(size_t offset)
    const T *operator+(size_t offset) const noexcept;

    /**
     * @brief Получает доступ к объект по индексу.
     * @param index Индекс объекта.
     * @return ссылку на объект.
     */
    T &operator[](size_t index) noexcept;

    //! @overload RawMemory::operator[](size_t index)
    const T &operator[](size_t index) const noexcept;

    /**
     * @brief Поменять местами текущее содержимое с содержимом переданного объекта.
     * @param other Объект, с которым нужно поменять внутренним содержимым.
     */
    void Swap(RawMemory &other) noexcept;

    /**
     * @brief Получает адрес начала выделенной памяти под объекты.
     * @return адрес начала выделенной памяти под объекты.
     */
    T *GetAddress() noexcept;

    //! @overload RawMemory::GetAddress()
    const T *GetAddress() const noexcept;

    /**
     * @brief Получить вместимость хранилища объектов.
     * @return вместимость.
     */
    [[nodiscard]] size_t Capacity() const;

private:
    T *buffer_ = nullptr; //!< Выделенная память.
    size_t capacity_ = 0U; //!< Вместимость хранилища, т.е. сколько поместится объектов.

    /**
     * @brief Выделяет сырую память под указанное количество элементов.
     * @param n Количество элементов.
     * @return указатель на начало выделенной памяти.
     */
    static T *Allocate(size_t n);

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    /**
     * @brief Освобождает переданную память.
     * @warning Предполагает, что будет передан указатель на память, которая была выделена
     * при помощи Allocate.
     * @see Allocate(size_t n)
     * @param buf память, которую нужно освободить.
     */
    static void Deallocate(T *buf) noexcept;
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