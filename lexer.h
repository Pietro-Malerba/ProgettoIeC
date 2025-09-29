#if !defined(LEXER_H)
#define LEXER_H

#include <fstream>
#include <iostream>
#include <vector>
#include "token.h"
#include "error.h"

/**
 * @file lexer.h
 * @brief Defines the Lexer component of the Python-Sublanguage interpreter
 * 
 * This file contains the declaration of the Lexer class, which is responsible for
 * tokenizing the input source code into a vector of tokens.
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */

/**
 * @class Lexer
 * @brief Lexical analyzer for the Python-Sublanguage interpreter
 *
 * The Lexer class is responsible for splitting the input file into
 * a token vector that can be used by the parser.
 */
class Lexer{
    public:
        // constructors
        Lexer() = delete;
        Lexer(std::ifstream& file) : file_(file) {}
        Lexer(Lexer const& l) = delete;

        // destructor
        ~Lexer() = default;

        // overload () operator to perform the lexing (the output overwrites the attribute tokens_)
        std::vector<Token*> operator()() {
            return tokenizeInputFile(file_);
        }

        // method to get the next char and update the line and column counters
        bool getChar(std::ifstream& file, char& ch);

    private:
        // method to tokenize the input file
        std::vector<Token*> tokenizeInputFile(std::ifstream& file);

        // indentation stack to keep track of indentation levels
        std::ifstream& file_;
        std::vector<int> indentStack_{0};
        std::vector<int> parStack_;
        int line_{1};
        int column_{0};
};

#endif