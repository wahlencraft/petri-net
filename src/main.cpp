#include <iostream>
#include <vector>

#include "verifier.h"

using namespace std;

int main() {

    cout << "Main" << endl;

    PetriNet bound_net{
        "P0     -> T0 -> P1",
        "P1, P3 -> T1 -> P0, P2",
        "P2, P4 -> T2 -> P3, P5",
        "P5     -> T3 -> P4"
    };
    vector<unsigned> initial_state{1, 0, 0, 5, 1, 0};
    bound_net.set_state(initial_state);

    Verifier verifier{bound_net};

    verifier.constraints.set_require_live();
    verifier.constraints.set_illegal_state(vector<unsigned>{1, 1, 5, 0, 0, 1});
    verifier.constraints.set_illegal_state(vector<unsigned>{1, 0, 5, 0, 1, 1});
    verifier.constraints.set_illegal_state(vector<unsigned>{1, 0, 3, 3, 0, 1});
    verifier.constraints.set_illegal_state(vector<unsigned>{1, 0, 5, 1, 0, 1});

    verifier.verify();
    cout << "State count: " << verifier.get_state_count() << endl;

    return 0;
}

