//
// Created by Nikolay Yakovets on 2021-11-18.
//

#ifndef QUICKSILVER_QUERY_H
#define QUICKSILVER_QUERY_H

#include <sstream>
#include <vector>

using Identifier = std::uint32_t;
static constexpr const Identifier NO_IDENTIFIER = ~Identifier(0);

struct LabelDir {
    std::uint32_t label :31;
    std::uint32_t reverse :1;

    template<typename T>
    void appendTo(T& s) const {
        s << label;
        s << (reverse ? '<' : '>');
    }
};
static_assert (sizeof (LabelDir) == 4);

struct PathEntry {
    std::vector<LabelDir> labels;
    bool kleene = false;

    template<typename T>
    void appendTo(T& s) const {
        bool parens = kleene || 1 < labels.size();
        if (parens) {
            s << '(';
        }

        for (std::size_t i = 0, max = labels.size(); i < max; i++) {
            if (i != 0) {
                s << '|';
            }

            labels[i].appendTo(s);
        }

        if (parens) {
            s << ')';
        }
        if (kleene) {
            s << '+';
        }
    }
};

struct Triple {
    Identifier src = NO_IDENTIFIER;
    std::vector<PathEntry> path;
    Identifier trg = NO_IDENTIFIER;

    template<typename T>
    void appendTo(T& s) const {
        if (src == NO_IDENTIFIER) {
            s << '*';
        } else {
            s << src;
        }
        s << ',';
        for (std::size_t i = 0, max = path.size(); i < max; i++) {
            if (i != 0) {
                s << '/';
            }

            path[i].appendTo(s);
        }
        s << ',';
        if (trg == NO_IDENTIFIER) {
            s << '*';
        } else {
            s << trg;
        }
    }

    std::string toString() const {
        std::ostringstream out;
        appendTo(out);
        return out.str();
    }
};


#endif //QUICKSILVER_QUERY_H
