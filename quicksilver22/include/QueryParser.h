//
// Created by Nikolay Yakovets on 2021-11-18.
//

#ifndef QUICKSILVER_QUERYPARSER_H
#define QUICKSILVER_QUERYPARSER_H

#include <cstdint>
#include <vector>
#include <sstream>

#include "Query.h"

/*
 * many ::= triple '\r'? '\n'
 * triple ::= freeEndpoint ',' pathExpr ',' freeEndpoint
 *          | boundEndpoint ',' pathExpr ',' endpoint
 *          | freeEndpoint ',' pathExpr ',' boundEndpoint
 * freeEndpoint ::= '*'
 * boundEndpoint ::= identifier
 * pathExpr :: = pathEntry ('/' pathEntry)*
 * pathEntry ::= labelDir
 *             | '(' labelDir ')' '+'
 *             | '(' labelDir ( '|' labelDir )+ ')' '+'?
 * labelDir ::= identifier dir
 * dir ::= '<' | '>'
 * identifier ::= [0-9]+
 */

class QueryParser {
public:

    struct Error {
        std::uint32_t idx;
        const char* msg;
    };

private:
    const char* _buf;
    std::uint32_t _len;
    std::uint32_t _line = 0;
    std::uint32_t _idx = 0;

    std::vector<Error> _errors;

    void addError(std::uint32_t idx, const char* msg) {
        _errors.emplace_back(Error { idx, msg });
    }
    bool eat(char c) {
        if (_idx < _len && _buf[_idx] == c) {
            _idx++;
            return true;
        }

        return false;
    }

public:
    QueryParser(std::string_view buf);

    bool haveErrors() const;
    std::vector<Error> errors() const;

    std::vector<Triple> many();

    Triple triple();

    Identifier endpoint() ;

    /*
     * pathExpr :: = pathEntry ('/' pathEntry)*
     */
    std::vector<PathEntry> path() ;

    /*
     * pathEntry ::= labelDir
     *             | '(' labelDir ')' '+'
     *             | '(' labelDir ( '|' labelDir )+ ')' '+'?
     */
    PathEntry pathEntry() ;

    /*
     * labelDir ::= identifier dir
     * dir ::= '<' | '>'
     */
    LabelDir labelDir() ;

    /*
     * identifier ::= [0-9]+
     */
    Identifier identifier() ;
};

#endif //QUICKSILVER_QUERYPARSER_H
