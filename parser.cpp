/**
 * @file parser.cpp
 * @brief Implements the Parser component of the Python-Sublanguage interpreter
 * 
 * This file contains the implementation of the Parser class, which is responsible for
 * creating the Syntax Tree from the token vector.
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */
#include "parser.h"
#include "syntax.h"
#include "error.h"
#include <iostream>
#include <vector>

/**
 * @brief Parses the token vector and creates the Syntax Tree
 * @param tokens The vector of pointers to Token objects to be parsed
 * @return A pointer to the root of the Syntax Tree (Program object)
 */
Program* Parser::parseProgram(){
    // Run through the token vector and initialize the Program object
    std::vector<Statement*> statements = {};

    while(index_ < tokens_.size()){
        Statement* stmt = parseStatement();
        if(stmt) statements.push_back(stmt);
        else if (tokens_[index_]->getType() == TokenType::EOF_TOKEN) break;
        else index_++;
    }

    Program* program = new Program(statements);
    return program;
}

/**
 * @brief Parses a simple statement from the token vector
 * @return A pointer to the parsed SimpleStatement object
 */
Statement* Parser::parseStatement(){
    // Check for 'print', 'break' and 'continue' statements
    if (
        tokens_[index_]->getType() == TokenType::RESERVEDKEYWORD_TOKEN &&
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() == ReservedKeywordToken::PRINT
    ) {
        return parsePrintStatement();
    }
    else if (
        tokens_[index_]->getType() == TokenType::RESERVEDKEYWORD_TOKEN &&
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() == ReservedKeywordToken::BREAK
    ) {
        return parseBreakStatement();
    }
    else if (
        tokens_[index_]->getType() == TokenType::RESERVEDKEYWORD_TOKEN &&
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() == ReservedKeywordToken::CONTINUE
    ) {
        return parseContinueStatement();
    }

    // Check for ids (list append or list declaration) or else it is an assignment
    else if (tokens_[index_]->getType() == TokenType::ID_TOKEN) {
        if (
            tokens_[index_ + 1]->getType() == TokenType::PUNCTUATION_TOKEN && 
            tokens_[index_ + 1]->getIntValue() == PunctuationToken::PERIOD &&
            tokens_[index_ + 2]->getType() == TokenType::RESERVEDKEYWORD_TOKEN &&
            tokens_[index_ + 2]->getIntValue() == ReservedKeywordToken::APPEND
        ) {
            return parseListAppendStatement();
        }
        else if (
            tokens_[index_ + 1]->getType() == TokenType::ASSIGNMENT_TOKEN &&
            tokens_[index_ + 2]->getType() == TokenType::RESERVEDKEYWORD_TOKEN &&
            tokens_[index_ + 2]->getIntValue() == ReservedKeywordToken::LIST
        ) {
            return parseListDeclarationStatement();
        }
        else {
            return parseAssignmentStatement();
        }
    }

    // Check for compound statements
    else if (
        tokens_[index_]->getType() == TokenType::RESERVEDKEYWORD_TOKEN &&
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() == ReservedKeywordToken::IF
    ) {
        return parseCompoundStatement();
    }
    else if (
        tokens_[index_]->getType() == TokenType::RESERVEDKEYWORD_TOKEN &&
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() == ReservedKeywordToken::WHILE
    ) {
        return parseCompoundStatement();
    }
    return nullptr;
}

/**
 * @brief Parses an assignment statement from the token vector
 * @return A pointer to the parsed AssignmentStatement object
 */
AssignmentStatement* Parser::parseAssignmentStatement(){
    // Calls the parsing functions for the location
    Location* location = parseLocation();

    // Check for the '=' token
    if (tokens_[index_]->getType() != TokenType::ASSIGNMENT_TOKEN) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '=' in assignment statement" );
    }
    // Skip the '=' token
    index_++;

    // Calls the parsing function for the expression
    Expression* expr = parseExpression();

    // Check for the newline token
    if (
        tokens_[index_]->getType() != TokenType::NEWLINE_TOKEN &&
        tokens_[index_]->getType() != TokenType::EOF_TOKEN
    ) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected newline at the end of assignment statement" );
    }
    // Skip the newline token
    index_++;

    // Create and return the AssignmentStatement object
    return new AssignmentStatement(location, expr, index_ - 1, tokens_);
}

/**
 * @brief Parses a list declaration statement from the token vector
 * @return A pointer to the parsed ListDeclarationStatement object
 */
