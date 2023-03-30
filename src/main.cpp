#include <iostream>
#include <vector>

#include "verifier.h"

using namespace std;

int main() {

    cout << "Main" << endl;

    // Make a really large net
    // Before commit f839909e263a7b910de321999c73afd638828538 this would have
    // taken practically forever to verify
    PetriNet bound_net{
        "G0     -> T0  -> G1",
        "G1, B1 -> T1  -> G0, B0",
        "B0, C1 -> T2  -> B1, C0",
        "C0     -> T3  -> W00, W01, W02, W03, W04",

        "W00    -> TW00 -> W10",
        "W01    -> TW01 -> W11",
        "W02    -> TW02 -> W12",
        "W03    -> TW03 -> W13",
        "W04    -> TW04 -> W14",

        "W10    -> TW10 -> W20",
        "W11    -> TW11 -> W21",
        "W12    -> TW12 -> W22",
        "W13    -> TW13 -> W23",
        "W14    -> TW14 -> W24",

        "W20    -> TW20 -> W30",
        "W21    -> TW21 -> W31",
        "W22    -> TW22 -> W32",
        "W23    -> TW23 -> W33",
        "W24    -> TW24 -> W34",

        "W30    -> TW30 -> W40",
        "W31    -> TW31 -> W41",
        "W32    -> TW32 -> W42",
        "W33    -> TW33 -> W43",
        "W34    -> TW34 -> W44",

        "W40    -> TW40 -> W50",
        "W41    -> TW41 -> W51",
        "W42    -> TW42 -> W52",
        "W43    -> TW43 -> W53",
        "W44    -> TW44 -> W54",

        "W50, W51, W52, W53, W54 -> T4 -> C1"
    };
    bound_net.update_state("G0", 1);
    bound_net.update_state("B0", 5);
    bound_net.update_state("C1", 1);

    Verifier verifier{bound_net};

    verifier.constraints.set_require_live();
    verifier.constraints.set_global_bound(1);
    verifier.constraints.set_bound("B0", 5);
    verifier.constraints.set_bound("B1", 5);

    verifier.verify();

    cout << "State count: " << verifier.get_state_count() << endl;
    vector<unsigned> max_bounds = verifier.get_max_bounds();
    cout << "Max bounds: ";
    for (unsigned bound : max_bounds)
        cout << bound << " ";
    cout << endl;

    return 0;
}

