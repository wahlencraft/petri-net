#include "constraints.h"

#include <climits>

using namespace std;

Constraints::Constraints(shared_ptr<PetriNetParser> const parser):
    _max_tokens(parser->get_found_places(), UINT_MAX), parser{parser} {}

// Setters

void Constraints::set_max_depth(unsigned depth) {
    _max_depth = depth;
}

void Constraints::set_require_live(bool yes) {
    _require_live = yes;
}

void Constraints::set_illegal_state(PetriNetState const &state) {
    illegal_states.insert(state);
}

void Constraints::set_bound(std::string const &place_name, unsigned tokens) {
    unsigned i = parser->get_place_index(place_name);
    _max_tokens[i] = tokens;
}

void Constraints::set_global_bound(unsigned tokens) {
    for (unsigned i = 0; i < _max_tokens.size(); ++i) {
        _max_tokens[i] = min(_max_tokens[i], tokens);
    }
}

void Constraints::clear_bounds() {
    for (unsigned i = 0; i < _max_tokens.size(); ++i) {
        _max_tokens[i] = UINT_MAX;
    }
}

// Getters

vector<unsigned> const& Constraints::get_max_tokens() const {
    return _max_tokens;
}

unsigned Constraints::get_max_token(unsigned index) const {
    return _max_tokens[index];
}

long unsigned const& Constraints::max_depth() const {
    return _max_depth;
}

bool const& Constraints::require_live() const {
    return _require_live;
}

bool Constraints::is_illegal(PetriNetState const &state) const {
    if (illegal_states.count(state) > 0)
        return true;
    else
        return false;
}
