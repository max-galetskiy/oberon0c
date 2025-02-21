/*
 * Scanner used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/15/17.
 */

#ifndef OBERON0C_SCANNER_H
#define OBERON0C_SCANNER_H


#include <filesystem>
#include <fstream>
#include <sstream>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>

#include "Token.h"
#include "LiteralToken.h"
#include "util/Logger.h"

using std::filesystem::path;
using std::ifstream;
using std::queue;
using std::streampos;
using std::string;
using std::unique_ptr;
using std::unordered_map;

class Scanner {

private:
    Logger &logger_;
    const path &path_;
    queue<unique_ptr<const Token>> tokens_;
    int lineNo_, charNo_;
    char ch_;
    bool eof_;
    unordered_map<string, TokenType> keywords_;
    ifstream file_;

    void init();
    void read();
    FilePos current();
    unique_ptr<const Token> scanToken();
    unique_ptr<const Token> scanIdent();
    unique_ptr<const Token> scanNumber();
    unique_ptr<const Token> scanString();
    void scanComment();

public:
    Scanner(const path &path, Logger &logger);
    ~Scanner();
    const Token* peek(bool = false);
    unique_ptr<const Token> next();
    void seek(const FilePos &);

    static string escape(string str);
    static string unescape(string str);

};


#endif //OBERON0C_SCANNER_H
