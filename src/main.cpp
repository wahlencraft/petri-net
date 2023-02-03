#include "petri.h"
#include "petri_net.h"

#include <iostream>
#include <vector>

using namespace std;

int main() {

    cout << "Main" << endl;

    // Place mappings
    array<unsigned, 0> p0_in_map{};
    array<unsigned, 1> p1_in_map{0};
    array<unsigned, 1> p2_in_map{1};

    array<unsigned, 2> p0_out_map{0, 1};
    array<unsigned, 0> p1_out_map{};
    array<unsigned, 0> p2_out_map{};

    // Transition mappings
    array<unsigned, 1> t0_in_map{0};
    array<unsigned, 1> t1_in_map{0};

    array<unsigned, 1> t0_out_map{1};
    array<unsigned, 1> t1_out_map{2};

    vector<GenericPlace*> places{
        new Place<0, 2>(1, p0_in_map, p0_out_map),
        new Place<1, 0>(0, p1_in_map, p1_out_map),
        new Place<1, 0>(0, p2_in_map, p2_out_map)
    };

    vector<GenericTransition*> transitions{
        new Transition<1, 1>(t0_in_map, t0_out_map),
        new Transition<1, 1>(t1_in_map, t1_out_map)
    };

    for (auto transition : transitions)
        transition->initialize(places);
    for (auto place : places)
        place->initialize(transitions);

    for (auto transition : transitions)
        cout << transition << ": " << transition->str() << "\n";
    for (auto place : places)
        cout << place << ": " << place->str() << "\n";

    // Copy
    vector<GenericPlace*> places_c{
        places[0]->copy(),
        places[1]->copy(),
        places[2]->copy()
    };

    vector<GenericTransition*> transitions_c{
        transitions[0]->copy(),
        transitions[1]->copy()
    };

    cout << "\n";

    for (auto transition : transitions_c)
        transition->initialize(places);
    for (auto place : places)
        place->initialize(transitions_c);

    for (auto transition : transitions_c)
        cout << transition << ": " << transition->str() << "\n";
    for (auto place : places_c)
        cout << place << ": " << place->str() << "\n";


    return 0;
}

