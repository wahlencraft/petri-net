#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"

#include "place.h"
#include "transition.h"

using namespace std;

TEST_CASE("Place/Transition") {

    // Transition mappings
    array<unsigned, 1> t0_in_map{0};
    array<unsigned, 1> t1_in_map{0};

    array<unsigned, 1> t0_out_map{1};
    array<unsigned, 1> t1_out_map{2};

    vector<Place*> places{
        new Place(1),
        new Place(0),
        new Place(0)
    };

    vector<GenericTransition*> transitions{
        new Transition<1, 1>(t0_in_map, t0_out_map),
        new Transition<1, 1>(t1_in_map, t1_out_map)
    };

    for (auto transition : transitions)
        transition->initialize(places);

    CHECK( places[0]->has_tokens() );
    CHECK_FALSE( places[1]->has_tokens() );
    CHECK_FALSE( places[2]->has_tokens() );
    CHECK( transitions[0]->enabled() );
    CHECK( transitions[1]->enabled() );

    // Copy
    // There is 2 possible next states, if T0 fires or if T1 fires
    // I will also check what happens if both fire
    vector<Place*> places_0{
        places[0]->copy(),
        places[1]->copy(),
        places[2]->copy()
    };
    vector<Place*> places_1{
        places[0]->copy(),
        places[1]->copy(),
        places[2]->copy()
    };
    vector<Place*> places_2{
        places[0]->copy(),
        places[1]->copy(),
        places[2]->copy()
    };

    vector<GenericTransition*> transitions_0{
        transitions[0]->copy(),
        transitions[1]->copy()
    };
    vector<GenericTransition*> transitions_1{
        transitions[0]->copy(),
        transitions[1]->copy()
    };
    vector<GenericTransition*> transitions_2{
        transitions[0]->copy(),
        transitions[1]->copy()
    };

    for (auto transition : transitions_0)
        transition->initialize(places_0);

    for (auto transition : transitions_1)
        transition->initialize(places_1);

    for (auto transition : transitions_2)
        transition->initialize(places_2);

    // Fire T0
    transitions_0[0]->fire();
    CHECK( 0 == places_0[0]->update() );
    CHECK( 0 == places_0[1]->update() );
    CHECK( 0 == places_0[2]->update() );

    CHECK( 0 == places_0[0]->get_tokens() );
    CHECK( 1 == places_0[1]->get_tokens() );
    CHECK( 0 == places_0[2]->get_tokens() );

    // Fire T1
    transitions_1[1]->fire();
    CHECK( 0 == places_1[0]->update() );
    CHECK( 0 == places_1[1]->update() );
    CHECK( 0 == places_1[2]->update() );

    CHECK( 0 == places_1[0]->get_tokens() );
    CHECK( 0 == places_1[1]->get_tokens() );
    CHECK( 1 == places_1[2]->get_tokens() );

    // Fire Both
    transitions_2[0]->fire();
    transitions_2[1]->fire();
    CHECK( 1 == places_2[0]->update() );
    CHECK( 0 == places_2[1]->update() );
    CHECK( 0 == places_2[2]->update() );

    for (auto place : places)
        delete place;
    for (auto place : places_0)
        delete place;
    for (auto place : places_1)
        delete place;
    for (auto place : places_2)
        delete place;
    for (auto transition : transitions)
        delete transition;
    for (auto transition : transitions_0)
        delete transition;
    for (auto transition : transitions_1)
        delete transition;
    for (auto transition : transitions_2)
        delete transition;

}
