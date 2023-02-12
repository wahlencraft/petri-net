#ifndef PETRI_NET_H_
#define PETRI_NET_H_

#include <initializer_list>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

#include "place.h"
#include "transition.h"
#include "petri_net_parser.h"
#include "petri_net_state.h"

class PetriNet {
public:
    PetriNet(std::initializer_list<std::string> lst);
    PetriNet(PetriNet const &other);
    ~PetriNet();
    PetriNet& operator=(PetriNet const &other);

    void set_state(PetriNetState const &state);
    void update_state(unsigned index, unsigned tokens);
    void update_state(std::string const &name, unsigned tokens);

    PetriNetState get_state() const;

    /* If the fire_vector is legal update the state of the PetriNet and return 0,
     * if it is not legal return 1 (the net is unchanged) */
    int fire(std::vector<bool> const &fire_vector);

    unsigned get_place_count() const;
    unsigned get_transition_count() const;
    std::shared_ptr<PetriNetParser> const& get_parser() const;

    std::string str() const;

private:
    std::vector<std::vector<unsigned>> transition_in_mappings;
    std::vector<std::vector<unsigned>> transition_out_mappings;
    std::vector<Place *> places;
    std::vector<Transition *> transitions;
    std::shared_ptr<PetriNetParser> parser;
    unsigned place_count;
    unsigned transition_count;
};

#endif  // PETRI_NET_H_

