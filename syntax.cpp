/**
 * @file syntax.cpp
 * @brief Implements the Syntax "component" of the Python-Sublanguage interpreter
 * 
 * This file contains the implementation of Syntax-related objects like Program, Statement, etc...
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */

#include "syntax.h"
#include "semantics.h"
#include "error.h"
#include <iostream>

/**
 * @brief Constructs a Statement object
 * @param position The position of the statement in the token vector
 * @param type The type of the statement (StatementType enum)
 * @param tokens The reference to the token vector (for error reporting)
 */
Statement::Statement(int position, StatementType type, std::vector<Token*> const& tokens) : 
    position_{position}, StatementType_{type}, tokens_{tokens} {
    // check if StatementType is valid
    if(type < ASSIGNMENT_STMT || type > WHILE_STMT) {
        throw InternalError(tokens_[position_]->getLine(), tokens_[position_]->getColumn(), "Invalid StatementType");
    }
}

/**
 * @brief Returns the line number of the statement
 * @return The line number of the statement
 */
int Statement::getLine() const {
    // Access the "position_"th token in the token vector and return its line
    return tokens_[position_]->getLine();
}

/**
 * @brief Returns the column number of the statement
 * @return The column number of the statement
 */
int Statement::getColumn() const {
    // Access the "position_"th token in the token vector and return its column
    return tokens_[position_]->getColumn();
}

/**
 * @brief Constructs a SimpleStatement object
 * @param StatementType The type of the simple statement (StatementType enum)
 */
AssignmentStatement::AssignmentStatement(Location* loc, Expression* expr, int position, std::vector<Token*> const& tokens) : 
    Statement(position, ASSIGNMENT_STMT, tokens), loc_{loc}, expression_{expr} {}

/**
 * @brief Constructs a ListDeclarationStatement object
 * @param id The IdToken representing the list identifier
 * @param position The position of the statement in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
ListDeclarationStatement::ListDeclarationStatement( IdToken* id, int position, std::vector<Token*> const& tokens) : 
    Statement(position, LIST_DECL_STMT, tokens), id_{id} {}

/**
 * @brief Constructs a ListAppendStatement object
 * @param id The IdToken representing the list identifier
 * @param expr The Expression representing the value to be appended to the list
 * @param position The position of the statement in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
ListAppendStatement::ListAppendStatement(IdToken* id, Expression* expr, int position, std::vector<Token*> const& tokens) : 
    Statement(position, LIST_APP_STMT, tokens), id_{id}, expr_{expr} {}

/**
 * @brief Constructs a BreakStatement object
 * @param position The position of the statement in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
BreakStatement::BreakStatement(int position, std::vector<Token*> const& tokens) : 
    Statement(position, BREAK_STMT, tokens) {}

/**
 * @brief Constructs a ContinueStatement object
 * @param position The position of the statement in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
ContinueStatement::ContinueStatement(int position, std::vector<Token*> const& tokens) : 
    Statement(position, CONTINUE_STMT, tokens) {}

/**
 * @brief Constructs a PrintStatement object
 * @param expr The Expression to be printed
 * @param position The position of the statement in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
PrintStatement::PrintStatement(Expression* expr, int position, std::vector<Token*> const& tokens) :
    Statement(position, PRINT_STMT, tokens), expr_{expr} {}

/**
 * @brief Constructs a CompoundStatement object
 * @param stype The type of the compound statement (StatementType enum)
 * @param expr The Expression representing the condition of the compound statement (if or while)
 * @param blocks The vector of Blocks contained in the compound statement
 * @param position The position of the statement in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
CompoundStatement::CompoundStatement(StatementType stype, Expression* expr, std::vector<Block*> blocks, int position, std::vector<Token*> const& tokens ) :
    Statement(position, stype, tokens), expr_{expr}, blocks_{blocks} {}

/**
 * @brief Constructs a Block object
 * @param BlockType The type of the block (BlockType enum)
 * @param stmts The vector of Statements contained in the block
 * @param position The position of the block in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Block::Block(BlockType type, int position, std::vector<Token*> const& tokens) :
    BlockType_{type}, position_{position}, tokens_{tokens} {
    // check if BlockType is valid
    if(type < SIMPLE_BLOCK || type > ELSE_BLOCK) {
        throw InternalError(tokens_[position_]->getLine(), tokens_[position_]->getColumn(), "Invalid BlockType");
    }
}

/**
 * @brief Returns the line number of the block
 * @return The line number of the block
 */
