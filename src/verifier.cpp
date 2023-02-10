#include "verifier.h"
#include "petri_net.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

Verifier::Verifier(): constraints{} {}

void Verifier::set_constraints(Constraints const &c) {
    constraints = c;
}

void Verifier::verify(PetriNet const &initial_net) {
    previous_states.clear();
    token_max = initial_net.get_state();
    verify(initial_net, 0);
}

void Verifier::verify(PetriNet const &petri_net, unsigned current_depth) {

    if (current_depth > constraints.max_depth) {
        throw runtime_error{string("Verifier reached max depth (") + to_string(constraints.max_depth) + string(")")};
    }

    check_boundness(petri_net);
    check_reachability(petri_net);

    previous_states.insert(petri_net.get_state());

    unsigned const transition_count = petri_net.get_transition_count();
    vector<bool> fire_vector(transition_count, false);
    fire_vector[0] = true;
    unsigned live_transitions = 0;
    while (fire_vector != vector<bool>(transition_count, false)) {
        PetriNet active_net{petri_net};
        if (active_net.fire(fire_vector)) {
            // Illegal fire
        } else {
            ++live_transitions;
            // Legal fire, active_net is now in a new state
            vector<unsigned> const &state = active_net.get_state();
            if (previous_states.count(state) == 0) {
                // This is a new state
                verify(active_net, current_depth + 1);
            } else {
                // Known state
            }
        }

        // Increment the fire vector
        for (unsigned i = 0; i < transition_count; ++i) {
            if (fire_vector[i] == false) {
                fire_vector[i] = true;
                break;
            } else {
                fire_vector[i] = false;
            }
        }
    }

    if (live_transitions == 0) {
        if (constraints.require_live) {
            throw LivenessException{"PetriNet no longer live"};
        }
    }
}

void Verifier::check_boundness(PetriNet const &net) {
    size_t const size = constraints.max_tokens.size();
    if (size > 0) {
        vector<unsigned> current_state = net.get_state();
        assert(size == current_state.size());
        stringstream ss;
        bool constraints_satisfied = true;
        for (unsigned i=0; i < size; ++i) {
            ss << current_state[i] << "/" << constraints.max_tokens[i] << " ";
            if (current_state[i] > constraints.max_tokens[i]) {
                constraints_satisfied = false;
            }
            token_max[i] = max(token_max[i], current_state[i]);
        }
        if (!constraints_satisfied) {
            throw BoundednessException("Net does not satisfy boundedness constraints: " + ss.str());
        }
    }
}

void Verifier::check_reachability(PetriNet const &net) {
    if (constraints.illegal_states.count(net.get_state())) {
        stringstream ss;
        for (auto token : net.get_state())
            ss << token << " ";
        throw ReachabilityException{"Illegal state (" + ss.str() + ") reached"};
    }
}

std::vector<unsigned> const & Verifier::get_max_bounds() const {
    return token_max;
}

bool Verifier::reached_state(vector<unsigned> const &state) const {
    return previous_states.count(state);
}

size_t Verifier::get_state_count() const {
    return previous_states.size();
}

