/*
 * Literal tokens returned by scanner of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 3/1/20.
 */

#include "LiteralToken.h"

BooleanLiteralToken::~BooleanLiteralToken() noexcept = default;

void BooleanLiteralToken::print(std::ostream &stream) const {
    stream << this->type() << ": " << (value() ? "TRUE" : "FALSE");
}

ShortLiteralToken::~ShortLiteralToken() noexcept = default;

IntLiteralToken::~IntLiteralToken() noexcept = default;

LongLiteralToken::~LongLiteralToken() noexcept = default;

FloatLiteralToken::~FloatLiteralToken() noexcept = default;

DoubleLiteralToken::~DoubleLiteralToken() noexcept = default;

StringLiteralToken::~StringLiteralToken() noexcept = default;

CharLiteralToken::~CharLiteralToken() noexcept = default;
