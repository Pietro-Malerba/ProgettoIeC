#if !defined(TOKEN_H)
#define TOKEN_H
#include <string>
#include "error.h"

/**
 * @file token.h
 * @brief Defines the Token-Object and all the derived classes
 * 
 * Defines Token
 * 
 * @author Pietro Malerba (S5839759) 
 * @date 08-2025
 */

/**
 * @enum Type
 * @brief Enumeration of token types
 */
enum TokenType {
    NUMBER_TOKEN,
    BOOL_TOKEN,
    ID_TOKEN,
    NEWLINE_TOKEN,
    EOF_TOKEN,
    ARITHMETIC_TOKEN,
    RELATIONAL_TOKEN,
    BOOLOP_TOKEN,
    RESERVEDKEYWORD_TOKEN,
    INDENTATION_TOKEN,
    ASSIGNMENT_TOKEN,
    PUNCTUATION_TOKEN
};

/**
 * @class Token
 * @brief Represents the main unit of the output of the Lexer
 * 
 * This class encapsulates the input file into objects which will be used by the Parser
 */
class Token{
    public:
        // constructors
        Token() = delete;
        Token(int line, int column, TokenType type) : line_{line}, column_{column}, type_{type} {}
        Token(const Token& t) = default;

        // virtual destructor (to allow proper cleanup of derived classes)
        virtual ~Token() = default;

        // methods
        int getLine() const { return line_; }
        int getColumn() const { return column_; }
        TokenType getType() const { return type_; }

        // pure virtual method to get the tag of the token
        virtual int getIntValue() const { throw InternalError( line_, column_, "This token does not have an integer value" ); }
        virtual bool getBoolValue() const { throw InternalError( line_, column_, "This token does not have a boolean value" ); }
        virtual std::string getStringValue() const { throw InternalError( line_, column_, "This token does not have a string value" ); }

    private:
        int line_;
        int column_;
        TokenType type_;
};

/**
 * @class NumberToken
 * @brief Tokenized version of integers
 * 
 * The lexer converts integers into numbertokens
 */
class NumberToken : public Token{
    public:
        // constructors
        NumberToken() = delete;
        NumberToken(std::string s, int line, int column); // defined in token.cpp
        NumberToken(const NumberToken& n) = default;

        // destructors
        ~NumberToken() = default;

        // methods
        int getIntValue() const override { return value_; }

    private:
        int value_;
};

/**
 * @class BoolToken
 * @brief Tokenized version of boolean values
 * 
 * The lexer converts boolean values into booltokens
 */
class BoolToken : public Token{
    public:
        // constructors
        BoolToken() = delete;
        BoolToken(bool value, int line, int column); // defined in token.cpp
        BoolToken(const BoolToken& b) = default;

        // destructors
        ~BoolToken() = default;

        // value method
        bool getBoolValue() const override { return value_; }

    private:
        bool value_;
};

/**
 * @class IdToken
 * @brief Tokenized version of labels and ids
 * 
 * The lexer converts strings into idtoken
 */
class IdToken : public Token{
    public:
        // constructors
        IdToken() = delete;
        IdToken(std::string value, int line, int column); // defined in token.cpp
        IdToken(const IdToken& id) = default;

        // destructors
        ~IdToken() = default;

        // methods
        std::string getStringValue() const override { return value_; }

    private:
        std::string value_;
};

/**
 * @class NewLineToken
 * @brief Tokenized version of '\n'
 * 
 * The lexer converts '\n' char into this Token
 */
class NewLineToken : public Token{
    public:
        // constructors
        NewLineToken() = delete;
        NewLineToken(int line, int column); // defined in token.cpp
        NewLineToken(const NewLineToken& nl) = default;

        // destructors
        ~NewLineToken() = default;
};

/**
 * @class EndOfFileToken
 * @brief Tokenized version of the EOF tag
 * 
 * The lexer converts the end of a file into this Token
 */
class EndOfFileToken : public Token{
    public:
        // constructors
        EndOfFileToken() = delete;
        EndOfFileToken(int line, int column); // defined in token.cpp
        EndOfFileToken(const EndOfFileToken& eof) = delete;

        // destructors
        ~EndOfFileToken() = default;
};

/**
 * @class ArithmeticToken
 * @brief Tokenized version of arithmetic operators
 * 
 * The lexer converts arithmetic operators into this Token
 */
class ArithmeticToken : public Token{
    public:
        static const int ADD = 0; // addition
        static const int SUB = 1; // subtraction
        static const int MUL = 2; // multiplication
        static const int DIV = 3; // division