ListDeclarationStatement* Parser::parseListDeclarationStatement(){
    // Check for the id token
    if (tokens_[index_]->getType() != TokenType::ID_TOKEN) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected identifier in list declaration statement" );
    }

    // Define the id token
    IdToken* id = static_cast<IdToken*>(tokens_[index_]);
    index_++;

    // Check for the '=' token
    if (tokens_[index_]->getType() != TokenType::ASSIGNMENT_TOKEN) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '=' in list declaration statement" );
    }
    // Skip the '=' token
    index_++;

    // Check for the 'list' token
    if (tokens_[index_]->getType() != TokenType::RESERVEDKEYWORD_TOKEN || 
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() != ReservedKeywordToken::LIST) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'list' in list declaration statement" );
    }
    // Skip the list token
    index_++;

    // Check for the '(' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::LPAR) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '(' in list declaration statement" );
    }
    // Skip the '(' token
    index_++;

    // Check for the ')' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::RPAR) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected ')' in list declaration statement" );
    }
    // Skip the ')' token
    index_++;

    // Check for the newline token
    if (
        tokens_[index_]->getType() != TokenType::NEWLINE_TOKEN &&
        tokens_[index_]->getType() != TokenType::EOF_TOKEN
    ) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected newline at the end of list declaration statement" );
    }
    // Skip the newline token
    index_++;

    // Create and return the ListDeclarationStatement object
    return new ListDeclarationStatement(id, index_ - 1, tokens_);
}

/**
 * @brief Parses a list append statement from the token vector
 * @return A pointer to the parsed ListAppendStatement object
 */
ListAppendStatement* Parser::parseListAppendStatement(){
    // Check for the id token
    if (tokens_[index_]->getType() != TokenType::ID_TOKEN) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected identifier in list append statement" );
    }
    // Define the id token
    IdToken* id = static_cast<IdToken*>(tokens_[index_]);
    index_++;


    // Check for the '.' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN ||
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::PERIOD) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '.' in list append statement" );
    }
    // Skip the period token
    index_++;

    // Check for the 'append' token
    if (tokens_[index_]->getType() != TokenType::RESERVEDKEYWORD_TOKEN ||
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() != ReservedKeywordToken::APPEND) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'append' in list append statement" );
    }
    // Skip the append token
    index_++;

    // Check for the '(' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::LPAR) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '(' in list append statement" );
    }
    // Skip the '(' token
    index_++;

    // Calls the parsing function for the expression
    Expression* expr = parseExpression();

    // Check for the ')' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::RPAR) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected ')' in list append statement" );
    }
    // Skip the ')' token
    index_++;

    // Check for the newline token
    if (tokens_[index_]->getType() != TokenType::NEWLINE_TOKEN &&
        tokens_[index_]->getType() != TokenType::EOF_TOKEN
    ) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected newline at the end of list append statement" );
    }
    // Skip the newline token
    index_++;

    // Create and return the ListAppendStatement object
    return new ListAppendStatement(id, expr, index_ - 1, tokens_);
}

/**
 * @brief Parses a break statement from the token vector
 * @return A pointer to the parsed BreakStatement object
 */
BreakStatement* Parser::parseBreakStatement(){
    // Check for the 'break' token
    if (tokens_[index_]->getType() != TokenType::RESERVEDKEYWORD_TOKEN ||
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() != ReservedKeywordToken::BREAK) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'break' in break statement" );
    }
    // Skip the 'break' token
    index_++;

    // Check for the newline token
    if (
        tokens_[index_]->getType() != TokenType::NEWLINE_TOKEN &&
        tokens_[index_]->getType() != TokenType::EOF_TOKEN
    ) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected newline at the end of break statement" );
    }
    // Skip the newline token
    index_++;

    // Create and return the BreakStatement object
    return new BreakStatement(index_ - 1, tokens_);
}

/**
 * @brief Parses a continue statement from the token vector
 * @return A pointer to the parsed ContinueStatement object
 */
ContinueStatement* Parser::parseContinueStatement(){
    // Check for the 'continue' token
    if (tokens_[index_]->getType() != TokenType::RESERVEDKEYWORD_TOKEN ||
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() != ReservedKeywordToken::CONTINUE) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'continue' in continue statement" );
    }
    // Skip the 'continue' token
    index_++;

    // Check for the newline token
    if (
        tokens_[index_]->getType() != TokenType::NEWLINE_TOKEN &&
        tokens_[index_]->getType() != TokenType::EOF_TOKEN
    ) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected newline at the end of continue statement" );
    }
    // Skip the newline token
    index_++;

    // Create and return the ContinueStatement object
    return new ContinueStatement(index_ - 1, tokens_);
}