int Block::getLine() const {
    return tokens_[position_]->getLine();
}

/**
 * @brief Returns the column number of the block
 * @return The column number of the block
 */
int Block::getColumn() const {
    return tokens_[position_]->getColumn();
}

/**
 * @brief Constructs a SimpleBlock object
 * @param stmts The vector of Statements contained in the block
 * @param position The position of the block in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
SimpleBlock::SimpleBlock(std::vector<Statement*> stmts, int position, std::vector<Token*> const& tokens) :
    Block(SIMPLE_BLOCK, position, tokens), stmts_{stmts} {}

/**
 * @brief Constructs a ElifBlock object
 * @param elif The ReservedKeywordToken representing the block elif
 * @param condition The Expression representing the condition of the elif block
 * @param block The Block contained in the compound block
 * @param position The position of the compound block in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
ElifBlock::ElifBlock(Expression* condition, Block* block, int position, std::vector<Token*> const& tokens) :
    Block(ELIF_BLOCK, position, tokens), condition_{condition}, block_{block} {}

/**
 * @brief Constructs a ElseBlock object
 * @param block The Block contained in the else block
 * @param position The position of the else block in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
ElseBlock::ElseBlock(Block* block, int position, std::vector<Token*> const& tokens) :
    Block(ELSE_BLOCK, position, tokens), block_{block} {}


/**
 * @brief Constructs an Expression object
 * @param ExprType The type of the expression (ExpressionType enum)
 * @param position The position of the expression in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Expression::Expression(ExpressionType exprType, int position, std::vector<Token*> const& tokens) :
    exprType_{exprType}, position_{position}, tokens_{tokens} {}

/**
 * @brief Returns the line number of the expression
 * @return The line number of the expression
 */
int Expression::getLine() const {
    // Access the "position_"th token in the token vector and return its line
    return tokens_[position_]->getLine();
}

/**
 * @brief Returns the column number of the expression
 * @return The column number of the expression
 */
int Expression::getColumn() const {
    // Access the "position_"th token in the token vector and return its column
    return tokens_[position_]->getColumn();
}

