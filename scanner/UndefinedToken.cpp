/*
 * Undefined token returned scanner of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/28/18.
 */

#include "UndefinedToken.h"

char UndefinedToken::value() {
    return value_;
}

void UndefinedToken::print(std::ostream &stream) const {
    stream << this->type() << ": " << value_;
}