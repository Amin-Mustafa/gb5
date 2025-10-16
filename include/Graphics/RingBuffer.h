#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <cstdint>
#include <array>
#include <iostream>

//basic circular RingBuffer structure
template <typename T, uint8_t Size>
class RingBuffer {
private:
    std::array<T, Size> data;
    uint8_t head;   //oldest element
    uint8_t tail;   //newest element
    uint8_t num_elements;  //number of elements
public:
    RingBuffer()
        :data{}, head{0}, tail{data.size() - 1}, num_elements{0} {}

    //iterator stuff
    class iterator;
    class const_iterator;
    iterator begin() {
        return iterator{&data, head, (tail + (uint8_t)1)%Size};
    }
    iterator end() {
        return iterator{&data, (tail + (uint8_t)1)%Size, (tail + (uint8_t)1)%Size};
    }
    const_iterator begin() const {
        return const_iterator{&data, head, (tail + (uint8_t)1)%Size};
    }
    const_iterator end() const {
        return const_iterator{&data, (tail + (uint8_t)1)%Size, (tail+(uint8_t)1)%Size};
    }

    //queue operations
    void push(T val) {
        tail = (tail + 1) % data.size();
        data[tail] = val; 
        num_elements++;
    }
    T pop() {
        T first = data[head];
        head = (head + 1) % data.size();
        num_elements--;
        return first;
    }
    
    //getter/setter
    T front() const { return data[head]; }
    uint8_t count() const { return num_elements; }
    bool empty() const { return num_elements == 0; }
    bool full() const { return num_elements == data.size();}

    void clear() {
        head = 0;
        tail = data.size() - 1;
        num_elements = 0;
    }

    //dbg stuff
    void print() {
        for(const auto& d : data) {
            std::cout << (int)d << ' ';
        } 
        std::cout << "HEAD/TAIL: " << (int)head << "/" << (int)tail << '\n';
    }
};

//RingBuffer iteration
template<typename T, uint8_t Size>
class RingBuffer<T,Size>::iterator {
private:
    std::array<T, Size>* data;
    uint8_t index;
    uint8_t end_index;
public:
    iterator(std::array<T, Size>* arr, uint8_t start, uint8_t end) :
        data{arr}, 
        index{start},
        end_index{end}
        {}

    iterator& operator++() {
        index = (index + 1) % Size;
        return *this;
    }
    T& operator*() { return (*data)[index]; }
    bool operator==(const iterator& other) {
        return (data == other.data) && (index == other.index);
    }
    bool operator!=(const iterator& other) { 
        return (data != other.data) || (index != other.index);
    }
};

template<typename T, uint8_t Size>
class RingBuffer<T, Size>::const_iterator {
private:
    const std::array<T, Size>* data;
    uint8_t index;
    uint8_t end_index;
public:
    const_iterator(const std::array<T, Size>* arr, uint8_t start, uint8_t end) :
        data{arr}, 
        index{start},
        end_index{end}
        {}

    const_iterator& operator++() {
        index = (index + 1) % Size;
        return *this;
    }
    const T& operator*() { return (*data)[index]; }
    bool operator==(const iterator& other) {
        return (data == other.data) && (index == other.index);
    }
    bool operator!=(const iterator& other) { 
        return (data != other.data) || (index != other.index);
    }
};

//aliases
class SpritePixel;
using BgFifo = RingBuffer<uint8_t, 16>;
using SprFifo = RingBuffer<SpritePixel, 16>;


#endif