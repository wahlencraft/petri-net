#ifndef PETRI_NET_H_
#define PETRI_NET_H_

#include <initializer_list>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "place.h"
#include "transition.h"

class PetriNetParser {
public:
    unsigned get_place_index(std::string const &name);
    unsigned get_transition_index(std::string const &name);

    std::vector<unsigned> get_in_mapping(std::string const &);
    void register_transition(std::string const &);
    std::vector<unsigned> get_out_mapping(std::string const &);

    unsigned get_found_places() const {
        return found_places;
    }

    unsigned get_found_transitions() const {
        return found_transitions;
    }

private:
    std::unordered_map<std::string, unsigned> places{};
    std::unordered_map<std::string, unsigned> transitions{};

    unsigned found_places = 0;
    unsigned found_transitions = 0;
    char const arrow{'>'};
    std::unordered_set<char> const dividers{' ', ',', ';', '-', '\t'};
};

class PetriNet {
public:
    PetriNet(std::initializer_list<std::string> lst);
    PetriNet(PetriNet const &other);
    PetriNet& operator=(PetriNet const &other);

    void set_state(std::vector<unsigned> state);
    std::vector<unsigned> get_state() const;
    int fire(std::vector<bool> const &fire_vector);
    unsigned get_place_count() const;
    unsigned get_transition_count() const;

    std::string str() const;

private:
    std::vector<std::vector<unsigned>> transition_in_mappings{};
    std::vector<std::vector<unsigned>> transition_out_mappings{};
    std::vector<Place *> places{};
    std::vector<Transition *> transitions{};
    unsigned place_count{};
    unsigned transition_count{};
};

#endif  // PETRI_NET_H_

