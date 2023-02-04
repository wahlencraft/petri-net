#ifndef PETRI_NET_H_
#define PETRI_NET_H_

#include <initializer_list>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

class PetriNetParser {
public:
    unsigned get_place_index(std::string const &name);
    unsigned get_transition_index(std::string const &name);

    std::vector<unsigned> get_in_mapping(std::string const &);
    std::vector<unsigned> get_out_mapping(std::string const &);

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

    std::string str() const;

private:
    std::vector<std::vector<unsigned>> transition_in_mappings{};
    std::vector<std::vector<unsigned>> transition_out_mappings{};
};

#endif  // PETRI_NET_H_

