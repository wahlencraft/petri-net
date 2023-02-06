#include "place.h"

#include <iostream>

using namespace std;

Place::Place(unsigned initial_tokens): tokens{initial_tokens} {}

Place* Place::copy() const {
    return new Place(get_tokens());
}

std::string Place::str() const {
    return "Place [" + std::to_string(get_tokens()) + "]";
}

void Place::set_tokens(unsigned count) {
    tokens = count;
}

int Place::get_tokens() const {
    return tokens;
}

bool Place::has_tokens() const {
    return tokens > 0;
}

/* If there is tokens, decrease with 1 and return 0. Otherwise return 1. */
void Place::take_token() {
    cout << "take_token()" << endl;
    cout << "output_tokens=" << output_tokens << endl;
    ++output_tokens;
    cout << "take_token() done" << endl;
}

/* Increase the number of tokens with 1 */
void Place::give_token() {
    cout << "give_token()" << endl;
    ++input_tokens;
}

/* After a global fire cycle. Call update() on all places. Return 0 if the 
 * fireing was legal, otherwise 1. If update() returns 1 the object is
 * invalid and should not be used agian. */
int Place::update() {
    if (tokens >= output_tokens) {
        tokens = tokens - output_tokens + input_tokens;
        input_tokens = 0;
        output_tokens = 0;
        return 0;
    } else {
        input_tokens = 0;
        output_tokens = 0;
        return 1;
    }
}
