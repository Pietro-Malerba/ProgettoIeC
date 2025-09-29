/**
 * @file lexer.cpp
 * @brief Implements the Lexer component of the Python-Sublanguage interpreter
 *
 * This file contains the implementation of the Lexer class, which is responsible for
 * tokenizing the input source code into a vector of tokens.
 *
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */
#include "lexer.h"
#include "token.h"
#include "error.h"
#include <iostream>
#include <fstream>

/**
 * @brief Tokenizes the input file into a vector of tokens
 * @param file The input file stream to be tokenized
 * @return A vector of pointers to Token objects representing the tokenized input
 */
std::vector<Token*> Lexer::tokenizeInputFile(std::ifstream& file){

    std::vector<Token*> res;
    // Read the file content 1 character at a time
    char ch;
    int n_t = 0; // indentation level
    bool indent = true; // bool variable used to track if we are at the beginning of a new line

    while (getChar(file, ch)) {
        // Indentation handling

        // check for spaces and tabs at the beginning of a line
        if (
            ((ch == ' ') || (ch == '\t')) &&
            (res.empty() || (res.back()->getType() == TokenType::NEWLINE_TOKEN) && 
            indent)
        ) {
            // if we find any, we increase the indentation level counter
            n_t++;
            if (ch == '\t') n_t+=3; // tabs count as 4 spaces
            continue;
        }
        // If we find a non-space/tab character, we check the indentation level
        else if (((ch != ' ') && (ch != '\t') && (ch != '\n') && (ch != '\r')) && indent) {
            indent = false;
            if (n_t > indentStack_.back()) {
                indentStack_.push_back(n_t);
                res.push_back(new IndentationToken(true, line_, column_));
            }
            else if (n_t < indentStack_.back()) {
                // Check if the indentation level is valid (n_t must be in the stack)
                while (n_t < indentStack_.back()) {
                    indentStack_.pop_back();
                    res.push_back(new IndentationToken(false, line_, column_));
                }
                if (n_t != indentStack_.back()) {
                    throw IndentationError(line_, column_, "Invalid indentation level");
                }
            }
            n_t = 0; // reset the indentation level counter
        }
        // if we find a newline character, we reset the indentation tracking variable
        else if ((ch == '\n') || (ch == '\r')) {
            indent = true;
            n_t = 0; // reset the indentation level counter
        }

        // Check if the character is a letter (identifier or reserved keyword)
        if ((ch >= 'a' && ch <= 'z') || ((ch >= 'A' && ch <= 'Z'))) {
            // Entering an internal loop to read the full word (id or reserved keyword or boolean operator)
            std::string word;
            word += ch; // Add the first character
            while ((file.peek() >= 'a' && file.peek() <= 'z') || (file.peek() >= 'A' && file.peek() <= 'Z') || (file.peek() >= '0' && file.peek() <= '9')) {
                getChar(file, ch); // consume the next character
                word += ch; // add it to the word
            }

            // Check if the word is a reserved keyword
            if (
                word == "if" ||
                word == "elif" ||
                word == "else" ||
                word == "while" || 
                word == "continue" ||
                word == "break" ||
                word == "list" ||
                word == "append" ||
                word == "print"
            ) {
                res.push_back(new ReservedKeywordToken(word, line_, column_));
                continue;
            }

            // Check if the word is a boolean operator
            if (
                word == "and" ||
                word == "or" ||
                word == "not"
            ) {
                res.push_back(new BoolOpToken(word, line_, column_));
                continue;
            }

            // Check if the word is a boolean literal
            if (word == "True") {
                res.push_back(new BoolToken(true, line_, column_));
                continue;
            }
            else if (word == "False") {
                res.push_back(new BoolToken(false, line_, column_));
                continue;
            }

            // If the word is not a ReservedKeyword or a BoolOperator, than it is an Id
            res.push_back(new IdToken(word, line_, column_));
            continue;
        }

        // Check if the character is a digit
        if (ch >= '1' && ch <= '9') {
            std::string numStr;
            numStr += ch;

            // if the number is longer than 1 digit, we enter an internal loop to read the full number
            while(file.peek() >= '0' && file.peek() <= '9') {
                getChar(file, ch); // consume the next character
                numStr += ch; // add it to the number string
            }
            
            // create the token and add it to the vector
            res.push_back(new NumberToken(numStr, line_, column_));
            continue;
        }

        // Check if the character is a newline or carriage return
        if ((ch == '\n') || (ch == '\r')) {
            res.push_back(new NewLineToken(line_, column_));

            // Reset indentation tracking variable
            indent = true;
            continue;
        }

        // Check if the character is a zero (0)
        if (ch == '0') {
            // Check if the next character is a digit (invalid number)
            if (file.peek() >= '0' && file.peek() <= '9') {
                throw LexicalError(line_, column_, "Invalid integer value: leading zeros are not allowed");
            }
            else {
                res.push_back(new NumberToken("0", line_, column_));
                continue;
            }
        }

        // Check if the character is an assignment operator
        if (ch == '=') {
            // We need 1 character lookahead to distinguish between '=' and '=='
            if (file.peek() == '=') {
                getChar(file, ch); // consume the next character
                res.push_back(new RelationalToken(RelationalToken::EQ, line_, column_));
                continue;
            } else {
                res.push_back(new AssignmentToken(line_, column_));
                continue;
            }
        }

        // Check for occurrences of the remaining relational operators (!=, <, >, <=, >=)
        if ((ch == '!') && (file.peek() == '=')){
            getChar(file, ch); // consume the next character
            res.push_back(new RelationalToken(RelationalToken::NEQ, line_, column_));
            continue;
        }
        else if (ch == '<'){
            if (file.peek() == '=') {
                getChar(file, ch); // consume the next character
                res.push_back(new RelationalToken(RelationalToken::LE, line_, column_));
                continue;
            }
            else {
                res.push_back(new RelationalToken(RelationalToken::LT, line_, column_));
                continue;
            }
        }
        else if (ch == '>'){
            if (file.peek() == '=') {
                getChar(file, ch); // consume the next character
                res.push_back(new RelationalToken(RelationalToken::GE, line_, column_));
                continue;
            }
            else {
                res.push_back(new RelationalToken(RelationalToken::GT, line_, column_));
                continue;
            }
        }

        // Check if the character is an arithmetic operator
        if (ch == '+') {
            res.push_back(new ArithmeticToken(ArithmeticToken::ADD, line_, column_));
            continue;
        }
        else if (ch == '-') {
            res.push_back(new ArithmeticToken(ArithmeticToken::SUB, line_, column_ ));
            continue;
        }
        else if (ch == '*') {
            res.push_back(new ArithmeticToken(ArithmeticToken::MUL, line_, column_));
            continue;
        }
        else if (ch == '/') {
            if (file.peek() == '/') {
                getChar(file, ch); // consume the next character
                res.push_back(new ArithmeticToken(ArithmeticToken::DIV, line_, column_));
            }
            else {
                throw LexicalError(line_, column_, "Invalid character '/' (did you mean '//' for integer division?)");
            }
            continue;
        }

        // Check if the character is a punctuation character
        if (ch == ':') {
            res.push_back(new PunctuationToken(PunctuationToken::COL, line_, column_));
            continue;
        }
        else if (ch == '.') {
            res.push_back(new PunctuationToken(PunctuationToken::PERIOD, line_, column_));
            continue;
        }

        // Check if the character is a parenthesis
        if (ch == '(') {
            res.push_back(new PunctuationToken(PunctuationToken::LPAR, line_, column_));
            
            parStack_.push_back(1); // update the parenthesis stack
            continue;
        }
        else if (ch == ')') {
            res.push_back(new PunctuationToken(PunctuationToken::RPAR, line_, column_));

            // check for mismatched parenthesis
            if (parStack_.empty() || (parStack_.back() == 0)) {
                throw LexicalError(line_, column_, "Mismatched parenthesis");
            }

            // update the parenthesis stack
            parStack_.pop_back();
            continue;
        }

        // Check if the character is a bracket
        if (ch == '[') {
            res.push_back(new PunctuationToken(PunctuationToken::LBRACK, line_, column_));

            parStack_.push_back(0); // update the parenthesis stack
            continue;
        }
        else if (ch == ']') {
            res.push_back(new PunctuationToken(PunctuationToken::RBRACK, line_, column_));

            // check for mismatched brackets
            if ((parStack_.back() == 1) || (parStack_.empty())) {
                throw LexicalError(line_, column_, "Mismatched brackets");
            }

            // update the bracket stack
            parStack_.pop_back();
            continue;
        }


        // If the character is none of the above, it is an invalid character
        if ((ch != ' ')) { // ignore spaces
            throw LexicalError(line_, column_, "Invalid character '" + std::string(1, ch) + "'"); // convert char to string
        }
    }

    // Check for mismatched parenthesis at the end of the file
    if (!parStack_.empty()) {
        throw LexicalError(line_, column_, "Mismatched parenthesis or brackets");
    }
    
    // Check if the indentation stack is back to the initial state
    while(indentStack_.size() > 1) {
        indentStack_.pop_back();
        res.push_back(new IndentationToken(false, line_, column_));
    }

    // Add EOF token at the end of the vector
    if (file.eof()) {
        res.push_back(new EndOfFileToken(line_, column_));
    }

    return res;
}

/**
 * Updates the character, line, and column counters while reading the file
 * @param file The input file stream
 * @param ch The character to be updated
 */
bool Lexer::getChar(std::ifstream& file, char& ch){
    if(file.get(ch)){
        if(ch == '\n'){
            line_++;
            column_ = 0;
        }
        else{
            column_++;
        }
        return true;
    }
    else{
        return false;
    }
}