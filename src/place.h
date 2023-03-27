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

private:
    unsigned tokens;
};

#endif  // PLACE_H_

