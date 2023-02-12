#ifndef PETRI_NET_PARSER_H_
#define PETRI_NET_PARSER_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

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


    std::unordered_map<std::string, unsigned> const& get_place_map() const;
    std::unordered_map<std::string, unsigned> const& get_transition_map() const;

private:
    std::unordered_map<std::string, unsigned> places{};
    std::unordered_map<std::string, unsigned> transitions{};

    unsigned found_places = 0;
    unsigned found_transitions = 0;
    char const arrow{'>'};
    std::unordered_set<char> const dividers{' ', ',', ';', '-', '\t'};
};

#endif  // PETRI_NET_PARSER_H_

