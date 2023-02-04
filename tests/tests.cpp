#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"

#include "place.h"
#include "transition.h"
#include "petri_net.h"

#include <iostream>

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

TEST_CASE("PetriNetParser") {
    SECTION("Basics") {
        PetriNetParser parser{};
        vector<unsigned> in_mapping0 = parser.get_in_mapping("P0, P1 > T0 > P2");
        vector<unsigned> in_mapping1 = parser.get_in_mapping("P0,P1>T0>P2");
        vector<unsigned> in_mapping2 = parser.get_in_mapping("P0, P1 -> T0 -> P2");
        CHECK( in_mapping0 == vector<unsigned>{0, 1} );
        CHECK( in_mapping1 == vector<unsigned>{0, 1} );
        CHECK( in_mapping1 == vector<unsigned>{0, 1} );
        vector<unsigned> out_mapping0 = parser.get_out_mapping("P0, P1 > T0 > P2");
        vector<unsigned> out_mapping1 = parser.get_out_mapping("P0,P1>T0>P2");
        vector<unsigned> out_mapping2 = parser.get_out_mapping("P0, P1 -> T0 -> P2");
        CHECK( out_mapping0 == vector<unsigned>{2} );
        CHECK( out_mapping1 == vector<unsigned>{2} );
        CHECK( out_mapping2 == vector<unsigned>{2} );
    }

    SECTION("Bad data") {
        PetriNetParser parser{};
        CHECK_THROWS( parser.get_in_mapping("P0,P1") );
        CHECK_THROWS( parser.get_out_mapping("P0,P1") );
        CHECK_THROWS( parser.get_out_mapping("P0,P1> T0") );
    }

    SECTION("Full graph") {
        PetriNetParser parser{};

        // Each PX will be given an internal index
        // In this case they appear in numerical order, so
        // PX will have index X

        string arg0 = "P0     -> T0 -> P1";
        string arg1 = "P1     -> T1 -> P0, P2";
        string arg2 = "P2, P3 -> T2 -> P4";
        string arg3 = "P4     -> T3 -> P3";

        vector<unsigned> in_mapping0 = parser.get_in_mapping(arg0);
        vector<unsigned> in_mapping1 = parser.get_in_mapping(arg1);
        vector<unsigned> in_mapping2 = parser.get_in_mapping(arg2);
        vector<unsigned> in_mapping3 = parser.get_in_mapping(arg3);
        CHECK( in_mapping0 == vector<unsigned>{0} );
        CHECK( in_mapping1 == vector<unsigned>{1} );
        CHECK( in_mapping2 == vector<unsigned>{2, 3} );
        CHECK( in_mapping3 == vector<unsigned>{4} );
        vector<unsigned> out_mapping0 = parser.get_out_mapping(arg0);
        vector<unsigned> out_mapping1 = parser.get_out_mapping(arg1);
        vector<unsigned> out_mapping2 = parser.get_out_mapping(arg2);
        vector<unsigned> out_mapping3 = parser.get_out_mapping(arg3);
        CHECK( out_mapping0 == vector<unsigned>{1} );
        CHECK( out_mapping1 == vector<unsigned>{0, 2} );
        CHECK( out_mapping2 == vector<unsigned>{4} );
        CHECK( out_mapping3 == vector<unsigned>{3} );
    }
}

TEST_CASE("PetriNet") {
    SECTION("Basics") {

        // Net
        //
        //  /----\                    /--------|
        // |      V                   V        |
        // |    ( P0)               ( P3)      |
        // |      |                   |        |
        // |      V          /------->V        |
        // |     ~~~ T0     |        ~~~ T2    |
        // |      |         |         |        |
        // |      V         |         |        |
        // |    ( P1)       |         |        |
        // |      |       ( P2)       V        |
        // |      V         ^       ( P4)      |
        // |     ~~~ T1     |         |        |
        // |     | |        |         V        |
        //  \---/   \------/         ~~~ T3    |
        //                            |        |
        //                             \------/
        //

        PetriNet net{
            "P0     -> T0 -> P1",
            "P1     -> T1 -> P0, P2",
            "P2, P3 -> T2 -> P4",
            "P4     -> T3 -> P3"
        };

        cout << net.str() << endl;
    }
}
