#if !defined(SYNTAX_H)
#define SYNTAX_H

#include <vector>
#include "token.h"
#include "semantics.h"
#include "error.h"
#include "types.h"

// forward declarations
class Statement;
class Expression;
class Location;
class Join;
class Equality;
class Term;
class Unary;
class NumExpr;
class Relation;
class Block;
class CompoundBlock;
class RelationalToken;
class ArithmeticToken;

/**
 * @file syntax.h
 * @brief Defines the Syntax "component" of the Python-Sublanguage interpreter
 * 
 * This file contains the declaration of Syntax-related objects like Program, Statement, etc...
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */


/**
 * @class Program
 * @brief Represents a program in the Python-Sublanguage interpreter
 */
class Program{
    public:
        // constructors
        Program() = default;
        Program( std::vector<Statement*> stmts) : stmts_{stmts} {}
        Program(Program const& p) = delete;

        // destructor
        ~Program() = default;

        // methods
        std::vector<Statement*> getStatements() const { return stmts_; }

    private:
        std::vector<Statement*> stmts_;
};

/**
 * 
 */
enum StatementType {
    // simple statements
    ASSIGNMENT_STMT,
    LIST_DECL_STMT,
    LIST_APP_STMT,
    BREAK_STMT,
    CONTINUE_STMT,
    PRINT_STMT,
    // compound statements
    IF_STMT,
    WHILE_STMT
};

/**
 * @class Statement
 * @brief Represents a statement in the Python-Sublanguage interpreter
 */
class Statement{
    public:

