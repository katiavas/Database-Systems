//
// Created by Nikolay Yakovets on 2021-11-18.
//

#include "../include/QueryParser.h"

QueryParser::QueryParser(std::string_view buf)
        : _buf(buf.data()), _len(buf.length()) {
}

bool QueryParser::haveErrors() const {
    return !_errors.empty();
}

std::vector<QueryParser::Error> QueryParser::QueryParser::errors() const {
    return _errors;
}

std::vector<Triple> QueryParser::many() {
    std::vector<Triple> out;
    while (_idx < _len) {
        Triple t = triple();
        if (haveErrors()) {
            return out;
        }

        out.emplace_back(t);
    }

    return out;
}

Triple QueryParser::triple() {
    auto startIdx = _idx;
    Triple out;
    out.src = endpoint();

    if (eat(',')) {
        out.path = path();

        if (eat(',')) {
            out.trg = endpoint();
        } else {
            addError(_idx, "Expected: ,");
        }
    } else {
        addError(_idx, "Expected: ,");
    }

    if (!haveErrors()
        && out.src != NO_IDENTIFIER
        && out.trg != NO_IDENTIFIER) {
        addError(startIdx, "Only one of the endpoints may be bound");
    }

    return out;
}

Identifier QueryParser::endpoint() {
    if (eat('*')) {
        return NO_IDENTIFIER;
    } else {
        return identifier();
    }
}


std::vector<PathEntry> QueryParser::path() {
    std::vector<PathEntry> out;

    do {
        PathEntry entry = pathEntry();
        if (haveErrors()) {
            break;
        }

        out.emplace_back(entry);
    } while (eat('/'));

    return out;
}

/*
 * pathEntry ::= labelDir
 *             | '(' labelDir ')' '+'
 *             | '(' labelDir ( '|' labelDir )+ ')' '+'?
 */
PathEntry QueryParser::pathEntry() {
    if (!eat('(')) {
        LabelDir label = labelDir();
        return PathEntry{
                {label},
        };
    }

    PathEntry out;
    do {
        LabelDir label = labelDir();
        if (haveErrors()) {
            break;
        }

        out.labels.emplace_back(label);
    } while (eat('|'));

    if (!eat(')')) {
        addError(_idx, "Expected: )");
    }

    if (eat('+')) {
        out.kleene = true;
    } else if (out.labels.size() == 1) {
        // Should have a + when only have one identifier
        addError(_idx, "Expected +");
    }

    return out;
}

/*
 * labelDir ::= identifier dir
 * dir ::= '<' | '>'
 */
LabelDir QueryParser::labelDir() {
    LabelDir out;
    out.label = identifier();
    if (eat('>')) {
        out.reverse = 0;
    } else if (eat('<')) {
        out.reverse = 1;
    } else {
        addError(_idx, "Expecting '>' or '<");
    }

    return out;
}

/*
 * identifier ::= [0-9]+
 */
Identifier QueryParser::identifier() {
    auto startIdx = _idx;
    Identifier out = 0;
    while (_idx < _len && '0' <= _buf[_idx] && _buf[_idx] <= '9') {
        std::uint32_t v = _buf[_idx] - '0';
        _idx++;

        std::uint32_t inc = out * 10 + v;
        if (inc == NO_IDENTIFIER || inc < out) {
            addError(startIdx, "Identifier out of range");
            break;
        }

        out = inc;
    }

    if (startIdx == _idx) {
        addError(startIdx, "Expecting numeric identifier");
    }

    return out;
}
