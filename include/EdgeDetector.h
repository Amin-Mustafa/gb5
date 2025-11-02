#ifndef EDGE_DETECTOR_H
#define EDGE_DETECTOR_H

class EdgeDetector {
private:
    bool previous = false;
public:
    bool rising_edge(bool current) {
        bool rising = !previous && current;
        previous = current;
        return rising;
    }
    bool falling_edge(bool current) {
        bool falling = previous && !current;
        previous = current;
        return falling;
    }
    void set_state(bool state) {
        previous = state;
    }
};

#endif