        // constructors
        Statement() = delete;
        Statement(int position, StatementType type, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Statement(Statement const& s) = delete;

        // destructor (virtual)
        virtual ~Statement() = default;


        // setter and getter for StatementType_
        int getStatementType() const { return StatementType_; }

        // methods to get line and column
        int getLine() const;
        int getColumn() const;

    private:
        int StatementType_;
        int position_; // position in the token vector (for error reporting)
        std::vector<Token*> tokens_; // reference to the token vector (for error reporting)
};

/**
 * @class AssignmentStatement
 * @brief Represents an assignment statement in the Python-Sublanguage interpreter
 */
class AssignmentStatement : public Statement{
    public:
        // constructors
        AssignmentStatement() = delete;
        AssignmentStatement(Location* loc, Expression* expr, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        AssignmentStatement(AssignmentStatement const& as) = delete;

        // destructor
        ~AssignmentStatement() = default;

        // methods
        Location* getLocation() const { return loc_; }
        Expression* getExpression() const { return expression_; }

    private:
        Location* loc_;
        Expression* expression_;
};

/**
 * @class ListDeclarationStatement
 * @brief Represents a list declaration statement in the Python-Sublanguage interpreter
 */
class ListDeclarationStatement : public Statement{
    public:
        // constructors
        ListDeclarationStatement() = delete;
        ListDeclarationStatement( IdToken* id, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        ListDeclarationStatement(ListDeclarationStatement const& lds) = delete;

        // destructor
        ~ListDeclarationStatement() = default;

        // methods
        std::string getId() const { return id_->getStringValue(); }

    private:
        IdToken* id_;
};

/**
 * @class ListAppendStatement
 * @brief Represents a list append statement in the Python-Sublanguage interpreter
 */
class ListAppendStatement : public Statement{
    public:
        // constructors
        ListAppendStatement() = delete;
        ListAppendStatement(IdToken* id, Expression* expr, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        ListAppendStatement(ListAppendStatement const& las) = delete;

        // destructor
        ~ListAppendStatement() = default;

        // methods
        std::string getId() const { return id_->getStringValue(); }
        Expression* getExpression() const { return expr_; }

    private:
        IdToken* id_;
        Expression* expr_;
};

/**
 * @class BreakStatement
 * @brief Represents a break statement in the Python-Sublanguage interpreter
 */
class BreakStatement : public Statement{
    public:
        // constructors
        BreakStatement() = delete;
        BreakStatement(int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        BreakStatement(BreakStatement const& bs) = delete;

        // destructor
        ~BreakStatement() = default;
};

/**
 * @class ContinueStatement
 * @brief Represents a continue statement in the Python-Sublanguage interpreter
 */
class ContinueStatement : public Statement{
    public:
        // constructors
        ContinueStatement() = delete;
        ContinueStatement(int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        ContinueStatement(ContinueStatement const& cs) = delete;

        // destructor
        ~ContinueStatement() = default;
};

/**
 * @class PrintStatement
 * @brief Represents a print statement in the Python-Sublanguage interpreter
 */
class PrintStatement : public Statement{
    public:
        // constructors
        PrintStatement() = delete;
        PrintStatement(Expression* expr, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        PrintStatement(PrintStatement const& ps) = delete;

        // destructor
        ~PrintStatement() = default;

        // methods
        Expression* getExpression() const { return expr_; }

    private:
        Expression* expr_;
};

/**
 * @class CompoundStatement
 * @brief Represents a compound statement in the Python-Sublanguage interpreter
 */
class CompoundStatement : public Statement{
    public:
        // constructors
        CompoundStatement() = delete;
        CompoundStatement(StatementType stype, Expression* expr, std::vector<Block*> blocks, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        CompoundStatement(CompoundStatement const& cs) = delete;

        // destructor
        ~CompoundStatement() = default;

        // methods
        Expression* getExpression() const { return expr_; }
        std::vector<Block*> getBlocks() const { return blocks_; }

    private:
        Expression* expr_;
        std::vector<Block*> blocks_;
};

/**
 * @enum BlockType
 * @brief Represents the different types of blocks in the Python-Sublanguage interpreter
 */
enum BlockType {
    SIMPLE_BLOCK,
    IF_BLOCK,
    ELIF_BLOCK,
    ELSE_BLOCK
};

/**
 * @class Block
 * @brief Represents a block of statements in the Python-Sublanguage interpreter
 */
class Block{
    public:
        // constructors
        Block() = delete;
        Block(BlockType type, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Block(Block const& b) = default;

        // destructor
        virtual ~Block() = default;

        // methods
        int getLine() const ;
        int getColumn() const;
        BlockType getBlockType() const { return BlockType_; }

    private:
        BlockType BlockType_;
        int position_; // position in the token vector (for error reporting)
        std::vector<Token*> tokens_; // reference to the token vector (for error reporting)
};

/**
 * @class SimpleBlock
 * @brief Represents a simple block of statements in the Python-Sublanguage interpreter
 */
class SimpleBlock : public Block{
    public:
        // constructors
        SimpleBlock() = delete;
        SimpleBlock(std::vector<Statement*> stmts, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        SimpleBlock(SimpleBlock const& sb) = default;

        // destructor
        ~SimpleBlock() = default;

        // methods
        std::vector<Statement*> getStatements() const { return stmts_; }

    private:
        std::vector<Statement*> stmts_;
};

/**
 * @class ElifBlock
 * @brief Represents an elif block in the Python-Sublanguage interpreter
 */
class ElifBlock : public Block{
    public:
        // constructors
        ElifBlock() = delete;
        ElifBlock(Expression* condition, Block* block, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        ElifBlock(ElifBlock const& eb) = default;

        // destructor
        ~ElifBlock() = default;

        // methods
        Expression* getCondition() const { return condition_; }
        Block* getBlock() const { return block_; }

    private:
        Expression* condition_;
        Block* block_;
};

/**
 * @class ElseBlock
 * @brief Represents an else block in the Python-Sublanguage interpreter
 */
class ElseBlock : public Block {
    public:
        // constructors
        ElseBlock() = delete;
        ElseBlock(Block* block, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        ElseBlock(ElseBlock const& eb) = default;

        // destructor
        ~ElseBlock() = default;

        // methods
        Block* getBlock() const { return block_; }

    private:
        Block* block_;
};

/**
 * @enum ExpressionType
 * @brief Represents the different types of expressions in the Python-Sublanguage interpreter
 */
enum ExpressionType {
    OR_EXPR,
    JOIN
};

/**
 * @class Expression
 * @brief Represents an expression in the Python-Sublanguage interpreter
 */
class Expression{
    public:
        // constructors
        Expression() = delete;
        Expression(ExpressionType ExprType, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Expression(Expression const& e) = delete;

        // destructor
        virtual ~Expression() = default;

        // methods
        ExpressionType getExprType() const { return exprType_; }
        int getLine() const;
        int getColumn() const;
        void setDataType(Types type) { dataType_ = type; }

    private:
        Types dataType_{Types::TYPE_UNDEFINED}; // Type of the expression (int, bool, undefined)
        ExpressionType exprType_;  // Type of the expression (ExpressionType enum)
        int position_; // position in the token vector (for error reporting)
        std::vector<Token*> const& tokens_; // reference to the token vector (for error reporting)
};

/**
 * @class OrExpr
 * @brief Represents a boolean OR expression in the Python-Sublanguage interpreter
 */
class OrExpr : public Expression{
    public:
        // constructors
        OrExpr() = delete;
        OrExpr(Join* left, Expression* right, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        OrExpr(OrExpr const& oe) = delete;

        // destructor
        ~OrExpr() = default;

        // methods
        Join* getLeft() const { return left_; }
        Expression* getRight() const { return right_; }

    private:
        Join* left_;
        Expression* right_;
};

/**
 * @enum JoinType
 * @brief Represents the different types of joins in the Python-Sublanguage interpreter
 */
enum JoinType {
    AND_JOIN,
    EQUALITY
};

/**
 * @class Join
 * @brief Represents a boolean expression in the Python-Sublanguage interpreter
 */
class Join : public Expression{
    public:
        // constructors
        Join() = delete;
        Join(int JoinType, int position, std::vector<Token*> const& tokens);
        Join(Join const& j) = delete;