/**
 * @brief Parses a print statement from the token vector
 * @return A pointer to the parsed PrintStatement object
 */
PrintStatement* Parser::parsePrintStatement(){
    // Check for the 'print' token
    if (tokens_[index_]->getType() != TokenType::RESERVEDKEYWORD_TOKEN ||
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() != ReservedKeywordToken::PRINT) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'print' in print statement" );
    }
    // Skip the 'print' token
    index_++;

    // Check for the '(' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::LPAR) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '(' in print statement" );
    }
    // Skip the '(' token
    index_++;

    // Calls the parsing function for the expression
    Expression* expr = parseExpression();

    // Check for the ')' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::RPAR) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected ')' in print statement" );
    }
    // Skip the ')' token
    index_++;

    // Check for the newline token
    if (
        tokens_[index_]->getType() != TokenType::NEWLINE_TOKEN &&
        tokens_[index_]->getType() != TokenType::EOF_TOKEN
    ) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected newline at the end of print statement" );
    }
    // Skip the newline token
    index_++;

    // Create and return the PrintStatement object
    return new PrintStatement(expr, index_ - 1, tokens_);
}

/**
 * @brief Parses a compound statement from the token vector
 * @return A pointer to the parsed CompoundStatement object
 */
CompoundStatement* Parser::parseCompoundStatement(){
    // Compound statements can be 'if' or 'while'
    StatementType StatementType;
    if (tokens_[index_]->getType() != TokenType::RESERVEDKEYWORD_TOKEN || 
        (
            tokens_[index_]->getIntValue() != ReservedKeywordToken::IF && 
            tokens_[index_]->getIntValue() != ReservedKeywordToken::WHILE
        )
    ) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'if' or 'while' in compound statement" );
    }
    if (tokens_[index_]->getIntValue() == ReservedKeywordToken::IF) StatementType = StatementType::IF_STMT;
    else if (tokens_[index_]->getIntValue() == ReservedKeywordToken::WHILE) StatementType = StatementType::WHILE_STMT;
    else {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'if' or 'while' in compound statement" );
    }

    // Increment the index to skip the 'if' or 'while' token
    index_++;

    // Calls the parsing function for the expression
    Expression* expr = parseExpression();

    // Check for the ':' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::COL) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected ':' in compound statement" );
    }
    // Skip the ':' token
    index_++;

    // Initialize the vector of blocks
    std::vector<Block*> blocks;

    // Calls the parsing function for the block
    Block* block = parseBlock();
    if (block) blocks.push_back(block);
    else {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected block in compound statement" );
    }

    // In case of 'if' statements, check for 'elif' and 'else' blocks
    if (StatementType == StatementType::IF_STMT) {
        // Check for 'elif' blocks
        while (tokens_[index_]->getType() == TokenType::RESERVEDKEYWORD_TOKEN && 
               tokens_[index_]->getIntValue() == ReservedKeywordToken::ELIF) {
            ElifBlock* elifBlock = parseElifBlock();
            if (elifBlock) blocks.push_back(elifBlock);
            else {
                throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected elif block in compound statement" );
            }
        }

        // Check for 'else' block
        if (tokens_[index_]->getType() == TokenType::RESERVEDKEYWORD_TOKEN && 
            tokens_[index_]->getIntValue() == ReservedKeywordToken::ELSE) {
            ElseBlock* elseBlock = parseElseBlock();
            if (elseBlock) blocks.push_back(elseBlock);
            else {
                throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected else block in compound statement" );
            }
        }
    }

    // Create and return the CompoundStatement object
    return new CompoundStatement(StatementType, expr, blocks, index_ - 1, tokens_);
}

/**
 * @brief Parses a block from the token vector
 * @return A pointer to the parsed Block object
 */
Block* Parser::parseBlock(){
    // Check for the newline token
    if (tokens_[index_]->getType() != TokenType::NEWLINE_TOKEN) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected newline in block" );
    }
    // Skip the newline token
    index_++;

    // Check for the indentation token
    if (tokens_[index_]->getType() != TokenType::INDENTATION_TOKEN ||
        !static_cast<IndentationToken*>(tokens_[index_])->getBoolValue()) {
        throw IndentationError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected indentation in block" );
    }
    // Skip the indentation token
    index_++;

    // Initialize the vector of statements
    std::vector<Statement*> statements;

    // Parse statements until the DEDENT token is found
    while (
        index_ < tokens_.size() &&
        (
            tokens_[index_]->getType() != TokenType::INDENTATION_TOKEN ||
            static_cast<IndentationToken*>(tokens_[index_])->getBoolValue() == true
        )
    ) {
        Statement* stmt = parseStatement();
        if (stmt) statements.push_back(stmt);
        // if the statement is null and the token is not EOF, increment the index to avoid infinite loops
        else if (tokens_[index_]->getType() != TokenType::EOF_TOKEN) index_++;
    }

    // Check for the dedentation token
    if (tokens_[index_]->getType() != TokenType::INDENTATION_TOKEN ||
        static_cast<IndentationToken*>(tokens_[index_])->getBoolValue()) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected dedentation in block" );
    }
    // Skip the dedentation token
    index_++;

    // Create and return the Block object
    return new SimpleBlock(statements, index_ - 1, tokens_);
}