        // constructors
        ArithmeticToken() = delete;
        ArithmeticToken(int value, int line, int column); // defined in token.cpp
        ArithmeticToken(const ArithmeticToken& arith) = default;

        // destructors
        ~ArithmeticToken() = default;

        // methods
        int getIntValue() const override { return value_; }

    private:
        int value_;
};

/**
 * @class RelationalToken
 * @brief Tokenized version of relational operators
 *
 * The lexer converts relational operators into this Token
 */
class RelationalToken : public Token{
    public:
        static const int EQ = 0; // equal
        static const int LT = 1; // less than
        static const int GT = 2; // greater than
        static const int LE = 3; // less than or equal
        static const int GE = 4; // greater than or equal
        static const int NEQ = 5; // not equal

        // constructors
        RelationalToken() = delete;
        RelationalToken(int value, int line, int column); // defined in token.cpp
        RelationalToken(const RelationalToken& rel) = default;

        // destructors
        ~RelationalToken() = default;

        // methods
        int getIntValue() const override { return value_; }

    private:
        int value_;
};

/**
 * @class BoolOpToken
 * @brief Tokenized version of boolean operators
 * 
 * The lexer converts boolean operators into this Token
 */
class BoolOpToken : public Token{
    public:
        static const int AND = 0; // logical and
        static const int OR = 1;  // logical or
        static const int NOT = 2; // logical not

        // constructors
        BoolOpToken() = delete;
        BoolOpToken(std::string s, int line, int column); // defined in token.cpp
        BoolOpToken(const BoolOpToken& b) = default;

        // destructors
        ~BoolOpToken() = default;

        // methods
        int getIntValue() const override { return value_; }

    private:
        int value_;
};

/**
 * @class ReservedKeywordToken
 * @brief Tokenized version of reserved keywords
 *
 * The lexer converts reserved keywords into this Token
 */
class ReservedKeywordToken : public Token{
    public:
        static const int IF = 0;        // "if" keyword
        static const int ELIF = 1;      // "elif" keyword
        static const int ELSE = 2;      // "else" keyword
        static const int WHILE = 3;     // "while" keyword
        static const int CONTINUE = 4;  // "continue" keyword
        static const int BREAK = 5;     // "break" keyword
        static const int LIST = 6;      // "list" keyword
        static const int APPEND = 7;    // "append" keyword
        static const int PRINT = 8;     // "print" keyword

        // constructors
        ReservedKeywordToken() = delete;
        ReservedKeywordToken(std::string word, int line, int column); // defined in token.cpp
        ReservedKeywordToken(const ReservedKeywordToken& r) = default;

        // destructors
        ~ReservedKeywordToken() = default;

        // methods
        int getIntValue() const override { return value_; }

    private:
        int value_;
};

/**
 * @class IndentationToken
 * @brief Tokenized version of indentation levels
 *
 * The lexer converts indentation levels into this Token
 */
class IndentationToken : public Token{
    public:
        // constructors
        IndentationToken() = delete;
        IndentationToken(bool isIndent, int line, int column); // defined in token.cpp
        IndentationToken(const IndentationToken& ind) = default;

        // destructors
        ~IndentationToken() = default;

        // methods
        bool getBoolValue() const override { return isIndent_; }
    
    private:
        bool isIndent_;
};

/**
 * @class AssignmentToken
 * @brief Tokenized version of the assignment operator
 * 
 * The lexer converts the '=' char into this Token
 */
class AssignmentToken : public Token{
    public:
        // constructor
        AssignmentToken() = delete;
        AssignmentToken(int line, int column); // defined in token.cpp
        AssignmentToken(const AssignmentToken& a) = default;

        // destructor
        ~AssignmentToken() = default;
};

/**
 * @class PunctuationToken
 * @brief Tokenized version of punctuation characters
 * 
 * The lexer converts punctuation characters into this Token
 */
class PunctuationToken : public Token{
    public:
        static const int COL = 0; // colon char ":"
        static const int LPAR = 1; // left parenthesis "("
        static const int RPAR = 2; // right parenthesis ")"
        static const int PERIOD = 3; // period char "."
        static const int LBRACK = 4; // left bracket "["
        static const int RBRACK = 5; // right bracket "]"
        

        // constructors
        PunctuationToken() = delete;
        PunctuationToken(int value, int line, int column); // defined in token.cpp
        PunctuationToken(const PunctuationToken& p) = default;

        // destructor
        ~PunctuationToken() = default;

        // token tag
        int getIntValue() const override { return value_; }

    private:
        int value_;
};

#endif