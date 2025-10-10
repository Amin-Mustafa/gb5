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

    T front() const { return data[head]; }
    uint8_t count() const { return num_elements; }
    bool empty() const { return num_elements == 0; }
    bool full() const { return num_elements == data.size(); }

    void clear() {
        std::fill(data.begin(), data.end(), T());
        head = 0;
        tail = data.size() - 1;
        num_elements = 0;
    }
};

using PixelFifo = RingBuffer<uint8_t, 16>;


#endif