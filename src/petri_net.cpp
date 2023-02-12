#include "petri_net.h"
#include "place.h"
#include "transition.h"

#include <iostream>
#include <sstream>

using namespace std;

PetriNet::PetriNet(std::initializer_list<string> lst):
    transition_in_mappings{}, transition_out_mappings{},
    places{}, transitions{},
    parser{shared_ptr<PetriNetParser>(new PetriNetParser())},
    place_count{}, transition_count{} {
    for (string arg : lst) {
        transition_in_mappings.push_back(parser->get_in_mapping(arg));
        parser->register_transition(arg);
        transition_out_mappings.push_back(parser->get_out_mapping(arg));
    }
    place_count = parser->get_found_places();
    transition_count = parser->get_found_transitions();

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
        transitions.at(i)->initialize(places);
    }
}

PetriNet::PetriNet(PetriNet const &other):
    transition_in_mappings{other.transition_in_mappings},
    transition_out_mappings{other.transition_out_mappings},
    places(other.place_count, nullptr),
    transitions(other.transition_count, nullptr),
    parser{other.parser},
    place_count{other.place_count},
    transition_count{other.transition_count} {

    for (unsigned i = 0; i < place_count; ++i) {
        places[i] = new Place(other.places[i]->get_tokens());
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

PetriNet::~PetriNet() {
    for (auto transition : transitions)
        delete transition;
    for (auto place : places)
        delete place;
}

PetriNet& PetriNet::operator=(PetriNet const &other) {

    PetriNet tmp{other};

    std::swap(place_count, tmp.place_count);
    std::swap(transition_count, tmp.transition_count);
    std::swap(transition_in_mappings, tmp.transition_in_mappings);
    std::swap(transition_out_mappings, tmp.transition_out_mappings);
    std::swap(places, tmp.places);
    std::swap(transitions, tmp.transitions);

    return *this;
}

void PetriNet::set_state(PetriNetState const & state) {
    assert(state.tokens.size() == place_count);
    for (unsigned i = 0; i < place_count; ++i) {
        places[i]->set_tokens(state.tokens[i]);
    }
}

void PetriNet::update_state(unsigned index, unsigned tokens) {
    places.at(index)->set_tokens(tokens);
}

void PetriNet::update_state(string const &name, unsigned tokens) {
    unsigned index = parser->get_place_index(name);
    places.at(index)->set_tokens(tokens);
}

PetriNetState PetriNet::get_state() const {
    vector<unsigned> tokens(place_count, 0);
    for (unsigned i = 0; i < place_count; ++i) {
        tokens[i] = places[i]->get_tokens();
    }
    return PetriNetState(tokens);
}

unsigned PetriNet::get_place_count() const {
    return place_count;
}

unsigned PetriNet::get_transition_count() const {
    return transition_count;
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

std::shared_ptr<PetriNetParser> const& PetriNet::get_parser() const {
    return parser;
}

string PetriNet::str() const {
    unordered_map<string, unsigned> const &place_map = parser->get_place_map();
    vector<string> place_names(place_map.size());
    for (auto const &[place_name, index]: place_map) {
        place_names.at(index) = place_name;
    }
    stringstream ss;
    PetriNetState const &current_state = get_state();
    for (unsigned i = 0; i < place_names.size() - 1; ++i) {
        ss << place_names[i] << ": "
           << current_state.tokens[i] << ", ";
    }
    ss << place_names[place_names.size() - 1] << ": "
       << current_state.tokens[place_names.size() - 1];
    return ss.str();
}
