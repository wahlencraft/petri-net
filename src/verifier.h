#ifndef VERIFIER_H_
#define VERIFIER_H_

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "petri_net.h"
#include "verification_exceptions.h"

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

struct Constraints {
    std::vector<long unsigned> max_tokens{};
    long unsigned max_depth = 10000;
    bool require_live = false;
    std::unordered_set<std::vector<long unsigned>, VectorHash> reachable{};
    std::unordered_set<std::vector<long unsigned>, VectorHash> un_reachable{};
};

class Verifier {
public:
    Verifier();

    void verify(PetriNet const &initial_net);
    void set_constraints(Constraints const &constraints);
private:
    void verify(PetriNet const &initial_net, unsigned current_depth);

    struct Constraints constraints;
    std::unordered_set<std::vector<unsigned>, VectorHash> previous_states{};
};

#endif  // VERIFIER_H_

