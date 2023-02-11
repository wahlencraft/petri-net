#ifndef VERIFIER_H_
#define VERIFIER_H_

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "petri_net.h"
#include "verification_exceptions.h"
#include "constraints.h"

class Verifier {
public:
    Verifier(PetriNet const &petri_net);

    void verify();
    std::vector<unsigned> const& get_max_bounds() const;
    std::string get_max_bounds_as_string() const;
    bool reached_state(PetriNetState const &state) const;
    size_t get_state_count() const;

    Constraints constraints;
private:
    void verify(PetriNet const &initial_net, unsigned current_depth);
    void check_boundness(PetriNet const &net);
    void check_reachability(PetriNet const &net);

    std::unordered_set<PetriNetState, PetriNetStateHash> previous_states{};
    PetriNet const initial_net;
    std::vector<unsigned> token_max{};
};

#endif  // VERIFIER_H_

