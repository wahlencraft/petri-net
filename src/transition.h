#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <cassert>
#include <array>
#include <vector>
#include <sstream>

class GenericPlace;

class GenericTransition {
public:
    virtual ~GenericTransition() {}
    virtual GenericTransition* copy() const = 0;
    virtual void initialize(std::vector<GenericPlace*> const &places) = 0;
    virtual std::string str() const = 0;

    /* If all preceding Places has tokens return true */
    virtual bool enabled() const = 0;

    /* Prepare all preceding places for a fire. If the fire-pattern is legal
     * the preceding places will decrease thair tokens next Place::update() */
    virtual void fire() = 0;
};

template <unsigned PREDECESSORS, unsigned SUCCESSORS>
class Transition: public GenericTransition {
public:
    Transition(std::array<unsigned, PREDECESSORS> const &predecessor_map,
               std::array<unsigned, SUCCESSORS> const &successor_map):
        predecessor_map{predecessor_map}, successor_map{successor_map} {}

    bool enabled() const override {
        for (unsigned i = 0; i < PREDECESSORS; ++i) {
            if (!predecessors[i]->has_tokens())
                return false;
        }
        return true;
    }

    GenericTransition* copy() const override {
        return new Transition<PREDECESSORS, SUCCESSORS> (predecessor_map, successor_map);
    }

    void initialize(std::vector<GenericPlace*> const &places) override {
        for (unsigned i = 0; i < PREDECESSORS; ++i)
            predecessors[i] = places[predecessor_map[i]];
        for (unsigned i = 0; i < SUCCESSORS; ++i)
            successors[i] = places[successor_map[i]];
    }

    std::string str() const override {
        std::string const class_name = "Transition<" + std::to_string(PREDECESSORS) + ", " + std::to_string(SUCCESSORS) + ">";
        std::ostringstream input_map;
        for (unsigned i = 0; i < PREDECESSORS; ++i)
            input_map << "P" << predecessor_map[i] << " ";
        std::ostringstream output_map;
        for (unsigned i = 0; i < SUCCESSORS; ++i)
            output_map << "P" << successor_map[i] << " ";
        return class_name + " " + input_map.str() + "-> " + output_map.str();
    }

    void fire() override {
        for (unsigned i = 0; i < PREDECESSORS; ++i)
            predecessors[i]->take_token();
        for (unsigned i = 0; i < SUCCESSORS; ++i)
            successors[i]->give_token();
    }

private:
    std::array<GenericPlace*, PREDECESSORS> predecessors{nullptr};
    std::array<GenericPlace*, SUCCESSORS> successors{nullptr};
    std::array<unsigned, PREDECESSORS> const predecessor_map;
    std::array<unsigned, SUCCESSORS> const successor_map;
    unsigned set_inputs{0};
};

#endif  // TRANSITION_H_

