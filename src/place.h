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

    /* If there is tokens, decrease with 1 and return 0. Otherwise return 1. */
    void take_token();

    /* Increase the number of tokens with 1 */
    void give_token();

    /* After a global fire cycle. Call update() on all places. Return 0 if the 
     * fireing was legal, otherwise 1. If update() returns 1 the object is
     * invalid and should not be used agian. */
    int update();

private:
    unsigned tokens;
    unsigned input_tokens{0};
    unsigned output_tokens{0};
};

#endif  // PLACE_H_