/**
 * @brief Parses an elif block from the token vector
 * @return A pointer to the parsed ElifBlock object
 */
ElifBlock* Parser::parseElifBlock(){
    // Check for the 'elif' token
    if (tokens_[index_]->getType() != TokenType::RESERVEDKEYWORD_TOKEN ||
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() != ReservedKeywordToken::ELIF) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'elif' in elif block" );
    }
    // Skip the 'elif' token
    index_++;

    // Calls the parsing function for the expression
    Expression* expr = parseExpression();

    // Check for the ':' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::COL) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected ':' in elif block" );
    }
    // skip the ':' token
    index_++;

    // Calls the parsing function for the block
    Block* block = parseBlock();
    if (!block) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected block in elif block" );
    }

    // Create and return the ElifBlock object
    return new ElifBlock(expr, block, index_ - 1, tokens_);
}

/**
 * @brief Parses an else block from the token vector
 * @return A pointer to the parsed ElseBlock object
 */
ElseBlock* Parser::parseElseBlock(){
    // Check for the 'else' token
    if (tokens_[index_]->getType() != TokenType::RESERVEDKEYWORD_TOKEN ||
        static_cast<ReservedKeywordToken*>(tokens_[index_])->getIntValue() != ReservedKeywordToken::ELSE) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'else' in else block" );
    }
    // Skip the 'else' token
    index_++;

    // Check for the ':' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN || 
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::COL) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected ':' in else block" );
    }
    // Skip the ':' token
    index_++;

    // Calls the parsing function for the block
    Block* block = parseBlock();
    if (!block) {
        throw SyntaxError( tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected block in else block" );
    }

    // Create and return the ElseBlock object
    return new ElseBlock(block, index_ - 1, tokens_);
}

/**
 * @brief Parses an expression from the token vector
 * @return A pointer to the parsed Expression object
 */
Expression* Parser::parseExpression(){
    // Calls the Join parsing function (try and catch TypeError or other errors)
    Join* join = nullptr;
    try {
        join = parseJoin();
        if (!join) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression");
    } catch (const TypeError& e) {
        error(e);
    }
    
    // Check for 'or' expressions
    if (
        tokens_[index_]->getType() == TokenType::BOOLOP_TOKEN &&
        static_cast<BoolOpToken*>(tokens_[index_])->getIntValue() == BoolOpToken::OR
    ) {
        // Calls the OrExpr parsing function (try and catch TypeError or other errors)
        OrExpr* orExpr = nullptr;
        try{
            orExpr = parseOrExpr(join);
            if (!orExpr) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'or' expression");
        } catch (const TypeError& e) {
            error(e);
        }

        // if an OrExpr was found, return it
        return orExpr;
    }
    else {
        // if no OrExpr was found, return the Join
        return join;
    }
}

/**
 * @brief Parses an OrExpr from the token vector
 * @return A pointer to the parsed OrExpr object
 */
OrExpr* Parser::parseOrExpr(Join* left){
    // Check for the 'or' token
    if (
        tokens_[index_]->getType() != TokenType::BOOLOP_TOKEN ||
        static_cast<BoolOpToken*>(tokens_[index_])->getIntValue() != BoolOpToken::OR
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'or' in or expression");
    }
    // Skip the 'or' token
    index_++;

    // Calls the parsing function for the right Expression
    Expression* right = nullptr;
    try {
        right = parseExpression();
        if (!right) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression after 'or'");
    } catch (const TypeError e) {
        error(e);
    }

    // Create and return the OrExpr object
    return new OrExpr(left, right, index_ - 1, tokens_);
}

/**
 * @brief Parses a Join from the token vector
 * @return A pointer to the parsed Join object
 */
