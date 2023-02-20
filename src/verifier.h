#ifndef VERIFIER_H_
#define VERIFIER_H_

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "petri_net.h"
#include "verification_exceptions.h"
#include "constraints.h"
#include "thread_pool.h"

class Verifier {
public:
    Verifier(PetriNet const &petri_net, unsigned max_threads=0);
    ~Verifier();

    void verify();
    std::vector<unsigned> const& get_max_bounds() const;
    std::string get_max_bounds_as_string() const;

    /* Chech whether a state has been reached before.
     * If not add it to previous_states
     *
     * Thread safe. */
    bool reached_state(PetriNetState const &state) ;

    size_t get_state_count() const;

    Constraints constraints;
private:
    void verify(PetriNet const &initial_net, unsigned current_depth);
    void check_boundness(PetriNet const &net);
    void check_reachability(PetriNet const &net);
    void check_liveness(PetriNet const &net, unsigned live_transactions, unsigned current_depth);

    std::unordered_set<PetriNetState, PetriNetStateHash> previous_states{};
    PetriNet const initial_net;
    std::vector<unsigned> token_max{};

    ThreadPool thread_pool;
    std::mutex mtx{};
    std::condition_variable cv{};

    std::atomic<bool> abort = false;
    std::exception_ptr exception = nullptr;
};

#endif  // VERIFIER_H_

