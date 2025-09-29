#if !defined(VISITOR_H)
#define VISITOR_H

#include "syntax.h"
#include "semantics.h"
#include "error.h"

/**
 * @file visitor.h
 * @brief Defines the Visitor component of the Python-Sublanguage interpreter
 * 
 * This file contains the declaration of the Visitor class, which is responsible for 
 * collecting information from the Syntax Tree and performing semantic analysis.
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */


/**
 * @class Visitor
 * @brief Semantic analyzer for the Python-Sublanguage interpreter
 * 
 * The Visitor class is responsible for collecting information from the Syntax Tree and performing semantic analysis.
 */
class Visitor{
    public:
        // constructors
        Visitor() = delete;
        Visitor(Program* program) : program_(program) {}
        Visitor(Visitor const& v) = delete;

        // destructor
        ~Visitor() = default;

        // overload () operator to perform the visiting
        void operator()() {
            // visit the Syntax Tree and perform semantic analysis
            return visitProgram();
        }

        // Methods for variable management
        void addVariable(std::string id, EvaluatedElement element, int line, int column);
        void updateVariable(std::string id, EvaluatedElement element, int line, int column);
        EvaluatedElement getVariableValue(std::string id, int line, int column);
        bool isVariableDefined(std::string id);

        // Methods for list management
        void addList(std::string id);
        void appendToList(std::string id, EvaluatedElement element);
        void updateListElement(std::string id, int index, EvaluatedElement element);
        EvaluatedElement getListElement(std::string id, int index, int line, int column);
        int getListSize(std::string id, int line, int column);
        bool isListDefined(std::string id);

        // General methods
        bool isAlreadyDefined(std::string id);

        // Visitor methods for each type of statement
        void visitProgram();
        void visitStatement(Statement* stmt);
        void visitAssignmentStatement(AssignmentStatement* as);
        void visitListDeclarationStatement(ListDeclarationStatement* lds);
        void visitListAppendStatement(ListAppendStatement* las);
        void visitPrintStatement(PrintStatement* ps);
        void visitIfStatement(CompoundStatement* ifs);
        void visitWhileStatement(CompoundStatement* ws);
        void visitSimpleBlock(SimpleBlock* sb);
        void visitElifBlock(ElifBlock* elifBlock);
        void visitElseBlock(ElseBlock* elseBlock);
        void visitBreakStatement(BreakStatement* bs);
        void visitContinueStatement(ContinueStatement* cs);
        

        // Method to get the type of an expression
        Types getDataType(Expression* expr);

        // Evaluation methods for expressions
        EvaluatedElement* eval(Expression* expr);

        // Method to access the symbol table
        SymbolTable& getSymbolTable() { return symbolTable_; }

    private:
        Program* program_;
        SymbolTable symbolTable_;
        std::vector<bool> conditionMetStack_;
        std::vector<bool> loopStack_;
};


#endif