Join* Parser::parseJoin(){
    // Calls the parsing function for the Equality
    Equality* equality = nullptr;
    try {
        equality = parseEquality();
        if (!equality) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression");
    } catch (const TypeError& e) {
        error(e);
    }

    // Check for 'and' expressions
    if (
        tokens_[index_]->getType() == TokenType::BOOLOP_TOKEN &&
        static_cast<BoolOpToken*>(tokens_[index_])->getIntValue() == BoolOpToken::AND
    ) {
        AndExpr* andExpr = nullptr;
        try {
            andExpr = parseAndExpr(equality);
            if (!andExpr) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'and' expression");
        } catch (const TypeError& e) {
            error(e);
        }
        // if an AndExpr was found, return it
        return andExpr;
    }
    else {
        // if no AndExpr was found, return the Equality
        return equality;
    }
}

/**
 * @brief Parses an AndExpr from the token vector
 * @return A pointer to the parsed AndExpr object
 */
AndExpr* Parser::parseAndExpr(Equality* left){
    // Increment the index to skip the left expression
    index_++;

    // Check for the 'and' token
    if (
        tokens_[index_]->getType() == TokenType::BOOLOP_TOKEN &&
        static_cast<BoolOpToken*>(tokens_[index_])->getIntValue() == BoolOpToken::AND
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'and' in and expression");
    }
    // skip the 'and' token
    // index_++;

    // Calls the parsing function for the right Expression
    Equality* right = nullptr;
    try {
        right = parseEquality();
        if (!right) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression after 'and'");
    } catch (const TypeError& e) {
        error(e);
    }

    // Create and return the AndExpr object
    return new AndExpr(left, right, index_ - 1, tokens_);
}

/**
 * @brief Parses an Equality from the token vector
 * @return A pointer to the parsed Equality object
 */
Equality* Parser::parseEquality(){
    // Calls the parsing function for the Relation
    Relation* relation = nullptr;
    try {
        relation = parseRelation();
        if (!relation) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression");
    } catch (const TypeError& e) {
        error(e);
    }

    // Check for '==' or '!=' expressions
    if (
        tokens_[index_]->getType() == TokenType::RELATIONAL_TOKEN &&
        (
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() == RelationalToken::EQ ||
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() == RelationalToken::NEQ
        )
    ) {
        EqualExpr* equalExpr = nullptr;
        try {
            equalExpr = parseEqualExpr(relation);
            if (!equalExpr) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '==' or '!=' expression");
        } catch (const TypeError& e) {
            error(e);
        }

        // if an EqualExpr was found, return it
        return equalExpr;
    }
    else {
        // if no EqualExpr was found, return the Relation
        return relation;
    }
}

/**
 * @brief Parses an EqualExpr from the token vector
 * @return A pointer to the parsed EqualExpr object
 */
EqualExpr* Parser::parseEqualExpr(Relation* left){
    // Check for the '==' or '!=' token
    if (
        tokens_[index_]->getType() != TokenType::RELATIONAL_TOKEN ||
        (
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() != RelationalToken::EQ &&
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() != RelationalToken::NEQ
        )
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '==' or '!=' in equality expression");
    }
    // Define the equality token
    RelationalToken* eqToken = static_cast<RelationalToken*>(tokens_[index_]);
    index_++;

    // Calls the parsing function for the right Expression
    Relation* right = nullptr;
    try {
        right = parseRelation();
        if (!right) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression after '==' or '!='");
    } catch (const TypeError& e) {
        error(e);
    }

    // Create and return the EqualExpr object
    return new EqualExpr(left, eqToken, right, index_ - 1, tokens_);
}

/**
 * @brief Parses a Relation from the token vector
 * @return A pointer to the parsed Relation object
 */
Relation* Parser::parseRelation(){
    // Calls the parsing function for the NumExpr
    NumExpr* numExpr = nullptr;
    try {
        numExpr = parseNumExpr();
        if (!numExpr) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression");
    } catch (const TypeError& e) {
        error(e);
    }

    // Check for comparative relations
    if (
        tokens_[index_]->getType() == TokenType::RELATIONAL_TOKEN &&
        (
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() == RelationalToken::LE ||
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() == RelationalToken::GT ||
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() == RelationalToken::GE ||
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() == RelationalToken::LT
        )
    ) {
        ComparativeRelation* compRel = nullptr;
        try {
            compRel = parseComparativeRelation(numExpr);
            if (!compRel) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected comparative relation");
        } catch (const TypeError& e) {
            error(e);
        }
        return compRel;
    }
    else {
        // if no ComparativeRelation was found, return the NumExpr
        return numExpr;
    }
}

/**
 * @brief Parses a ComparativeRelation from the token vector
 * @return A pointer to the parsed ComparativeRelation object
 */
