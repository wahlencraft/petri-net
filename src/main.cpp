#include <iostream>
#include <vector>

#include "verifier.h"

using namespace std;

int main() {

    cout << "Main" << endl;

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
    cout << "Max bounds: " << verifier.get_max_bounds_as_string() << endl;

    return 0;
}

