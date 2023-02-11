#include "petri_net_state.h"

using namespace std;

PetriNetState::PetriNetState(vector<unsigned> tokens): tokens{tokens} {}

bool PetriNetState::operator==(PetriNetState const &other) const {
    return tokens == other.tokens;
}

size_t PetriNetStateHash::operator()(PetriNetState const &state) const {
    std::hash<unsigned> hasher;
    size_t seed = 0;
    for (unsigned token : state.tokens) {
        seed ^= hasher(token) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    return seed;
}

ostream& operator<<(ostream &os, PetriNetState const &state) {
    os << "{ ";
    for (unsigned token : state.tokens)
        os << token << " ";
    os << "}";
    return os;
}