ComparativeRelation* Parser::parseComparativeRelation(NumExpr* left){
    // Check for the '<', '<=', '>', '>=' token
    if (
        tokens_[index_]->getType() != TokenType::RELATIONAL_TOKEN ||
        (
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() != RelationalToken::LT &&
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() != RelationalToken::LE &&
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() != RelationalToken::GT &&
            static_cast<RelationalToken*>(tokens_[index_])->getIntValue() != RelationalToken::GE
        )
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '<', '<=', '>', '>=' in comparative relation");
    }
    // Define the comparative token
    RelationalToken* compToken = static_cast<RelationalToken*>(tokens_[index_]);
    index_++;

    // Calls the parsing function for the right Expression
    NumExpr* right = nullptr;
    try {
        right = parseNumExpr();
        if (!right) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression after '<', '<=', '>', '>='");
    } catch (const TypeError& e) {
        error(e);
    }

    // Create and return the ComparativeRelation object
    return new ComparativeRelation(left, compToken, right, index_ - 1, tokens_);
}

/**
 * @brief Parses a NumExpr from the token vector
 * @return A pointer to the parsed NumExpr object
 */
NumExpr* Parser::parseNumExpr(){
    // Calls the parsing function for the Term
    Term* term = nullptr;
    try {
        term = parseTerm();
        if (!term) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression");
    } catch (const TypeError& e) {
        error(e);
    }

    // Check for '+' or '-' expressions
    if (
        tokens_[index_]->getType() == TokenType::ARITHMETIC_TOKEN &&
        (
            static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() == ArithmeticToken::ADD || 
            static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() == ArithmeticToken::SUB
        )
    ) {
        AritExpr* aritExpr = nullptr;
        try {
            aritExpr = parseAritExpr(term);
            if (!aritExpr) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '+' or '-' expression");
        } catch (const TypeError& e) {
            error(e);
        }

        // if an AritExpr was found, return it
        return aritExpr;
    }
    else {
        // if no AritExpr was found, return the Term
        return term;
    }
}

/**
 * @brief Parses an AritExpr from the token vector
 * @return A pointer to the parsed AritExpr object
 */
AritExpr* Parser::parseAritExpr(Term* left){
    // Check for the '+' or '-' token
    if (tokens_[index_]->getType() != TokenType::ARITHMETIC_TOKEN ||
        (
            static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() != ArithmeticToken::ADD &&
            static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() != ArithmeticToken::SUB
        )
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '+' or '-' in arithmetic expression");
    }
    // Define the arithmetic token
    ArithmeticToken* aritToken = static_cast<ArithmeticToken*>(tokens_[index_]);
    index_++;
    
    // Calls the parsing function for the right Expression
    Term* right = nullptr;
    try {
        right = parseTerm();
        if (!right) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression after '+' or '-'");
    } catch (const TypeError& e) {
        error(e);
    }

    // Create and return the AritExpr object
    return new AritExpr(left, aritToken, right, index_ - 1, tokens_);
}

/**
 * @brief Parses a Term from the token vector
 * @return A pointer to the parsed Term object
 */
Term* Parser::parseTerm(){
    // Calls the parsing function for the Unary
    Unary* unary = nullptr;
    try {
        unary = parseUnary();
        if (!unary) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression");
    } catch (const TypeError& e) {
        error(e);
    }

    // Check for '*' or '//' expressions
    if (
        tokens_[index_]->getType() == TokenType::ARITHMETIC_TOKEN && 
        (
            static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() == ArithmeticToken::MUL ||
            static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() == ArithmeticToken::DIV
        )
    ) {
        MulDivTerm* mulDivTerm = nullptr;
        try {
            mulDivTerm = parseMulDivTerm(unary);
            if (!mulDivTerm) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '*' or '/' expression");
        } catch (const TypeError& e) {
            error(e);
        }

        // if a MulDivTerm was found, return it
        return mulDivTerm;
    }
    else {
        // if no MulDivTerm was found, return the Unary
        return unary;
    }
}

/**
 * @brief Parses a MulDivTerm from the token vector
 * @return A pointer to the parsed MulDivTerm object
 */
MulDivTerm* Parser::parseMulDivTerm(Unary* left){
    // Check for the '*' or '//' token
    if (tokens_[index_]->getType() != TokenType::ARITHMETIC_TOKEN ||
        (
            static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() != ArithmeticToken::MUL &&
            static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() != ArithmeticToken::DIV
        )
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '*' or '/' in multiplication/division expression");
    }
    // Define the multiplication/division token
    ArithmeticToken* mulDivToken = static_cast<ArithmeticToken*>(tokens_[index_]);
    index_++;
    
    // Calls the parsing function for the right Expression
    Term* right = nullptr;
    try {
        right = parseTerm();
        if (!right) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression after '*' or '/'");
    } catch (const TypeError& e) {
        error(e);
    }

    // Create and return the MulDivTerm object
    return new MulDivTerm(left, mulDivToken, right, index_ - 1, tokens_);
}

