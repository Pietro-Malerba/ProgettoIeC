/**
 * @file token.cpp
 * @brief Implements all methods defined in token.h
 * 
 * Implements functions
 * 
 * @author Pietro Malerba (S5839759) 
 * @date 08-2025
 */

#include <iostream>
#include "token.h"
#include "error.h"

/**
 * Constructor that maps a string to the corresponding NumberToken
 * @param s The string representation of the integer value
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
NumberToken::NumberToken(std::string s, int line, int column) :
    Token(line, column, TokenType::NUMBER_TOKEN) { 
    try {
        value_ = std::stoi(s);
    } catch (const std::invalid_argument& e) {
        throw InternalError(line, column, "Invalid integer value: '" + s + "'");
    }
}

/**
 * Constructor that maps a string to the corresponding BoolToken
 * @param s The string representation of the boolean value
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
BoolToken::BoolToken(bool value, int line, int column) :
    Token(line, column, TokenType::BOOL_TOKEN), value_{value} {}

/**
 * Constructor that initializes an IdToken with a string value
 * @param value The string value of the identifier
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
IdToken::IdToken(std::string value, int line, int column) :
    Token(line, column, TokenType::ID_TOKEN), value_{value} {}

/**
 * Constructor that initializes a NewLineToken
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
NewLineToken::NewLineToken(int line, int column) :
    Token(line, column, TokenType::NEWLINE_TOKEN) {}

/**
 * Constructor that initializes an EndOfFileToken
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
EndOfFileToken::EndOfFileToken(int line, int column) :
    Token(line, column, TokenType::EOF_TOKEN) {}

/**
 * Constructor that initializes an ArithmeticToken
 * @param value The integer value representing the arithmetic operator
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
ArithmeticToken::ArithmeticToken(int value, int line, int column) :
    Token(line, column, TokenType::ARITHMETIC_TOKEN), value_{value} {}

/**
 * Constructor that initializes a RelationalToken
 * @param value The integer value representing the relational operator
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
RelationalToken::RelationalToken(int value, int line, int column) :
    Token(line, column, TokenType::RELATIONAL_TOKEN), value_{value} {}

/**
 * Constructor that maps a string to the corresponding BoolOpToken
 * @param s The string representation of the bool operator
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
BoolOpToken::BoolOpToken(std::string s, int line, int column) :
    Token(line, column, TokenType::BOOLOP_TOKEN) {
    if (s == "and") value_ = AND;
    else if (s == "or") value_ = OR;
    else if (s == "not") value_ = NOT;
    else{
        throw InternalError(line, column, "Invalid Boolean Operator '" + s + "'");
    }
}

/**
 * Constructor that initializes a ReservedKeywordToken
 * @param word The string representation of the reserved keyword
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
ReservedKeywordToken::ReservedKeywordToken(std::string word, int line, int column) :
    Token(line, column, TokenType::RESERVEDKEYWORD_TOKEN) {
    if (word == "if") value_ = IF;
    else if (word == "elif") value_ = ELIF;
    else if (word == "else") value_ = ELSE;
    else if (word == "while") value_ = WHILE;
    else if (word == "continue") value_ = CONTINUE;
    else if (word == "break") value_ = BREAK;
    else if (word == "list") value_ = LIST;
    else if (word == "append") value_ = APPEND;
    else if (word == "print") value_ = PRINT;
    else {
        throw InternalError(line, column, "Invalid reserved keyword '" + word + "'");
    }
}

/**
 * Constructor that initializes an IndentationToken
 * @param isIndent Boolean indicating if indentation level increases (true) or decreases (false)
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
IndentationToken::IndentationToken(bool isIndent, int line, int column) :
    Token(line, column, TokenType::INDENTATION_TOKEN), isIndent_{isIndent} {}


/**
 * Constructor that initializes an AssignmentToken
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
AssignmentToken::AssignmentToken(int line, int column) :
    Token(line, column, TokenType::ASSIGNMENT_TOKEN) {}


/**
 * Constructor that initializes a PunctuationToken
 * @param value The integer value representing the punctuation token
 * @param line The line number where the token is found
 * @param column The column number where the token is found
 */
PunctuationToken::PunctuationToken(int value, int line, int column) :
    Token(line, column, TokenType::PUNCTUATION_TOKEN), value_{value} {}