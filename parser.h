#if !defined(PARSER_H)
#define PARSER_H

#include <vector>
#include "token.h"
#include "syntax.h"
#include "error.h"

/**
 * @file parser.h
 * @brief Defines the Parser component of the Python-Sublanguage interpreter
 * 
 * This file contains the declaration of the Parser class, which is responsible for 
 * creating the Syntax Tree from the token vector.
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */


/**
 * @class Parser
 * @brief Syntactic analyzer for the Python-Sublanguage interpreter
 * 
 * The Parser class is responsible for creating the Syntax Tree from the token vector.
 */
class Parser{
    public:
        // constructors
        Parser() = delete;
        Parser(std::vector<Token*> tokens) : tokens_(std::move(tokens)) {} // move the token vector
        Parser(Parser const& p) = delete;

        // destructor
        ~Parser() = default;

        // overload () operator to perform the parsing
        Program* operator()() {
            // parse the token vector and create the Syntax Tree
            return parseProgram();
        }

        // methods to parse the token vector and create the Syntax Tree
        Program* parseProgram();
        Statement* parseStatement();
        AssignmentStatement* parseAssignmentStatement();
        ListDeclarationStatement* parseListDeclarationStatement();
        ListAppendStatement* parseListAppendStatement();
        BreakStatement* parseBreakStatement();
        ContinueStatement* parseContinueStatement();
        PrintStatement* parsePrintStatement();
        CompoundStatement* parseCompoundStatement();
        Block* parseBlock();
        ElifBlock* parseElifBlock();
        ElseBlock* parseElseBlock();

        Expression* parseExpression();
        OrExpr* parseOrExpr(Join* left);
        Join* parseJoin();
        AndExpr* parseAndExpr(Equality* left);
        Equality* parseEquality();
        EqualExpr* parseEqualExpr(Relation* left);
        Relation* parseRelation();
        ComparativeRelation* parseComparativeRelation(NumExpr* left);
        NumExpr* parseNumExpr();
        AritExpr* parseAritExpr(Term* left);
        Term* parseTerm();
        MulDivTerm* parseMulDivTerm(Unary* left);
        Unary* parseUnary();
        NotUnary* parseNotUnary();
        MinusUnary* parseMinusUnary();
        Factor* parseFactor();
        ExpressionFactor* parseExpressionFactor();
        NumberFactor* parseNumberFactor();
        BoolFactor* parseBoolFactor();
        Location* parseLocation();
        ListElementLocation* parseListElementLocation(IdToken* idToken);
        
    private:
        int index_{0};
        std::vector<Token*> tokens_;
};



#endif