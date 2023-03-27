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
    --tokens;
}

/* Increase the number of tokens with 1 */
void Place::give_token() {
    ++tokens;
}

