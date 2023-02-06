#include "petri_net.h"
#include "place.h"
#include "transition.h"

#include <iostream>
#include <sstream>

using namespace std;

vector<unsigned> PetriNetParser::get_in_mapping(string const &arg) {
    vector<unsigned> in_mapping{};
    stringstream ss;
    char c;
    string name{};
    ss << arg << '\0';

    while (arrow != (c = ss.get())) {
        if (c == '\0') {
            throw runtime_error{"Unexpected end of argument"};
        } else if (dividers.count(c) == 0) {
            // c is a part of a place name
            name.push_back(c);
        } else {
            // c is a divider character
            if (name.size() > 0) {
                in_mapping.push_back(get_place_index(name));
                name.clear();
            }
        }
    }

    if (name.size() > 0) {
        in_mapping.push_back(get_place_index(name));
        name.clear();
    }

    in_mapping.shrink_to_fit();
    return in_mapping;
}

void PetriNetParser::register_transition(string const &arg) {
    ++found_transitions;
}

vector<unsigned> PetriNetParser::get_out_mapping(string const &arg) {
    vector<unsigned> out_mapping{};
    stringstream ss;
    char c;
    string name{};
    ss << arg << '\0';

    while (arrow != (c = ss.get())) {
        if (c == '\0') {
            throw runtime_error{"Unexpected end of argument"};
        }
    }

    while (arrow != (c = ss.get())) {
        if (c == '\0') {
            throw runtime_error{"Unexpected end of argument"};
        }
    }

    while ('\0' != (c = ss.get())) {
        if (dividers.count(c) == 0) {
            // c is a part of a place name
            name.push_back(c);
        } else {
            // c is a divider character
            if (name.size() > 0) {
                out_mapping.push_back(get_place_index(name));
                name.clear();
            }
        }
    }

    if (name.size() > 0) {
        out_mapping.push_back(get_place_index(name));
        name.clear();
    }

    out_mapping.shrink_to_fit();
    return out_mapping;
}

unsigned PetriNetParser::get_place_index(string const &name) {
    if (places.count(name) == 0) {
        // This is a new place
        places[name] = found_places++;
    }
    return places.at(name);
}

unsigned PetriNetParser::get_transition_index(string const &name) {
    if (transitions.count(name) == 0) {
        // This is a new transition
        transitions[name] = found_transitions++;
    }
    return transitions.at(name);
}

PetriNet::PetriNet(std::initializer_list<string> lst) {
    PetriNetParser parser{};
    for (string arg : lst) {
        transition_in_mappings.push_back(parser.get_in_mapping(arg));
        parser.register_transition(arg);
        transition_out_mappings.push_back(parser.get_out_mapping(arg));
    }
    place_count = parser.get_found_places();
    transition_count = parser.get_found_transitions();
    cout << "transition_count=" << transition_count << endl;

    places.resize(place_count);
    transitions.resize(transition_count);

    for (unsigned i = 0; i < place_count; ++i) {
        places[i] = new Place(0);
    }

    for (unsigned i = 0; i < transition_count; ++i) {
        vector<unsigned> const &transition_in_mapping = transition_in_mappings[i];
        vector<unsigned> const &transition_out_mapping = transition_out_mappings[i];
        transitions[i] = new Transition(transition_in_mapping, transition_out_mapping);
    }

    for (unsigned i = 0; i < transition_count; ++i) {
        transitions[i]->initialize(places);
    }
}

void PetriNet::set_state(vector<unsigned> state) {
    assert(state.size() == place_count);
    for (unsigned i = 0; i < place_count; ++i) {
        places[i]->set_tokens(state[i]);
    }
}

vector<unsigned> PetriNet::get_state() const {
    vector<unsigned> state(place_count);
    for (unsigned i=0; i<place_count; ++i)
        state[i] = places[i]->get_tokens();
    return state;
}

int PetriNet::fire(std::vector<bool> const &fire_vector) {

    // Fire every Transition which is marked by fire_vector
    for (unsigned i=0; i < transition_count; ++i) {
        if (fire_vector[i]) {
            transitions[i]->fire();
        }
    }

    // Update every Place. If a Place reports an error return 1
    for (unsigned i=0; i < place_count; ++i) {
        if (places[i]->update()) {
            return 1;
        }
    }

    // No errors
    return 0;
}

string PetriNet::str() const {
    stringstream ss;
    for (unsigned i = 0; i < transition_in_mappings.size(); ++i) {
        for (unsigned idx : transition_in_mappings[i])
            ss << idx << " ";
        ss << "-> T -> ";
        for (unsigned idx : transition_out_mappings[i])
            ss << idx << " ";
        ss << ", ";
    }
    return ss.str();
}
