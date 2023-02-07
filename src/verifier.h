#ifndef VERIFIER_H_
#define VERIFIER_H_

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "petri_net.h"

struct VectorHash {
    size_t operator()(std::vector<unsigned> const &v) const {
        std::hash<unsigned> hasher;
        size_t seed = 0;
        for (unsigned i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
        return seed;
    }
};

class Verifier {
public:
    Verifier();

    void verify(PetriNet const &initial_net);
private:
    void verify(PetriNet const &initial_net, unsigned current_depth);

    std::unordered_set<std::vector<unsigned>, VectorHash> previous_states{};
    std::unordered_set<std::vector<unsigned>, VectorHash> illigal_states{};
    std::unordered_map<std::vector<unsigned>, unsigned, VectorHash> target_states{};
    unsigned const max_depth;
};

#endif  // VERIFIER_H_