        // destructor
        virtual ~Join() = default;

        // methods
        int getJoinType() const { return joinType_; }

    private:
        int joinType_;
};

/**
 * @class AndExpr
 * @brief Represents a boolean AND expression in the Python-Sublanguage interpreter
 */
class AndExpr : public Join{
    public:
        // constructors
        AndExpr() = delete;
        AndExpr(Equality* left, Join* right, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        AndExpr(AndExpr const& ae) = delete;

        // destructor
        ~AndExpr() = default;

        // methods
        Equality* getLeft() const { return left_; }
        Join* getRight() const { return right_; }

    private:
        Equality* left_;
        Join* right_;
};

/**
 * @enum EqualityType
 * @brief Represents the different types of equalities in the Python-Sublanguage interpreter
 */
enum EqualityType {
    COMP_EQUALITY,
    REL
};

/**
 * @class Equality
 * @brief Represents an equality expression in the Python-Sublanguage interpreter
 */
class Equality : public Join{
    public:
        // constructors
        Equality() = delete;
        Equality(int EqualityType, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Equality(Equality const& e) = delete;

        // destructor
        virtual ~Equality() = default;

        // methods
        int getEqualityType() const { return equalityType_; }

    private:
        int equalityType_;
};

/**
 * @enum EqualExprType
 * @brief Represents the different types of equality expressions in the Python-Sublanguage interpreter
 */
enum EqualExprType {
    EQ_EXPR,
    NEQ_EXPR
};

/**
 * @class EqualExpr
 * @brief Represents an equality expression in the Python-Sublanguage interpreter
 */
class EqualExpr : public Equality{
    public:
        // constructors
        EqualExpr() = delete;
        EqualExpr(Relation* left, RelationalToken* op, Equality* right, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        EqualExpr(EqualExpr const& ee) = delete;

        // destructor
        ~EqualExpr() = default;

        // methods
        Relation* getLeft() const { return left_; }
        Equality* getRight() const { return right_; }
        EqualExprType getType() const { return EqualExprType_; }

    private:
        Relation* left_;
        EqualExprType EqualExprType_; 
        Equality* right_;
};

/**
 * @enum RelationType
 * @brief Represents the different types of relations in the Python-Sublanguage interpreter
 */
enum RelationType {
    COMPARATIVE_RELATION,
    NUM_EXPR
};

/**
 * @class Relation
 * @brief Represents a relational expression in the Python-Sublanguage interpreter
 */
class Relation : public Equality{
    public:
        // constructors
        Relation() = delete;
        Relation(RelationType RelType, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Relation(Relation const& r) = delete;

        // destructor
        ~Relation() = default;

        // methods
        RelationType getRelType() const { return relType_; }

    private:
        RelationType relType_;
};

/**
 * @enum ComparativeRelationType
 * @brief Represents the different types of comparative relations in the Python-Sublanguage interpreter
 */
enum ComparativeRelationType {
    LT_REL,
    LE_REL,
    GT_REL,
    GE_REL
};

/**
 * @class ComparativeRelation
 * @brief Represents a comparative relational expression in the Python-Sublanguage interpreter
 */
class ComparativeRelation : public Relation{
    public:
        // constructors
        ComparativeRelation() = delete;
        ComparativeRelation(NumExpr* left, RelationalToken* op, NumExpr* right, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        ComparativeRelation(ComparativeRelation const& cr) = delete;

        // destructor
        ~ComparativeRelation() = default;

        // methods
        NumExpr* getLeft() const { return left_; }
        NumExpr* getRight() const { return right_; }
        ComparativeRelationType getType() const { return ComparativeRelationType_; }

    private:
        NumExpr* left_;
        ComparativeRelationType ComparativeRelationType_;
        NumExpr* right_;
};

/**
 * @enum NumExprType
 * @brief Represents the different types of numerical expressions in the Python-Sublanguage interpreter
 */
enum NumExprType {
    ARIT_EXPR,
    TERM
};

/**
 * @class NumExpr
 * @brief Represents a numerical expression in the Python-Sublanguage interpreter
 */
class NumExpr : public Relation{
    public:
        // constructors
        NumExpr() = delete;
        NumExpr(NumExprType ExprType, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        NumExpr(NumExpr const& ne) = delete;

        // destructor
        ~NumExpr() = default;

        // methods
        NumExprType getNumExprType() const { return numExprType_; }

    private:
        NumExprType numExprType_;
};

/**
 * @enum AritExprType
 * @brief Represents the different types of arithmetic expressions in the Python-Sublanguage interpreter
 */
enum AritExprType {
    ADD_EXPR,
    SUB_EXPR
};

/**
 * @class AritExpr
 * @brief Represents arithmetic expressions (addition and subtraction) between two numerical expressions in the Python-Sublanguage interpreter
 */
class AritExpr : public NumExpr{
    public:
        // constructors
        AritExpr() = delete;
        AritExpr(Term* left, ArithmeticToken* op, NumExpr* right, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        AritExpr(AritExpr const& ae) = delete;

        // destructor
        ~AritExpr() = default;

        // methods
        Term* getLeft() const { return left_; }
        NumExpr* getRight() const { return right_; }
        AritExprType getAritExprType() const { return aritExprType_; }

    private:
        Term* left_;
        AritExprType aritExprType_;
        NumExpr* right_;
};

/**
 * @enum TermType
 * @brief Represents the different types of terms in the Python-Sublanguage interpreter
 */
enum TermType {
    MULDIV_TERM,
    UNARY_TERM
};

/**
 * @class Term
 * @brief Represents an arithmetic expression in the Python-Sublanguage interpreter
 */
class Term : public NumExpr{
    public:
        // constructors
        Term() = delete;
        Term(int TermType, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Term(Term const& t) = delete;

        // destructor
        ~Term() = default;

        // methods
        int getTermType() const { return termType_; }

    private:
        int termType_;
};

/**
 * @enum MulDivTermType
 * @brief Represents the different types of multiplication/division terms in the Python-Sublanguage interpreter
 */
enum MulDivTermType {
    MUL_TERM,
    DIV_TERM
};

/**
 * @class MulTerm
 * @brief Represents multiplication between two terms in the Python-Sublanguage interpreter
 */
class MulDivTerm : public Term{
    public:
        // constructors
        MulDivTerm() = delete;
        MulDivTerm( Unary* left, ArithmeticToken* op, Term* right, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        MulDivTerm(MulDivTerm const& mdt) = delete;

        // destructor
        ~MulDivTerm() = default;

        // methods
        Unary* getLeft() const { return left_; }
        Term* getRight() const { return right_; }
        MulDivTermType getMulDivTermType() const { return mulDivTermType_; }

    private:
        Unary* left_;
        MulDivTermType mulDivTermType_;
        Term* right_;
};

/**
 * @enum UnaryType
 * @brief Represents the different types of unary expressions in the Python-Sublanguage interpreter
 */
enum UnaryType {
    NOT_UNARY,
    MINUS_UNARY,
    FACTOR
};

/**
 * @class Unary
 * @brief Represents a unary expression in the Python-Sublanguage interpreter
 */
class Unary : public Term{
    public:
        // constructors
        Unary() = delete;
        Unary(int unaryType, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Unary(Unary const& u) = delete;

        // destructor
        ~Unary() = default;

        // methods
        int getUnaryType() const { return unaryType_; }

    private:
        int unaryType_;
};

/**
 * @class NotUnary
 * @brief Represents a logical not unary expression in the Python-Sublanguage interpreter
 */
class NotUnary : public Unary{
    public:
        // constructors
        NotUnary() = delete;
        NotUnary(Unary* unary, int position, std::vector<Token*> const& tokens);
        NotUnary(NotUnary const& nu) = delete;