/**     
 * @brief Parses a Unary from the token vector
 * @return A pointer to the parsed Unary object
 */
Unary* Parser::parseUnary(){
    // Check for NOT or '-' unary operators
    if (
        tokens_[index_]->getType() == TokenType::BOOLOP_TOKEN &&
        static_cast<BoolOpToken*>(tokens_[index_])->getIntValue() == BoolOpToken::NOT
    ) {
        NotUnary* notUnary = nullptr;
        try{
            notUnary = parseNotUnary();
            if (!notUnary) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'not' unary expression");
        } catch (const TypeError& e) {
            error(e);
        }

        // if a NotUnary was found, return it
        return notUnary;
    }
    else if (
        tokens_[index_]->getType() == TokenType::ARITHMETIC_TOKEN &&
        static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() == ArithmeticToken::SUB
    ) {
        MinusUnary* minusUnary = nullptr;
        try{
            minusUnary = parseMinusUnary();
            if (!minusUnary) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '-' unary expression");
        } catch (const TypeError& e) {
            error(e);
        }

        // if a MinusUnary was found, return it
        return minusUnary;
    }
    else {
        // If no unary operator was found, parse a Factor
        Factor* factor = nullptr;
        try {
            factor = parseFactor();
            if (!factor) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected factor");
        } catch (const TypeError& e) {
            error(e);
        }
        return factor;
    }
}

/**
 * @brief Parses a NotUnary from the token vector
 * @return A pointer to the parsed NotUnary object
 */
NotUnary* Parser::parseNotUnary(){
    // Check for the 'not' token
    if (
        tokens_[index_]->getType() != TokenType::BOOLOP_TOKEN ||
        static_cast<BoolOpToken*>(tokens_[index_])->getIntValue() != BoolOpToken::NOT
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected 'not' in not unary expression");
    }
    // Skip the 'not' token
    index_++;
    
    // Calls the parsing function for the right Expression
    Unary* expr = nullptr;
    try {
        expr = parseUnary();
        if (!expr) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression after 'not'");
    } catch (const TypeError& e) {
        error(e);
    }

    // Create and return the NotUnary object
    return new NotUnary(expr, index_ - 1, tokens_);
}

/**
 * @brief Parses a MinusUnary from the token vector
 * @return A pointer to the parsed MinusUnary object
 */
MinusUnary* Parser::parseMinusUnary(){
    // Check for the '-' token
    if (
        tokens_[index_]->getType() != TokenType::ARITHMETIC_TOKEN ||
        static_cast<ArithmeticToken*>(tokens_[index_])->getIntValue() != ArithmeticToken::SUB
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '-' in minus unary expression");
    }
    // Skip the '-' token
    index_++;

    // Calls the parsing function for the right Expression
    Unary* expr = nullptr;
    try {
        expr = parseUnary();
        if (!expr) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression after '-'");
    } catch (const TypeError& e) {
        error(e);
    }

    // Create and return the MinusUnary object
    return new MinusUnary(expr, index_ - 1, tokens_);
}

/**
 * @brief Parses a Factor from the token vector
 * @return A pointer to the parsed Factor object
 */
Factor* Parser::parseFactor(){
    // A Factor can be an ExpressionFactor, NumberFactor, BoolFactor or Location
    // Check for ExpressionFactor
    if (
        tokens_[index_]->getType() == TokenType::PUNCTUATION_TOKEN &&
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() == PunctuationToken::LPAR
    ) {
        ExpressionFactor* exprFactor = nullptr;
        try {
            exprFactor = parseExpressionFactor();
            if (!exprFactor) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression factor");
        } catch (const TypeError& e) {
            error(e);
        }
        return exprFactor;
    }

    // Check for NumberFactor
    else if (tokens_[index_]->getType() == TokenType::NUMBER_TOKEN) {
        NumberFactor* numFactor = nullptr;
        try {
            numFactor = parseNumberFactor();
            if (!numFactor) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected number factor");
        } catch (const TypeError& e) {
            error(e);
        }
        return numFactor;
    }

    // Check for BoolFactor
    else if (tokens_[index_]->getType() == TokenType::BOOL_TOKEN) {
        BoolFactor* boolFactor = nullptr;
        try {
            boolFactor = parseBoolFactor();
            if (!boolFactor) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected boolean factor");
        } catch (const TypeError& e) {
            error(e);
        }
        return boolFactor;
    }

    // Check for Location
    else if (tokens_[index_]->getType() == TokenType::ID_TOKEN) {
        Location* location = nullptr;
        try {
            location = parseLocation();
            if (!location) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected location");
        } catch (const TypeError& e) {
            error(e);
        }
        return location;
    }

    // If no Factor was found, return nullptr
    else {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected factor");
        return nullptr;
    }
}

