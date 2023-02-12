#include "petri_net_parser.h"

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


unordered_map<string, unsigned> const& PetriNetParser::get_place_map() const {
    return places;
}

unordered_map<string, unsigned> const& PetriNetParser::get_transition_map() const {
    return transitions;
}
