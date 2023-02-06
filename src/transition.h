#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <vector>

#include "place.h"

class Transition {
public:
    Transition(std::vector<unsigned> const &p_map, std::vector<unsigned> const &s_map);

    /* If all preceding Places has tokens return true */
    bool enabled() const;

    Transition* copy() const;

    void initialize(std::vector<Place*> const &places);

    std::string str() const;

    /* Prepare all preceding places for a fire. If the fire-pattern is legal
     * the preceding places will decrease thair tokens next Place::update() */
    void fire();

private:
    std::vector<Place*> predecessors{nullptr};
    std::vector<Place*> successors{nullptr};
    std::vector<unsigned> const &predecessor_map;
    std::vector<unsigned> const &successor_map;
    size_t const predecessor_count;
    size_t const successor_count;
};

#endif  // TRANSITION_H_