/**
 * @brief Constructs an OrExpr object
 * @param left The left Join of the OrExpr
 * @param right The right Expression of the OrExpr
 * @param position The position of the OrExpr in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
OrExpr::OrExpr(Join* left, Expression* right, int position, std::vector<Token*> const& tokens) :
    Expression(OR_EXPR, position, tokens), left_{left}, right_{right} {}

/**
 * @brief Constructs a Join object
 * @param ExprType The type of the join (JoinType enum)
 * @param position The position of the Join in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Join::Join(int JoinType, int position, std::vector<Token*> const& tokens) :
    Expression(JOIN, position, tokens), joinType_{JoinType} {}

/**
 * @brief Constructs an AndExpr object
 * @param left The left Equality of the AndExpr
 * @param right The right Join of the AndExpr
 * @param position The position of the AndExpr in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
AndExpr::AndExpr(Equality* left, Join* right, int position, std::vector<Token*> const& tokens) :
    Join(AND_JOIN, position, tokens), left_{left}, right_{right} {}

/**
 * @brief Constructs an Equality object
 * @param EqualityType The type of the equality (EqualityType enum)
 * @param position The position of the equality in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Equality::Equality(int EqualityType, int position, std::vector<Token*> const& tokens) :
    Join(EQUALITY, position, tokens), equalityType_{EqualityType} {}

/**
 * @brief Constructs an EqualExpr object
 * @param left The left Relation of the EqualExpr
 * @param op The RelationalToken representing the equality operator (== or !=)
 * @param right The right Equality of the EqualExpr
 * @param position The position of the EqualExpr in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
EqualExpr::EqualExpr(Relation* left, RelationalToken* op, Equality* right, int position, std::vector<Token*> const& tokens) :
    Equality(COMP_EQUALITY, position, tokens), left_{left}, right_{right} {

        if (op->getIntValue() == RelationalToken::EQ) {
            EqualExprType_ = EQ_EXPR;
        } else if (op->getIntValue() == RelationalToken::NEQ) {
            EqualExprType_ = NEQ_EXPR;
        } else {
            throw InternalError( op->getLine(), op->getColumn(), "Invalid relational operator for EqualExpr" );
        }
    }

/**
 * @brief Constructs a Relation object
 * @param RelType The type of the relation (int representing the relational operator)
 * @param position The position of the relation in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Relation::Relation(RelationType RelType, int position, std::vector<Token*> const& tokens) :
    Equality(REL, position, tokens), relType_{RelType} {}

/**
 * @brief Constructs a ComparativeRelation object
 * @param left The left NumExpr of the ComparativeRelation
 * @param op The RelationalToken representing the relational operator (<, <=, >, >=
 * @param right The right NumExpr of the ComparativeRelation
 * @param position The position of the ComparativeRelation in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
ComparativeRelation::ComparativeRelation(NumExpr* left, RelationalToken* op, NumExpr* right, int position, std::vector<Token*> const& tokens) :
    Relation(COMPARATIVE_RELATION, position, tokens), left_{left}, right_{right} {
        if (op->getIntValue() == RelationalToken::LT) {
            ComparativeRelationType_ = LT_REL;
        } else if (op->getIntValue() == RelationalToken::LE) {
            ComparativeRelationType_ = LE_REL;
        } else if (op->getIntValue() == RelationalToken::GT) {
            ComparativeRelationType_ = GT_REL;
        } else if (op->getIntValue() == RelationalToken::GE) {
            ComparativeRelationType_ = GE_REL;
        } else {
            throw InternalError(op->getLine(), op->getColumn(), "Invalid relational operator for ComparativeRelation");
        }
    }

/**
 * @brief Constructs a NumExpr object
 * @param ExprType The type of the numerical expression (NumExprType enum)
 * @param position The position of the NumExpr in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
NumExpr::NumExpr(NumExprType ExprType, int position, std::vector<Token*> const& tokens) :
    Relation(NUM_EXPR, position, tokens), numExprType_{ExprType} {}

/**
 * @brief Constructs a AritExpr object
 * @param left The left Term of the AritExpr
 * @param op The ArithmeticToken representing the arithmetic operator (+ or -)
 * @param right The right NumExpr of the AritExpr
 * @param position The position of the AritExpr in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
AritExpr::AritExpr(Term* left, ArithmeticToken* op, NumExpr* right, int position, std::vector<Token*> const& tokens) :
    NumExpr(ARIT_EXPR, position, tokens), left_{left}, right_{right} {
        if (op->getIntValue() == ArithmeticToken::ADD) {
            aritExprType_ = ADD_EXPR;
        } else if (op->getIntValue() == ArithmeticToken::SUB) {
            aritExprType_ = SUB_EXPR;
        } else {
            throw InternalError(op->getLine(), op->getColumn(), "Invalid arithmetic operator for AritExpr");
        }
    }

/**
 * @brief Constructs a Term object
 * @param TermType The type of the term (int representing the term type)
 * @param position The position of the Term in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Term::Term(int TermType, int position, std::vector<Token*> const& tokens) :
    NumExpr(TERM, position, tokens), termType_{TermType} {}

/**
 * @brief Constructs a MulDivTerm object
 * @param left The left Unary of the MulDivTerm
 * @param op The ArithmeticToken representing the arithmetic operator (* or //)
 * @param right The right Term of the MulDivTerm
 * @param position The position of the MulDivTerm in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
MulDivTerm::MulDivTerm( Unary* left, ArithmeticToken* op, Term* right, int position, std::vector<Token*> const& tokens) :
    Term(MULDIV_TERM, position, tokens), left_{left}, right_{right} {
        if (op->getIntValue() == ArithmeticToken::MUL) {
            mulDivTermType_ = MUL_TERM;
        } else if (op->getIntValue() == ArithmeticToken::DIV) {
            mulDivTermType_ = DIV_TERM;
        } else {
            throw InternalError(op->getLine(), op->getColumn(), "Invalid arithmetic operator for MulDivTerm");
        }
    }

/**
 * @brief Constructs a Unary object
 * @param unaryType The type of the unary (UnaryType enum)
 * @param position The position of the Unary in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Unary::Unary(int unaryType, int position, std::vector<Token*> const& tokens) :
    Term(UNARY_TERM, position, tokens), unaryType_{unaryType} {}

/**
 * @brief Constructs a NotUnary object
 * @param unary The Unary expression following the 'not' operator
 * @param position The position of the NotUnary in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
NotUnary::NotUnary(Unary* unary, int position, std::vector<Token*> const& tokens) :
    Unary(NOT_UNARY, position, tokens), unary_{unary} {}

/**
 * @brief Constructs a MinusUnary object
 * @param unary The Unary expression following the '-' operator
 * @param position The position of the MinusUnary in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
MinusUnary::MinusUnary(Unary* unary, int position, std::vector<Token*> const& tokens) :
    Unary(MINUS_UNARY, position, tokens), unary_{unary} {}

/**
 * @brief Constructs a Factor object
 * @param factorType The type of the factor (FactorType enum)
 * @param position The position of the Factor in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Factor::Factor(FactorType factorType, int position, std::vector<Token*> const& tokens) :
    Unary(FACTOR, position, tokens), factorType_{factorType} {}

/**
 * @brief Constructs an ExpressionFactor object
 * @param expr The Expression contained in the parentheses
 * @param position The position of the ExpressionFactor in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
ExpressionFactor::ExpressionFactor(Expression* expr, int position, std::vector<Token*> const& tokens) :
    Factor(EXPR_FACTOR, position, tokens), expr_{expr} {}

/**
 * @brief Constructs a NumberFactor object
 * @param number The NumberToken representing the numeric value
 * @param position The position of the NumberFactor in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
NumberFactor::NumberFactor(NumberToken* number, int position, std::vector<Token*> const& tokens) :
    Factor(NUMBER, position, tokens), number_{number} {}

/**
 * @brief Constructs a BoolFactor object
 * @param boolean The BoolToken representing the boolean value
 * @param position The position of the BoolFactor in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
BoolFactor::BoolFactor(BoolToken* boolean, int position, std::vector<Token*> const& tokens) :
    Factor(BOOL, position, tokens), boolean_{boolean} {}

/**
 * @brief Constructs a Location object
 * @param locType The type of the location (LocationType enum)
 * @param position The position of the Location in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
Location::Location(int locType, int position, std::vector<Token*> const& tokens) :
    Factor(LOCATION, position, tokens), locType_{locType} {}

/**
 * @brief Constructs an IdLocation object
 * @param id The IdToken representing the identifier
 * @param position The position of the IdLocation in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
IdLocation::IdLocation(IdToken* id, int position, std::vector<Token*> const& tokens) :
    Location(ID, position, tokens), id_{id} {}

/**
 * @brief Constructs a ListElementLocation object
 * @param id The IdToken representing the list identifier
 * @param expr The Expression representing the index of the list element
 * @param position The position of the ListElementLocation in the token vector
 * @param tokens The reference to the token vector (for error reporting)
 */
ListElementLocation::ListElementLocation(IdToken* id, Expression* expr, int position, std::vector<Token*> const& tokens) :
    Location(LIST_ELEM, position, tokens), id_{id}, expr_{expr} {}