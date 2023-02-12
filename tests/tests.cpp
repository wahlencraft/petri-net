#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "catch.hpp"

#include "place.h"
#include "transition.h"
#include "petri_net.h"
#include "verifier.h"

#include <iostream>

using namespace std;

TEST_CASE("Place/Transition") {

    SECTION("Basic Example") {
        // Transition mappings
        vector<unsigned> t0_in_map{0};
        vector<unsigned> t1_in_map{0};
        vector<unsigned> t0_out_map{1};
        vector<unsigned> t1_out_map{2};

        vector<Place*> places{
            new Place(1),
            new Place(0),
            new Place(0)
        };

        vector<Transition*> transitions{
            new Transition(t0_in_map, t0_out_map),
            new Transition(t1_in_map, t1_out_map)
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

        vector<Transition*> transitions_0{
            transitions[0]->copy(),
            transitions[1]->copy()
        };
        vector<Transition*> transitions_1{
            transitions[0]->copy(),
            transitions[1]->copy()
        };
        vector<Transition*> transitions_2{
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

    SECTION("Extract indexes") {
        PetriNetParser parser{};

        // Each PX will be given an internal index
        // In this case they appear in numerical order, so
        // PX will have index X

        // Check that we can extract that index

        string arg0 = "P0     -> T0 -> P1";
        string arg1 = "P1     -> T1 -> P0, P2";
        string arg2 = "P2, P3 -> T2 -> P4";
        string arg3 = "P4     -> T3 -> P3";
        vector<unsigned> in_mapping0 = parser.get_in_mapping(arg0);
        vector<unsigned> in_mapping1 = parser.get_in_mapping(arg1);
        vector<unsigned> in_mapping2 = parser.get_in_mapping(arg2);
        vector<unsigned> in_mapping3 = parser.get_in_mapping(arg3);
        vector<unsigned> out_mapping0 = parser.get_out_mapping(arg0);
        vector<unsigned> out_mapping1 = parser.get_out_mapping(arg1);
        vector<unsigned> out_mapping2 = parser.get_out_mapping(arg2);
        vector<unsigned> out_mapping3 = parser.get_out_mapping(arg3);

        CHECK( parser.get_place_index("P0") == 0 );
        CHECK( parser.get_place_index("P1") == 1 );
        CHECK( parser.get_place_index("P2") == 2 );
        CHECK( parser.get_place_index("P3") == 3 );
        CHECK( parser.get_place_index("P4") == 4 );

        CHECK( parser.get_transition_index("T0") == 0 );
        CHECK( parser.get_transition_index("T1") == 1 );
        CHECK( parser.get_transition_index("T2") == 2 );
        CHECK( parser.get_transition_index("T3") == 3 );
     }
}

TEST_CASE("PetriNet") {
    SECTION("Small example") {
        //   ( P0)
        //     |
        //     V
        //    ~~~ T0
        //     |
        //     V
        //   ( P1)
        //     |
        //     V
        //    ~~~ T1
        //    /  \    *
        //   V    V
        // ( P2) ( P3)
        PetriNet net{
            "P0 -> T0 -> P1",
            "P1 -> T1 -> P2, P3"
        };
        net.update_state("P0", 1);

        vector<bool> const fire_vector0{1, 0};
        vector<bool> const fire_vector1{0, 1};
        vector<bool> const fire_vector2{1, 1};

        PetriNet net0{net};

        CHECK( 0 == net0.fire(fire_vector0) );
        CHECK( net0.get_state() == PetriNetState(vector<unsigned>{0, 1, 0, 0}) );

        PetriNet net00 = net0;
        PetriNet net01 = net0;
        CHECK( 1 == net00.fire(fire_vector0) );
        CHECK( 0 == net01.fire(fire_vector1) );
        CHECK( net01.get_state() == PetriNetState(vector<unsigned>{0, 0, 1, 1}) );

        PetriNet net010 = net01;
        PetriNet net011 = net01;
        PetriNet net012 = net01;
        CHECK( 1 == net010.fire(fire_vector0) );
        CHECK( 1 == net011.fire(fire_vector1) );
        CHECK( 1 == net012.fire(fire_vector2) );

    }

    SECTION("Larger Example") {

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

        vector<unsigned> state{1, 0, 0, 1, 0};
        net.set_state(state);

        vector<bool> const fire_vector0{1, 0, 0, 0};
        CHECK(net.fire(fire_vector0) == 0);
        CHECK(net.get_state() == vector<unsigned>{0, 1, 0, 1, 0});

        vector<bool> const fire_vector1{0, 1, 0, 0};
        CHECK(net.fire(fire_vector1) == 0);
        CHECK(net.get_state() == vector<unsigned>{1, 0, 1, 1, 0});

        vector<bool> const fire_vector2{0, 0, 1, 0};
        CHECK(net.fire(fire_vector2) == 0);
        CHECK(net.get_state() == vector<unsigned>{1, 0, 0, 0, 1});

        PetriNet net_copy = net;

        // Illegal fire vector
        vector<bool> const fire_vector3{1, 1, 0, 1};
        CHECK(net.fire(fire_vector3) == 1);
        // Check that the state is unchanged
        CHECK(net.get_state() == net_copy.get_state());

        vector<bool> const fire_vector4{1, 0, 0, 1};
        CHECK(net.fire(fire_vector4) == 0);
        CHECK(net_copy.fire(fire_vector4) == 0);
        CHECK(net.get_state() == vector<unsigned>{0, 1, 0, 1, 0});
        CHECK(net_copy.get_state() == vector<unsigned>{0, 1, 0, 1, 0});

    }
}

TEST_CASE("Verifier") {
    SECTION("Basics") {

        //   ( P0)
        //     |
        //     V
        //    ~~~ T0
        //     |
        //     V
        //   ( P1)
        //     |
        //     V
        //    ~~~ T1
        //    /  \    *
        //   V    V
        // ( P2) ( P3)
        PetriNet net{
            "P0 -> T0 -> P1",
            "P1 -> T1 -> P2, P3"
        };
        vector<unsigned> state{1, 0, 0, 0};
        net.set_state(state);

        Verifier verifier{net};
        verifier.verify();

    }

    SECTION("Liveness") {
        // Net which dies
        PetriNet net_with_deadlock{  // Same net as above
            "P0 -> T0 -> P1",
            "P1 -> T1 -> P2, P3"
        };
        vector<unsigned> state1{1, 0, 0, 0};
        net_with_deadlock.set_state(state1);

        Verifier verifier1{net_with_deadlock};
        verifier1.constraints.set_require_live(true);

        CHECK_THROWS_AS(verifier1.verify(), LivenessException);

        // Live net
        //
        //   ---------
        //   |        |
        //   V        |
        // ( P0)      |
        //   |        |
        //   V        |
        //  ~~~ T0    |
        //   |        |
        //   V        |
        // ( P1)      |
        //   |        |
        //   V        |
        //  ~~~ T1    |
        //   |        |
        //   ----------
        PetriNet live_net{
            "P0 -> T0 -> P1",
            "P1 -> T1 -> P0"
        };
        vector<unsigned> state2{1, 0};
        live_net.set_state(state2);
        Verifier verifier2{live_net};
        CHECK_NOTHROW(verifier2.verify());
    }

    SECTION("Boundedness") {
        // Unbound Net
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
        PetriNet unbound_net{
            "P0     -> T0 -> P1",
            "P1     -> T1 -> P0, P2",
            "P2, P3 -> T2 -> P4",
            "P4     -> T3 -> P3"
        };

        vector<unsigned> state1{1, 0, 0, 1, 0};
        unbound_net.set_state(state1);

        Verifier verifier1{unbound_net};
        verifier1.constraints.require_live();
        verifier1.constraints.set_global_bound(5);

        CHECK_THROWS_AS(verifier1.verify(), BoundednessException);

        // Bound Net
        //
        //  /----\                    /--------|
        // |      V                   V        |
        // |    ( P0)               ( P4)      |
        // |      |                   |        |
        // |      V          /------->V        |
        // |     ~~~ T0     |        ~~~ T2    |
        // |      |       ( P2)      /|        |
        // |      V         ^       / |        |
        // |    ( P1)       |      /  |        |
        // |      |         |     /   V        |
        // |      V         |    V  ( P5)      |
        // |     ~~~ T1     | ( P3)   |        |
        // |     |^|        |   |     V        |
        //  \---/ | \------/    |    ~~~ T3    |
        //        |             |     |        |
        //        ---------------      \------/
        //
        PetriNet bound_net{
            "P0     -> T0 -> P1",
            "P1, P3 -> T1 -> P0, P2",
            "P2, P4 -> T2 -> P3, P5",
            "P5     -> T3 -> P4"
        };
        vector<unsigned> state2{1, 0, 0, 5, 1, 0};
        bound_net.set_state(state2);

        Verifier verifier2{bound_net};
        verifier2.constraints.require_live();
        verifier2.constraints.set_global_bound(5);

        CHECK_NOTHROW(verifier2.verify());
        CHECK(verifier2.get_max_bounds() == std::vector<unsigned>{1, 1, 5, 5, 1, 1});
    }
    SECTION("Reachability") {

        // Bound Net
        //
        //  /----\                    /--------|
        // |      V                   V        |
        // |    ( P0)               ( P4)      |
        // |      |                   |        |
        // |      V          /------->V        |
        // |     ~~~ T0     |        ~~~ T2    |
        // |      |       ( P2)      /|        |
        // |      V         ^       / |        |
        // |    ( P1)       |      /  |        |
        // |      |         |     /   V        |
        // |      V         |    V  ( P5)      |
        // |     ~~~ T1     | ( P3)   |        |
        // |     |^|        |   |     V        |
        //  \---/ | \------/    |    ~~~ T3    |
        //        |             |     |        |
        //        ---------------      \------/
        //
        PetriNet bound_net{
            "P0     -> T0 -> P1",
            "P1, P3 -> T1 -> P0, P2",
            "P2, P4 -> T2 -> P3, P5",
            "P5     -> T3 -> P4"
        };
        vector<unsigned> state1{1, 0, 0, 5, 1, 0};
        bound_net.set_state(state1);
        Verifier verifier{bound_net};
        verifier.constraints.require_live();

        // Check constraints afterwards
        verifier.verify();
        CHECK( verifier.reached_state(vector<unsigned>{1, 0, 5, 0, 1, 0}) );
        CHECK( verifier.reached_state(vector<unsigned>{0, 1, 5, 0, 1, 0}) );
        CHECK( verifier.reached_state(vector<unsigned>{0, 1, 2, 3, 1, 0}) );
        CHECK( !verifier.reached_state(vector<unsigned>{1, 1, 2, 3, 1, 0}) );

        // Throw error if illegal state reached
        PetriNetState const illegal_state0{vector<unsigned>{0, 0, 5, 0, 1, 0}};
        verifier.constraints.set_illegal_state(illegal_state0);
        CHECK_NOTHROW(verifier.verify());

        PetriNetState const illegal_state1{vector<unsigned>{1, 0, 2, 3, 1, 0}};
        verifier.constraints.set_illegal_state(illegal_state1);
        CHECK_THROWS_AS(verifier.verify(), ReachabilityException);
    }

    BENCHMARK_ADVANCED("6 Places, 4 Transitions")(Catch::Benchmark::Chronometer meter) {
        PetriNet bound_net{
            "G0     -> T0  -> G1",
            "G1, B1 -> T1  -> G0, B0",
            "B0, C1 -> T2  -> B1, C0",
            "C0     -> T3  -> C1"
        };
        bound_net.update_state("G0", 1);
        bound_net.update_state("B1", 5);
        bound_net.update_state("C1", 1);

        Verifier verifier{bound_net};
        verifier.constraints.require_live();
        meter.measure([&verifier] { verifier.verify(); });
    };

    BENCHMARK_ADVANCED("11 Places, 5 Transitions")(Catch::Benchmark::Chronometer meter) {
        PetriNet bound_net{
            "G0     -> T0  -> G1",
            "G1, B1 -> T1  -> G0, B0",
            "B0, C1 -> T2  -> B1, C0",
            "C0     -> T3  -> W00, W01, W02, W03, W04",

            "W00, W01, W02, W03, W04 -> T4 -> C1"
        };
        bound_net.update_state("G0", 1);
        bound_net.update_state("B1", 5);
        bound_net.update_state("C1", 1);

        Verifier verifier{bound_net};
        verifier.constraints.require_live();
        meter.measure([&verifier] { verifier.verify(); });
    };

    BENCHMARK_ADVANCED("16 Places, 10 Transitions")(Catch::Benchmark::Chronometer meter) {
        PetriNet bound_net{
            "G0     -> T0  -> G1",
            "G1, B1 -> T1  -> G0, B0",
            "B0, C1 -> T2  -> B1, C0",
            "C0     -> T3  -> W00, W01, W02, W03, W04",

            "W00    -> TW0 -> W10",
            "W01    -> TW1 -> W11",
            "W02    -> TW2 -> W12",
            "W03    -> TW3 -> W13",
            "W04    -> TW4 -> W14",

            "W10, W11, W12, W13, W14 -> T4 -> C1"
        };
        bound_net.update_state("G0", 1);
        bound_net.update_state("B1", 5);
        bound_net.update_state("C1", 1);

        Verifier verifier{bound_net};
        verifier.constraints.require_live();
        meter.measure([&verifier] { verifier.verify(); });
    };

}