/**
 * @brief Parses an ExpressionFactor from the token vector
 * @return A pointer to the parsed ExpressionFactor object
 */
ExpressionFactor* Parser::parseExpressionFactor(){
    // Check for the '(' token
    if (
        tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN ||
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::LPAR
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '(' in expression factor");
    }
    // Skip the '(' token
    index_++;

    // Calls the parsing function for the expression
    Expression* expr = nullptr;
    try {
        expr = parseExpression();
        if (!expr) throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression in expression factor");
    } catch (const TypeError& e) {
        error(e);
    }

    // Check for the ')' token
    if (
        tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN ||
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::RPAR
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected ')' in expression factor");
    }
    // Skip the ')' token
    index_++;

    // Create and return the ExpressionFactor object
    return new ExpressionFactor(expr, index_ - 1, tokens_);
}

/**
 * @brief Parses a NumberFactor from the token vector
 * @return A pointer to the parsed NumberFactor object
 */
NumberFactor* Parser::parseNumberFactor(){
    // Check for the 'NUM' token
    if (tokens_[index_]->getType() != TokenType::NUMBER_TOKEN) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected number in number factor");
    }
    // Define the number token
    NumberToken* numberToken = static_cast<NumberToken*>(tokens_[index_]);
    index_++;

    // Create and return the NumberFactor object
    return new NumberFactor(numberToken, index_ - 1, tokens_);
}

/**
 * @brief Parses a BoolFactor from the token vector
 * @return A pointer to the parsed BoolFactor object
 */
BoolFactor* Parser::parseBoolFactor(){
    // Check for the 'BOOL' token
    if (tokens_[index_]->getType() != TokenType::BOOL_TOKEN) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected boolean in boolean factor");
    }
    // Define the bool token
    BoolToken* boolToken = static_cast<BoolToken*>(tokens_[index_]);
    index_++;

    // Create and return the BoolFactor object
    return new BoolFactor(boolToken, index_ - 1, tokens_);
}

/**
 * @brief Parses a Location from the token vector
 * @return A pointer to the parsed Location object
 */
Location* Parser::parseLocation(){
    // Check for the 'ID' token
    if (tokens_[index_]->getType() != TokenType::ID_TOKEN) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected identifier in location");
    }
    // Define the id token
    IdToken* idToken = static_cast<IdToken*>(tokens_[index_]);
    index_++;

    // Check for ListElementLocation
    if (
        tokens_[index_]->getType() == TokenType::PUNCTUATION_TOKEN &&
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() == PunctuationToken::LBRACK
    ) {
        ListElementLocation* listElemLoc = parseListElementLocation(idToken);
        if (!listElemLoc) {
            throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected list element location");
        }
        return listElemLoc;
    }
    else {
        // If no ListElementLocation was found, return the IdToken as a simple Location
        IdLocation* location = new IdLocation(idToken, index_ - 1, tokens_);
        return location;
    }
}


/**
 * @brief Parses a ListElementLocation from the token vector
 * @return A pointer to the parsed ListElementLocation object
 */
ListElementLocation* Parser::parseListElementLocation(IdToken* idToken){
    // Check for the '[' token
    if (tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN ||
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::LBRACK
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected '[' in list element location");
    }
    // Skip the '[' token
    index_++;

    // Calls the parsing function for the expression
    Expression* expr = parseExpression();
    if (!expr) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected expression in list element location");
    }

    // Check for the ']' token
    if (
        tokens_[index_]->getType() != TokenType::PUNCTUATION_TOKEN ||
        static_cast<PunctuationToken*>(tokens_[index_])->getIntValue() != PunctuationToken::RBRACK
    ) {
        throw SyntaxError(tokens_[index_]->getLine(), tokens_[index_]->getColumn(), "Expected ']' in list element location");
    }
    // Skip the ']' token
    index_++;

    // Create and return the ListElementLocation object
    return new ListElementLocation(idToken, expr, index_ - 1, tokens_);
}