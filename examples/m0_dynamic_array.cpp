#include <cstddef>
#include <iostream>
#include<utility>

class DynamicArray {
public:
    DynamicArray()
        : data_(nullptr), size_(0), capacity_(0) {}

    DynamicArray(const DynamicArray& other):data_(nullptr),size_(other.size_),capacity_(other.capacity_){
        if(capacity_>0){
            data_=new int[capacity_];
        }
        for(std::size_t i=0;i<other.size_;i++){
            data_[i]=other.data_[i];
        }
    }

    DynamicArray& operator=(const DynamicArray& other){
        if(this==&other){
            return *this;
        }
        if(this->data_!=nullptr){
            delete[] this->data_;
        }
        int* newdata=nullptr;
        if(other.capacity_>0){
            newdata=new int[other.capacity_];
        }
        for(std::size_t i=0;i<other.size_;i++){
            newdata[i]=other.data_[i];
        }
        this->data_=newdata;
        this->size_=other.size_;
        this->capacity_=other.capacity_;
        return *this;
    }

    ~DynamicArray() {
        delete[] data_;
    }

    DynamicArray(DynamicArray&& other) noexcept:data_(other.data_),size_(other.size_),capacity_(other.capacity_){
        other.data_=nullptr;
        other.capacity_=0;
        other.size_=0;
    }

    DynamicArray& operator=(DynamicArray&& other) noexcept{
        if(this==&other){
            return *this;
        }
        delete[] data_;
        data_=other.data_;
        size_=other.size_;
        capacity_=other.capacity_;
        other.data_=nullptr;
        other.capacity_=0;
        other.size_=0;
        return *this;
    }

    std::size_t size() const {
        return size_;
    }

    std::size_t capacity() const {
        return capacity_;
    }

    void push_back(int value) {
        // TODO 1: 如果 size_ == capacity_，调用 grow();
        if(size_==capacity_){
            grow();
        }
        // TODO 2: 把 value 放到 data_[size_]；
        data_[size_]=value;
        // TODO 3: 把 size_ 加 1。
        size_++;
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
        // TODO 4: 计算 new_capacity：
        //         capacity_ == 0 时为 1，否则为 capacity_ * 2。
        std::size_t new_capacity;
        if(capacity_==0){
            new_capacity=1;
        }else{
            new_capacity=capacity_*2;
        }
        // TODO 5: 用 new int[new_capacity] 申请新内存。
        int* newdata=new int[new_capacity];
        // TODO 6: 把旧数据复制到新内存。
        for(std::size_t i=0;i<size_;i++){
            newdata[i]=data_[i];
        }
        // TODO 7: delete[] data_ 释放旧内存。
        delete[] data_;
        // TODO 8: 让 data_ 指向新内存，并更新 capacity_。
        data_=newdata;
        capacity_=new_capacity;
    }
};

int main() {
    DynamicArray a;

    a.push_back(1);
    a.push_back(2);

    DynamicArray b=std::move(a);
    std::cout<<"a.size="<<a.size()<<std::endl;
    std::cout<<"b.size="<<b.size()<<std::endl;

    DynamicArray c;
    c.push_back(9);

    c=std::move(b);
    std::cout<<"b.size="<<b.size()<<std::endl;
    std::cout<<"c.size="<<c.size()<<std::endl;

    return 0;
}