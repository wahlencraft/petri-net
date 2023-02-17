#include "verifier.h"
#include "petri_net.h"
#include <vector>
#include <iostream>
#include <string>

using namespace std;

Verifier::Verifier(PetriNet const &petri_net):
    constraints{petri_net.get_parser()}, initial_net{petri_net} {}

Verifier::~Verifier() {}

void Verifier::verify() {
    previous_states.clear();
    abort = false;
    token_max = initial_net.get_state().tokens;
    verify(initial_net, 0);
    // Todo wait for all threads
    if (abort) {
        rethrow_exception(exception);
    }
}

void Verifier::verify(PetriNet const &petri_net, unsigned current_depth) {

    if (current_depth > constraints.max_depth()) {
        abort = true;
        string msg{
            string("Verifier reached max depth (") +
            to_string(constraints.max_depth()) + string(")")
        };
        exception = make_exception_ptr(runtime_error{msg});
    }

    check_boundness(petri_net);
    check_reachability(petri_net);

    unsigned const transition_count = petri_net.get_transition_count();
    vector<bool> fire_vector(transition_count, false);
    fire_vector[0] = true;
    unsigned live_transitions = 0;
    PetriNet active_net{petri_net};
    bool last_fire_was_illegal = false;
    bool first_itteration = true;
    while (fire_vector != vector<bool>(transition_count, false)) {
        if (last_fire_was_illegal || first_itteration) {
            // We can keep the old value of active_net
            first_itteration = false;
        } else {
            // Make a new copy
            active_net = petri_net;
        }

        if (active_net.fire(fire_vector)) {
            // Illegal fire
            last_fire_was_illegal = true;
        } else {
            last_fire_was_illegal = false;
            ++live_transitions;
            // Legal fire, active_net is now in a new state
            PetriNetState const &state = active_net.get_state();
            if (!reached_state(state)) {
                // This is a new state
                if (!abort)
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

    check_liveness(petri_net, live_transitions, current_depth);

}

void Verifier::check_boundness(PetriNet const &net) {
    std::unique_lock lock(mtx);
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
        exception = make_exception_ptr(BoundednessException("Net does not satisfy boundedness constraints: " + ss.str()));
        abort = true;
    }
}

void Verifier::check_reachability(PetriNet const &net) {
    if (constraints.is_illegal(net.get_state())) {
        abort = true;
        std::unique_lock lock(mtx);
        stringstream ss;
        for (auto token : net.get_state().tokens)
            ss << token << " ";
        exception = make_exception_ptr(
            ReachabilityException{"Illegal state (" + ss.str() + ") reached"}
        );
    }
}

void Verifier::check_liveness(PetriNet const &petri_net, unsigned live_transitions, unsigned current_depth) {
    if (live_transitions == 0) {
        if (constraints.require_live()) {
            abort = true;
            std::unique_lock lock(mtx);
            exception = make_exception_ptr(
                    LivenessException{
                        "PetriNet not live at depth " + to_string(current_depth) +
                        "\n\tState: " + petri_net.str()
                    }
            );
        }
    }
}

std::vector<unsigned> const & Verifier::get_max_bounds() const {
    return token_max;
}

string Verifier::get_max_bounds_as_string() const {
    stringstream ss;
    return "";
}

bool Verifier::reached_state(PetriNetState const &state) {
    std::unique_lock lock(mtx);
    if (previous_states.count(state) > 0) {
        return true;
    } else {
        previous_states.insert(state);
        return false;
    }
}

size_t Verifier::get_state_count() const {
    return previous_states.size();
}

