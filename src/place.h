#ifndef PLACE_H_
#define PLACE_H_

#include <cassert>
#include <array>
#include <vector>
#include <sstream>

class Place {
public:
    Place(unsigned initial_tokens);
    Place* copy() const;
    std::string str() const;
    int get_tokens() const;
    bool has_tokens() const;

    /* Overwrite the number of tokens */
    void set_tokens(unsigned tokens);

    /* If there is tokens, decrease with 1 and return 0. Otherwise return 1. */
    void take_token();

    /* Increase the number of tokens with 1 */
    void give_token();

    /* After a global fire cycle. Call update() on all places. Return 0 if the
     * fireing was legal, otherwise 1. If update() returns 1 the object is
     * invalid and should not be used agian, unless it is restored. */
    int update();

    /* Restore the object to it's state before the last fire cylce. */
    void restore();

private:
    unsigned tokens;
    unsigned last_tokens{0};
    unsigned input_tokens{0};
    unsigned output_tokens{0};
};

#endif  // PLACE_H_

