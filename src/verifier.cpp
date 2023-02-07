#include "verifier.h"
#include "petri_net.h"
#include <vector>
#include <iostream>

using namespace std;

Verifier::Verifier(): max_depth{10} {}

void Verifier::verify(PetriNet const &initial_net) {
    verify(initial_net, 0);
}

void Verifier::verify(PetriNet const &petri_net, unsigned current_depth) {
    cout << "\nVerify: depth=" << current_depth << endl;
    cout << "petri_net=" << petri_net.str() << endl;

    if (current_depth > max_depth) {
        throw runtime_error{"Max depth exceeded"};
    }

    unsigned const transition_count = petri_net.get_transition_count();
    vector<bool> fire_vector(transition_count, false);
    fire_vector[0] = true;
    PetriNet active_net{};
    while (fire_vector != vector<bool>(transition_count, false)) {
        cout << "fire_vector=";
        for (bool e : fire_vector)
            cout << e;
        cout << endl;
        cout << "copy begin" << endl;
        active_net = petri_net;
        cout << "copy end" << endl;
        //PetriNet active_net{petri_net};
        if (active_net.fire(fire_vector)) {
            cout << "Illigal" << endl;
        } else {
            cout << "Legal" << endl;
            // Legal fire, active_net is now in a new state
            vector<unsigned> const &state = active_net.get_state();
            if (previous_states.count(state) == 0) {
                // This is a new state
                previous_states.insert(state);
                verify(active_net, current_depth + 1);
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
}

