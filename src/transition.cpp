#include "transition.h"

#include <cassert>
#include <vector>
#include <sstream>
#include <iostream>

#include "place.h"

using namespace std;

Transition::Transition(std::vector<unsigned> const &p_map, std::vector<unsigned> const &s_map):
    predecessor_map{p_map}, successor_map{s_map},
    predecessor_count{p_map.size()}, successor_count{s_map.size()} {}

bool Transition::enabled() const {
    for (unsigned i = 0; i < predecessor_count; ++i) {
        if (!predecessors[i]->has_tokens())
            return false;
    }
    return true;
}

Transition* Transition::copy() const {
    return new Transition(predecessor_map, successor_map);
}

void Transition::initialize(std::vector<Place*> const &places) {
    for (unsigned i = 0; i < predecessor_count; ++i) {
        predecessors[i] = places[predecessor_map[i]];
    }
    for (unsigned i = 0; i < successor_count; ++i) {
        successors[i] = places[successor_map[i]];
    }
}

std::string Transition::str() const {
    std::string const class_name = "Transition<" + std::to_string(predecessor_count) + ", " + std::to_string(successor_count) + ">";
    std::ostringstream input_map;
    for (unsigned i = 0; i < predecessor_count; ++i)
        input_map << "P" << predecessor_map[i] << " ";
    std::ostringstream output_map;
    for (unsigned i = 0; i < successor_count; ++i)
        output_map << "P" << successor_map[i] << " ";
    return class_name + " " + input_map.str() + "-> " + output_map.str();
}

void Transition::fire() {
    cout << str() << "::fire()" << endl;
    for (unsigned i = 0; i < predecessor_count; ++i)
        predecessors[i]->take_token();
    for (unsigned i = 0; i < successor_count; ++i)
        successors[i]->give_token();
}

