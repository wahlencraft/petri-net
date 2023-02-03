#ifndef PLACE_H_
#define PLACE_H_

#include <cassert>
#include <array>
#include <vector>
#include <sstream>

class GenericTransition;

class GenericPlace {
public:
    GenericPlace(unsigned initial_tokens): tokens{initial_tokens} {}
    virtual ~GenericPlace() {}
    virtual GenericPlace* copy() const = 0;
    virtual void initialize(std::vector<GenericTransition*> const &transitions) = 0;
    virtual std::string str() const = 0;

    int get_tokens() const {
        return tokens;
    }

    bool has_tokens() const {
        return tokens > 0;
    }

    /* If there is tokens, decrease with 1 and return 0. Otherwise return 1. */
    void take_token() {
        ++output_tokens;
    }

    /* Increase the number of tokens with 1 */
    void give_token() {
        ++input_tokens;
    }

    /* After a global fire cycle. Call update() on all places. Return 0 if the 
     * fireing was legal, otherwise 1. If update() returns 1 the object is
     * invalid and should not be used agian. */
    int update() {
        if (tokens >= output_tokens) {
            tokens = tokens - output_tokens + input_tokens;
            input_tokens = 0;
            output_tokens = 0;
            return 0;
        } else {
            input_tokens = 0;
            output_tokens = 0;
            return 1;
        }
    }

private:
    unsigned tokens;
    unsigned input_tokens{0};
    unsigned output_tokens{0};
};

template <unsigned PREDECESSORS, unsigned SUCCESSORS>
class Place: public GenericPlace {
public:
    Place(unsigned initial_tokens,
          std::array<unsigned, PREDECESSORS> const &predecessor_map,
          std::array<unsigned, SUCCESSORS> const &successor_map):
        GenericPlace(initial_tokens),
        predecessor_map{predecessor_map}, successor_map{successor_map} {}

    GenericPlace* copy() const override {
        return new Place<PREDECESSORS, SUCCESSORS>(get_tokens(), predecessor_map, successor_map);
    }

    void initialize(std::vector<GenericTransition*> const &transitions) override {
        for (unsigned i = 0; i < PREDECESSORS; ++i)
            predecessors[i] = transitions[predecessor_map[i]];
        for (unsigned i = 0; i < SUCCESSORS; ++i)
            successors[i] = transitions[successor_map[i]];
    }

    std::string str() const override {
        std::string const class_name = "Place<" + std::to_string(PREDECESSORS) + ", " + std::to_string(SUCCESSORS) + ">";
        std::ostringstream input_map;
        for (unsigned i = 0; i < PREDECESSORS; ++i)
            input_map << "T" << predecessor_map[i] << " ";
        std::ostringstream output_map;
        for (unsigned i = 0; i < SUCCESSORS; ++i)
            output_map << "T" << successor_map[i] << " ";
        return class_name + " " + input_map.str() + "-> " + output_map.str() + " (" + std::to_string(get_tokens()) + ")";
    }

private:
    std::array<GenericTransition*, PREDECESSORS> predecessors{};
    std::array<GenericTransition*, SUCCESSORS> successors{};
    std::array<unsigned, PREDECESSORS> const &predecessor_map;
    std::array<unsigned, SUCCESSORS> const &successor_map;
};

#endif  // PLACE_H_

