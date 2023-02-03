#ifndef PETRI_H_
#define PETRI_H_

#include <initializer_list>
#include <cassert>
#include <array>
#include <vector>
#include <iterator>
#include <sstream>

class GenericPlace;

class GenericTransition {
public:
    virtual ~GenericTransition() {}
    virtual GenericTransition* copy() const = 0;
    virtual void initialize(std::vector<GenericPlace*> const &places) = 0;
    virtual std::string str() const = 0;
};

class GenericPlace {
public:
    virtual ~GenericPlace() {}
    virtual GenericPlace* copy() const = 0;
    virtual void initialize(std::vector<GenericTransition*> const &transitions) = 0;
    virtual std::string str() const = 0;
};

//std::ostream& operator<<(std::ostream &os, GenericTransition const *transition) {
    //unsigned ptr = static_cast<unsigned>(transition);
    //return os;
//}

template <unsigned PREDECESSORS, unsigned SUCCESSORS>
class Transition: public GenericTransition {
public:
    Transition(std::array<unsigned, PREDECESSORS> const &predecessor_map,
               std::array<unsigned, SUCCESSORS> const &successor_map):
        predecessor_map{predecessor_map}, successor_map{successor_map} {}

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

private:
    std::array<GenericPlace*, PREDECESSORS> predecessors{nullptr};
    std::array<GenericPlace*, SUCCESSORS> successors{nullptr};
    std::array<unsigned, PREDECESSORS> const predecessor_map;
    std::array<unsigned, SUCCESSORS> const successor_map;
    unsigned set_inputs{0};
};

template <unsigned PREDECESSORS, unsigned SUCCESSORS>
class Place: public GenericPlace {
public:
    Place(unsigned tokens,
          std::array<unsigned, PREDECESSORS> const &predecessor_map,
          std::array<unsigned, SUCCESSORS> const &successor_map):
        predecessor_map{predecessor_map}, successor_map{successor_map},
        tokens{tokens} {}

    GenericPlace* copy() const override {
        return new Place<PREDECESSORS, SUCCESSORS>(tokens, predecessor_map, successor_map);
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
        return class_name + " " + input_map.str() + "-> " + output_map.str() + " (" + std::to_string(tokens) + ")";
    }

private:
    std::array<GenericTransition*, PREDECESSORS> predecessors{};
    std::array<GenericTransition*, SUCCESSORS> successors{};
    std::array<unsigned, PREDECESSORS> const &predecessor_map;
    std::array<unsigned, SUCCESSORS> const &successor_map;
    unsigned tokens;
};

#endif  // PETRI_H_

