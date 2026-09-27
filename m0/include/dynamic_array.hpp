#pragma once

#include <cstddef>
#include <utility>

class DynamicArray {
public:
    DynamicArray()
        : data_(nullptr), size_(0), capacity_(0) {}

    DynamicArray(const DynamicArray& other)
        : data_(nullptr), size_(other.size_), capacity_(other.capacity_) {
        if (capacity_ > 0) {
            data_ = new int[capacity_];
        }
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }

    DynamicArray& operator=(const DynamicArray& other) {
        if (this == &other) {
            return *this;
        }

        int* newdata=other.capacity_>0?new int[other.capacity_]:nullptr;
        delete[] data_;
        data_=newdata;

        for (std::size_t i = 0; i < other.size_; ++i) {
            data_[i] = other.data_[i];
        }
        size_ = other.size_;
        capacity_ = other.capacity_;
        return *this;
    }

    DynamicArray(DynamicArray&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        delete[] data_;

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;

        return *this;
    }

    ~DynamicArray() {
        delete[] data_;
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }

    void push_back(int value) {
        if (size_ == capacity_) {
            grow();
        }
        data_[size_] = value;
        ++size_;
    }

    int& operator[](std::size_t index) {
        return data_[index];
    }

    const int& operator[](std::size_t index) const {
        return data_[index];
    }

private:
    int* data_;
    std::size_t size_;
    std::size_t capacity_;

    void grow() {
        const std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
        int* new_data = new int[new_capacity];
        for (std::size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[i];
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }
};