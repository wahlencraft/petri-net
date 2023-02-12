#include "verifier.h"
#include "petri_net.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

Verifier::Verifier(PetriNet const &petri_net): constraints{petri_net.get_parser()}, initial_net{petri_net} {}

void Verifier::verify() {
    previous_states.clear();
    token_max = initial_net.get_state().tokens;
    verify(initial_net, 0);
}

void Verifier::verify(PetriNet const &petri_net, unsigned current_depth) {

    if (current_depth > constraints.max_depth()) {
        throw runtime_error{string("Verifier reached max depth (") + to_string(constraints.max_depth()) + string(")")};
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
            PetriNetState const &state = active_net.get_state();
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
        if (constraints.require_live()) {
            throw LivenessException{"PetriNet no longer live"};
        }
    }
}

void Verifier::check_boundness(PetriNet const &net) {
    PetriNetState const &current_state = net.get_state();
    bool constraints_satisfied = true;
    for (unsigned i = 0; i < initial_net.get_place_count(); ++i) {
        if (current_state.tokens[i] > constraints.get_max_token(i))
            constraints_satisfied = false;
        token_max[i] = max(token_max[i], current_state.tokens[i]);
    }
    if (!constraints_satisfied) {
        // Print a usefull error message
        // The Place names are found in the parser, they are however stored in
        // an unordered way. I first put them in the usual order (by index).
        shared_ptr<PetriNetParser> parser = net.get_parser();
        unordered_map<string, unsigned> const &place_map = parser->get_place_map();
        vector<string> place_names(place_map.size());
        for (auto const &[place_name, index]: place_map) {
            place_names.at(index) = place_name;
        }
        stringstream ss;
        for (unsigned i = 0; i < place_names.size() - 1; ++i) {
            ss << place_names[i] << ": "
               << current_state.tokens[i] << "/"
               << constraints.get_max_token(i) << ", ";
        }
        ss << place_names[place_names.size() - 1] << ": "
           << current_state.tokens[place_names.size() - 1] << "/"
           << constraints.get_max_token(place_names.size() - 1);

        throw BoundednessException("Net does not satisfy boundedness constraints: " + ss.str());
    }
}

void Verifier::check_reachability(PetriNet const &net) {
    if (constraints.is_illegal(net.get_state())) {
        stringstream ss;
        for (auto token : net.get_state().tokens)
            ss << token << " ";
        throw ReachabilityException{"Illegal state (" + ss.str() + ") reached"};
    }
}

std::vector<unsigned> const & Verifier::get_max_bounds() const {
    return token_max;
}

string Verifier::get_max_bounds_as_string() const {
    stringstream ss;
    return "";
}

bool Verifier::reached_state(PetriNetState const &state) const {
    return previous_states.count(state);
}

size_t Verifier::get_state_count() const {
    return previous_states.size();
}

