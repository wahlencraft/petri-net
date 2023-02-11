#ifndef PETRI_NET_STATE_H_
#define PETRI_NET_STATE_H_

#include <vector>
#include <cstddef>
#include <iostream>

struct PetriNetState {
    PetriNetState(std::vector<unsigned> tokens);

    bool operator==(PetriNetState const &other) const;

    std::vector<unsigned> tokens;
};

struct PetriNetStateHash {
    size_t operator()(PetriNetState const &v) const;
};

std::ostream& operator<<(std::ostream &os, PetriNetState const &state);

#endif  // PETRI_NET_STATE_H_

