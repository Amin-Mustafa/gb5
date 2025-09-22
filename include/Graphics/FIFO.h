#ifndef FIFO_H
#define FIFO_H

#include <cstdint>
#include <array>
#include <iostream>

//basic circular FIFO structure
class FIFO {
private:
    std::array<uint8_t, 16> data;
    uint8_t head;   //oldest element
    uint8_t tail;   //newest element
    uint8_t num_elements;  //number of elements
public:
    FIFO()
        :data{}, head{0}, tail{data.size() - 1}, num_elements{0} {}
    
    void push(uint8_t val) {
        tail = (tail + 1) % data.size();
        data[tail] = val; 
        num_elements++;
    }
    uint8_t pop() {
        uint8_t first = data[head];
        head = (head + 1) % data.size();
        num_elements--;
        return first;
    }
    uint8_t count() const {return num_elements;}
    bool empty() const {return num_elements == 0;}
    bool full() const {return num_elements == data.size();}
    void clear() {
        std::fill(data.begin(), data.end(), 0);
        head = 0;
        tail = data.size() - 1;
        num_elements = 0;
    }

    //dbg stuff
    void print() {
        for(const auto& n : data) std::cout << (int)n << " ";
        std::cout << "\t(" << (int)num_elements << "elements. Head/Tail: "
                  << (int)head << "/" << (int)tail << ")" << std::endl;
    }
};

#endif