        // destructor
        ~NotUnary() = default;

        // methods
        Unary* getUnary() const { return unary_; }

    private:
        Unary* unary_;
};

/**
 * @class MinusUnary
 * @brief Represents a minus unary expression in the Python-Sublanguage interpreter
 */
class MinusUnary : public Unary{
    public:
        // constructors
        MinusUnary() = delete;
        MinusUnary(Unary* unary, int position, std::vector<Token*> const& tokens);
        MinusUnary(MinusUnary const& mu) = delete;

        // destructor
        ~MinusUnary() = default;

        // methods
        Unary* getUnary() const { return unary_; }

    private:
        Unary* unary_;
};

/**
 * @enum FactorType
 * @brief Represents the different types of factors in the Python-Sublanguage interpreter
 */
enum FactorType {
    EXPR_FACTOR,
    LOCATION,
    NUMBER,
    BOOL
};

/**
 * @class Factor
 * @brief Represents a factor in the Python-Sublanguage interpreter
 */
class Factor : public Unary{
    public:
        // constructors
        Factor() = delete;
        Factor(FactorType factorType, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Factor(Factor const& f) = delete;

        // destructor
        ~Factor() = default;

        // methods
        FactorType getFactorType() const { return factorType_; }

    private:
        FactorType factorType_;
};

/**
 * @class ExpressionFactor
 * @brief Represents an expression factor in the Python-Sublanguage interpreter
 */
class ExpressionFactor : public Factor{
    public:
        // constructors
        ExpressionFactor() = delete;
        ExpressionFactor(Expression* expr, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        ExpressionFactor(ExpressionFactor const& ef) = delete;

        // destructor
        virtual ~ExpressionFactor() = default;

        // methods
        Expression* getExpression() const { return expr_; }

    private:
        Expression* expr_;
};

/**
 * @class NumberFactor
 * @brief Represents a number factor in the Python-Sublanguage interpreter
 */
class NumberFactor : public Factor{
    public:
        // constructors
        NumberFactor() = delete;
        NumberFactor(NumberToken* number, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        NumberFactor(NumberFactor const& nf) = delete;

        // destructor
        ~NumberFactor() = default;

        // methods
        NumberToken* getNumber() const { return number_; }

    private:
        NumberToken* number_;
};

/**
 * @class BoolFactor
 * @brief Represents a boolean factor in the Python-Sublanguage interpreter
 */
class BoolFactor : public Factor{
    public:
        // constructors
        BoolFactor() = delete;
        BoolFactor(BoolToken* boolean, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        BoolFactor(BoolFactor const& bf) = delete;

        // destructor
        ~BoolFactor() = default;

        // methods
        BoolToken* getBool() const { return boolean_; }

    private:
        BoolToken* boolean_;
};

/**
 * @enum LocationType
 * @brief Represents the different types of locations in the Python-Sublanguage interpreter
 */
enum LocationType {
    ID,
    LIST_ELEM
};

/**
 * @class Location
 * @brief Represents a location in the Python-Sublanguage interpreter
 */
class Location : public Factor{
    public:
        // constructors
        Location() = delete;
        Location(int locType, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        Location(Location const& l) = delete;

        // destructor
        ~Location() = default;

        // methods
        int getLocationType() const { return locType_; }

    private:
        int locType_;
};

/**
 * @class IdLocation
 * @brief Represents an identifier location in the Python-Sublanguage interpreter
 */
class IdLocation : public Location{
    public:
        // constructors
        IdLocation() = delete;
        IdLocation(IdToken* id, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        IdLocation(IdLocation const& l) = delete;

        // destructor
        ~IdLocation() = default;

        // methods
        std::string getId() const { return id_->getStringValue(); }
    
    private:
        IdToken* id_;
};

/**
 * @class ListElementLocation
 * @brief Represents a list element location in the Python-Sublanguage interpreter
 */
class ListElementLocation : public Location{
    public:
        // constructors
        ListElementLocation() = delete;
        ListElementLocation(IdToken* id, Expression* expr, int position, std::vector<Token*> const& tokens); // defined in syntax.cpp
        ListElementLocation(ListElementLocation const& l) = delete;

        // destructor
        ~ListElementLocation() = default;

        // methods
        std::string getId() const { return id_->getStringValue(); }
        Expression* getIndex() const { return expr_; }

    private:
        IdToken* id_;
        Expression* expr_;
};

#endif