#ifndef CONSTRAINTS_H_
#define CONSTRAINTS_H_

#include <string>
#include <vector>
#include <unordered_set>
#include <memory>

#include "petri_net_state.h"
#include "petri_net_parser.h"

class Constraints {
public:
    Constraints(std::shared_ptr<PetriNetParser> const parser);
    void set_max_depth(unsigned depth);
    void set_require_live(bool yes=true);
    void set_illegal_state(PetriNetState const &state);

    /* Set the token bound for one place. */
    void set_bound(std::string const &place_name, unsigned tokens);

    /* Set the token bound for all places.
     *
     * Will not override a previous stricter bound. */
    void set_global_bound(unsigned tokens);

    void clear_bounds();

    std::vector<unsigned> const& get_max_tokens() const;
    unsigned get_max_token(unsigned index) const;
    long unsigned const& max_depth() const;
    bool const& require_live() const;
    bool is_illegal(PetriNetState const &state) const;

private:
    std::vector<unsigned> _max_tokens;
    long unsigned _max_depth = 10000;
    bool _require_live = false;
    std::unordered_set<PetriNetState, PetriNetStateHash> illegal_states{};
    std::shared_ptr<PetriNetParser> parser;
};

#endif  // CONSTRAINTS_H_

