/**
 * @file visitor.cpp
 * @brief Implements the Visitor component of the Python-Sublanguage interpreter
 * 
 * expr file contains the implementation of the Visitor class, which is responsible for 
 * collecting information from the Syntax Tree and performing semantic analysis.
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */

#include "visitor.h"
#include "syntax.h"
#include "error.h"
#include <iostream>

/**
 * @brief Adds a variable to the symbol table
 * @param id The identifier of the variable
 * @param element The EvaluatedElement representing the value of the variable
 */
void Visitor::addVariable(std::string id, EvaluatedElement element, int line, int column) {
    if (element.getType() == Types::TYPE_INT) {
        symbolTable_.addVariable(id, element.getIntValue());
    } else if (element.getType() == Types::TYPE_BOOL) {
        symbolTable_.addVariable(id, element.getBoolValue());
    } else {
        throw InternalError(line, column, "Unknown EvaluatedElement type");
    }
}

/**
 * @brief Updates the value of an existing variable in the symbol table
 * @param id The identifier of the variable
 * @param element The EvaluatedElement representing the new value of the variable
 */
void Visitor::updateVariable(std::string id, EvaluatedElement element, int line, int column) {
    if (element.getType() == Types::TYPE_INT) {
        symbolTable_.updateVariable(id, element.getIntValue());
    } else if (element.getType() == Types::TYPE_BOOL) {
        symbolTable_.updateVariable(id, element.getBoolValue());
    } else {
        throw InternalError(line, column, "Unknown EvaluatedElement type");
    }
}

/**
 * @brief Retrieves the value of a variable from the symbol table
 * @param id The identifier of the variable
 * @return The EvaluatedElement representing the value of the variable
 */
EvaluatedElement Visitor::getVariableValue(std::string id, int line, int column) {
    if (!symbolTable_.isVariableDefined(id)) {
        throw InternalError(line, column, "Variable '" + id + "' is not defined");
    }
    return symbolTable_.getVariableValue(id);
}

/**
 * @brief Checks if a variable is defined in the symbol table
 * @param id The identifier of the variable
 * @return true if the variable is defined, false otherwise
 */
bool Visitor::isVariableDefined(std::string id) {
    return symbolTable_.isVariableDefined(id);
}

/**
 * @brief Adds a list to the symbol table
 * @param id The identifier of the list
 */
void Visitor::addList(std::string id) {
    symbolTable_.addList(id);
}

/**
 * @brief Appends an element to a list in the symbol table
 * @param id The identifier of the list
 * @param element The EvaluatedElement to append to the list
 */
void Visitor::appendToList(std::string id, EvaluatedElement element) {
    symbolTable_.appendToList(id, element);
}

/**
 * @brief Updates an element at a specific index in a list in the symbol table
 * @param id The identifier of the list
 * @param index The index of the element to update
 * @param element The new EvaluatedElement to set at the specified index
 */
void Visitor::updateListElement(std::string id, int index, EvaluatedElement element) {
    symbolTable_.updateListElement(id, index, element);
}

/**
 * @brief Retrieves an element at a specific index from a list in the symbol table
 * @param id The identifier of the list
 * @param index The index of the element to retrieve
 * @return The EvaluatedElement at the specified index in the list
 */
EvaluatedElement Visitor::getListElement(std::string id, int index, int line, int column) {
    if (!symbolTable_.isListDefined(id)) {
        throw InternalError(line, column, "List '" + id + "' is not defined");
    }
    return symbolTable_.getListElement(id, index);
}

/**
 * @brief Retrieves the size of a list in the symbol table
 * @param id The identifier of the list
 * @return The size of the list
 */
int Visitor::getListSize(std::string id, int line, int column) {
    if (!symbolTable_.isListDefined(id)) {
        throw InternalError(line, column, "List '" + id + "' is not defined");
    }
    return symbolTable_.getListSize(id);
}

/**
 * @brief Checks if a list is defined in the symbol table
 * @param id The identifier of the list
 * @return true if the list is defined, false otherwise
 */
bool Visitor::isListDefined(std::string id) {
    return symbolTable_.isListDefined(id);
}

/**
 * @brief Checks if an identifier is already defined as a variable or a list in the symbol table
 * @param id The identifier to check
 * @return true if the identifier is defined as a variable or a list, false otherwise
 */
bool Visitor::isAlreadyDefined(std::string id) {
    return isVariableDefined(id) || isListDefined(id);
}

/**
 * @brief Visits the entire program and performs semantic analysis
 */
void Visitor::visitProgram() {
    // Visit each statement in the program
    for(auto stmt : program_->getStatements()) {
        visitStatement(stmt);
    }
}

/**
 * @brief Visits a statement and dispatches to the appropriate visit method based on the statement type
 * @param stmt The statement to visit
 */
void Visitor::visitStatement(Statement* stmt) {
    switch(stmt->getStatementType()) {
        case ASSIGNMENT_STMT:
            visitAssignmentStatement(static_cast<AssignmentStatement*>(stmt));
            break;
        case LIST_DECL_STMT:
            visitListDeclarationStatement(static_cast<ListDeclarationStatement*>(stmt));
            break;
        case LIST_APP_STMT:
            visitListAppendStatement(static_cast<ListAppendStatement*>(stmt));
            break;
        case PRINT_STMT:
            visitPrintStatement(static_cast<PrintStatement*>(stmt));
            break;
        case IF_STMT:
            visitIfStatement(static_cast<CompoundStatement*>(stmt));
            break;
        case WHILE_STMT:
            visitWhileStatement(static_cast<CompoundStatement*>(stmt));
            break;
        case BREAK_STMT:
            visitBreakStatement(static_cast<BreakStatement*>(stmt));
            break;
        case CONTINUE_STMT:
            visitContinueStatement(static_cast<ContinueStatement*>(stmt));
            break;
        default:
            throw InternalError(stmt->getLine(), stmt->getColumn(), "Unknown StatementType");
    }
}

/**
 * @brief Visits an assignment statement and performs semantic analysis
 * @param as The assignment statement to visit
 */
void Visitor::visitAssignmentStatement(AssignmentStatement* as) {
    // Get the location object
    Location* loc = as->getLocation();
    if (!loc) {
        throw InternalError(as->getLine(), as->getColumn(), "Null location in assignment statement");
    }

    // Get the IdLocation object or ListElementLocation object by checking the location type
    if (loc->getLocationType() == LocationType::ID) {
        IdLocation* idLoc = static_cast<IdLocation*>(loc);
    } else if (loc->getLocationType() == LocationType::LIST_ELEM) {
        ListElementLocation* listElemLoc = static_cast<ListElementLocation*>(loc);
    } else {
        throw InternalError(loc->getLine(), loc->getColumn(), "Unknown LocationType in assignment statement");
    }

    // Evaluate the expression on the right-hand side
    Expression* expr = as->getExpression();
    if (!expr) {
        throw InternalError(as->getLine(), as->getColumn(), "Null expression in assignment statement");
    }

    EvaluatedElement* value = eval(expr);
    if (!value) {
        throw InternalError(expr->getLine(), expr->getColumn(), "Failed to evaluate expression in assignment statement");
    }

    // Perform the assignment based on the location type
    if (loc->getLocationType() == LocationType::ID) {
        IdLocation* idLoc = static_cast<IdLocation*>(loc);
        std::string id = idLoc->getId();
        if (isVariableDefined(id)) {
            updateVariable(id, *value, idLoc->getLine(), idLoc->getColumn());
        } else if (isListDefined(id) && !isVariableDefined(id)) {
            // Dynamically delete the existing list and create a new variable
            symbolTable_.clear(id);
            addVariable(id, *value, idLoc->getLine(), idLoc->getColumn());
        } else if (!isAlreadyDefined(id)) {
            // If the variable is not defined, create it
            addVariable(id, *value, idLoc->getLine(), idLoc->getColumn());
        } else {
            throw SemanticError(idLoc->getLine(), idLoc->getColumn(), "Identifier '" + id + "' is not defined");
        }
    } else if (loc->getLocationType() == LocationType::LIST_ELEM) {
        ListElementLocation* listElemLoc = static_cast<ListElementLocation*>(loc);
        std::string listId = listElemLoc->getId();
        if (!isListDefined(listId)) {
            throw SemanticError(listElemLoc->getLine(), listElemLoc->getColumn(), "List '" + listId + "' is not defined");
        }
        // Get the index expression and evaluate it
        Expression* indexExpr = listElemLoc->getIndex();
        if (!indexExpr) {
            throw InternalError(listElemLoc->getLine(), listElemLoc->getColumn(), "Null index expression in list element location");
        }
        EvaluatedElement* indexValue = eval(indexExpr);
        if (!indexValue) {
            throw InternalError(indexExpr->getLine(), indexExpr->getColumn(), "Failed to evaluate index expression in list element location");
        }
        if (indexValue->getType() != Types::TYPE_INT) {
            throw SemanticError(indexExpr->getLine(), indexExpr->getColumn(), "List index must be an integer");
        }
        int index = indexValue->getIntValue();
        // Update the list element at the specified index
        updateListElement(listId, index, *value);
    } else {
        throw InternalError(loc->getLine(), loc->getColumn(), "Unknown LocationType in assignment statement");
    }
}

/**
 * @brief Visits a list declaration statement and performs semantic analysis
 * @param lds The list declaration statement to visit
 */
void Visitor::visitListDeclarationStatement(ListDeclarationStatement* lds) {
    std::string id = lds->getId();
    if (isAlreadyDefined(id)) {
        throw SemanticError(lds->getLine(), lds->getColumn(), "Identifier '" + id + "' is already defined");
    }
    addList(id);
}

/**
 * @brief Visits a list append statement and performs semantic analysis
 * @param las The list append statement to visit
 */
void Visitor::visitListAppendStatement(ListAppendStatement* las) {
    std::string id = las->getId();
    if (!isListDefined(id)) {
        throw SemanticError(las->getLine(), las->getColumn(), "List '" + id + "' is not defined");
    }
    Expression* expr = las->getExpression();
    if (!expr) {
        throw InternalError(las->getLine(), las->getColumn(), "Null expression in list append statement");
    }
    EvaluatedElement* value = eval(expr);
    if (!value) {
        throw InternalError(expr->getLine(), expr->getColumn(), "Failed to evaluate expression in list append statement");
    }
    appendToList(id, *value);
}

/**
 * @brief Visits a print statement and performs semantic analysis
 * @param ps The print statement to visit
 */
void Visitor::visitPrintStatement(PrintStatement* ps) {
    Expression* expr = ps->getExpression();
    if (!expr) {
        throw InternalError(ps->getLine(), ps->getColumn(), "Null expression in print statement");
    }
    EvaluatedElement* value = eval(expr);
    if (!value) {
        throw InternalError(expr->getLine(), expr->getColumn(), "Failed to evaluate expression in print statement");
    }
    // Print the value based on its type
    if (value->getType() == Types::TYPE_INT) {
        std::cout << value->getIntValue() << std::endl;
    } else if (value->getType() == Types::TYPE_BOOL) {
        std::cout << (value->getBoolValue() ? "True" : "False") << std::endl;
    } else {
        throw InternalError(expr->getLine(), expr->getColumn(), "Unknown EvaluatedElement type in print statement");
    }
}

/**
 * @brief Visits an if statement and performs semantic analysis
 * @param ifs The if statement to visit
 */
void Visitor::visitIfStatement(CompoundStatement* ifs) {
    // Get the condition expression
    Expression* condition = ifs->getExpression();
    if (!condition) {
        throw InternalError(ifs->getLine(), ifs->getColumn(), "Null condition in if statement");
    }
    // Evaluate the condition expression
    EvaluatedElement* condValue = eval(condition);
    if (!condValue) {
        throw InternalError(condition->getLine(), condition->getColumn(), "Failed to evaluate condition in if statement");
    }
    // Adds a new level to the conditionMetStack_
    conditionMetStack_.push_back(false);

    // Check that the condition is boolean
    if (condValue->getType() != Types::TYPE_BOOL) {
        throw SemanticError(condition->getLine(), condition->getColumn(), "If condition must be boolean");
    }

    // If the condition is true, visit the blocks (check the blocktype and executes the corresponding block)
    if (condValue->getBoolValue()) {
        conditionMetStack_.back() = true;
        for (auto block : ifs->getBlocks()) {
            if (block->getBlockType() == BlockType::SIMPLE_BLOCK) {
                visitSimpleBlock(static_cast<SimpleBlock*>(block));
            } else if (block->getBlockType() == BlockType::ELIF_BLOCK) {
                visitElifBlock(static_cast<ElifBlock*>(block));
            } else if (block->getBlockType() == BlockType::ELSE_BLOCK) {
                visitElseBlock(static_cast<ElseBlock*>(block));
            }
        }
    }

    // Check for elif and else blocks
    for (auto block : ifs->getBlocks()) {
        if (block->getBlockType() == BlockType::ELIF_BLOCK) {
            visitElifBlock(static_cast<ElifBlock*>(block));
        } else if (block->getBlockType() == BlockType::ELSE_BLOCK) {
            visitElseBlock(static_cast<ElseBlock*>(block));
        }
    }

    // Remove the last level from the conditionMetStack_
    conditionMetStack_.pop_back();
}

/**
 * @brief Visits a simple block and performs semantic analysis
 * @param sb The simple block to visit
 */
void Visitor::visitSimpleBlock(SimpleBlock* sb) {
    for (auto stmt : sb->getStatements()) {
        visitStatement(stmt);
    }
}

/**
 * @brief Visits an elif block and performs semantic analysis
 * @param elifBlock The elif block to visit
 */
void Visitor::visitElifBlock(ElifBlock* elifBlock) {
    // Check if a previous condition was met
    if (!conditionMetStack_.empty() && conditionMetStack_.back()) {
        return;
    }
    else if (conditionMetStack_.empty()) {
        throw InternalError(elifBlock->getLine(), elifBlock->getColumn(), "Elif block outside of if statement");
    }

    // Evaluate the condition expression
    Expression* condition = elifBlock->getCondition();
    if (!condition) {
        throw InternalError(elifBlock->getLine(), elifBlock->getColumn(), "Null condition in elif block");
    }
    EvaluatedElement* condValue = eval(condition);
    if (!condValue) {
        throw InternalError(condition->getLine(), condition->getColumn(), "Failed to evaluate condition in elif block");
    }

    // If the condition is true, visit the block
    if (condValue->getBoolValue()) {
        conditionMetStack_.back() = true;
        visitSimpleBlock(static_cast<SimpleBlock*>(elifBlock->getBlock()));
        // Set the conditionMetStack_ to true to prevent further blocks from being executed
        conditionMetStack_.back() = true;
    }
}

/**
 * @brief Visits an else block and performs semantic analysis
 * @param elseBlock The else block to visit
 */
void Visitor::visitElseBlock(ElseBlock* elseBlock) {
    // Check if a previous condition was met
    if (!conditionMetStack_.empty() && conditionMetStack_.back()) {
        return;
    }
    else if (conditionMetStack_.empty()) {
        throw InternalError(elseBlock->getLine(), elseBlock->getColumn(), "Else block outside of if statement");
    }

    // Visit the block
    visitSimpleBlock(static_cast<SimpleBlock*>(elseBlock->getBlock()));
    // Set the conditionMetStack_ to true to prevent further blocks from being executed
    conditionMetStack_.back() = true;
}

/**
 * @brief Visits a while statement and performs semantic analysis
 * @param ws The while statement to visit
 */
void Visitor::visitWhileStatement(CompoundStatement* ws) {
    // Get the condition expression
    Expression* condition = ws->getExpression();
    if (!condition) {
        throw InternalError(ws->getLine(), ws->getColumn(), "Null condition in while statement");
    }

    // Adds a new level to the loopStack_
    loopStack_.push_back(true);

    // Evaluate the condition expression and visit the block while the condition is true
    while (true) {
        EvaluatedElement* condValue = eval(condition);

        if (!condValue) {
            throw InternalError(condition->getLine(), condition->getColumn(), "Failed to evaluate condition in while statement");
        }

        // Check that the condition is boolean
        if (condValue->getType() != Types::TYPE_BOOL) {
            throw SemanticError(condition->getLine(), condition->getColumn(), "While condition must be boolean");
        }

        // If the condition is false, exit the loop
        if (!condValue->getBoolValue()) {
            break;
        }

        // If a break statement was encountered in the previous iteration, exit the loop
        if (!loopStack_.back()) {
            break;
        }

        // Check if there is more than one block (which is an error)
        if (ws->getBlocks().size() != 1) {
            throw SemanticError(ws->getLine(), ws->getColumn(), "While statement must have exactly one block");
        }

        // Visit the block
        SimpleBlock* block = static_cast<SimpleBlock*>(ws->getBlocks()[0]);

        // Visit the block manually becase of break and continue statements
        for (auto stmt : block->getStatements()) {
            // If a break statement was encountered, exit the loop
            if (stmt->getStatementType() == StatementType::BREAK_STMT) {
                loopStack_.back() = false;
                break;
            }
            // If a continue statement was encountered, skip to the next iteration
            else if (stmt->getStatementType() == StatementType::CONTINUE_STMT) {
                continue;
            }
            visitStatement(stmt);
        }
    }

    // Remove the last level from the loopStack_
    loopStack_.pop_back();
}

/**
 * @brief Visits a break statement
 * @param bs The break statement to visit
 */
void Visitor::visitBreakStatement(BreakStatement* bs) {
    // Check if we are inside a loop
    if (loopStack_.empty()) {
        throw SemanticError(bs->getLine(), bs->getColumn(), "Break statement not allowed outside of loop");
    }

    // Mark the loop as broken
    loopStack_.back() = false;
}

/**
 * @brief Visits a continue statement
 * @param cs The continue statement to visit
 */
void Visitor::visitContinueStatement(ContinueStatement* cs) {
    // Check if we are inside a loop
    if (loopStack_.empty()) {
        throw SemanticError(cs->getLine(), cs->getColumn(), "Continue statement not allowed outside of loop");
    }
}

/**
 * @brief Determines the data type of an expression
 * @param expr The expression to check
 * @return The Types enum value representing the data type of the expression
 */
EvaluatedElement* Visitor::eval(Expression* expr) {
    if (expr->getExprType() == ExpressionType::OR_EXPR) {
        // Downcast to OrExpr
        OrExpr* orExpr = static_cast<OrExpr*>(expr);

        // Check the type of the expressions (if they aren't boolean, raise an error)
        if (
            getDataType(orExpr->getLeft()) != Types::TYPE_BOOL || 
            getDataType(orExpr->getRight()) != Types::TYPE_BOOL
        ) {
            throw TypeError(orExpr->getLine(), orExpr->getColumn(), "Operands of 'or' must be boolean");
        }
        // Evaluate the left and right expressions
        EvaluatedElement* leftValue = eval(orExpr->getLeft());
        // Short-circuit evaluation
        if (leftValue->getBoolValue()) {
            return new EvaluatedElement(true);
        }
        // If leftValue is false, evaluate the right expression
        EvaluatedElement* rightValue = eval(orExpr->getRight());
        if (!rightValue) {
            throw InternalError(orExpr->getRight()->getLine(), orExpr->getRight()->getColumn(), "Failed to evaluate right operand of 'or'");
        }
        return new EvaluatedElement(rightValue->getBoolValue()); // (False) OR (X) = (X)

    } else if (expr->getExprType() == ExpressionType::JOIN) {
        // Downcast to Join
        Join* join = static_cast<Join*>(expr);
        if (join->getJoinType() == JoinType::AND_JOIN) {
            AndExpr* andExpr = static_cast<AndExpr*>(join);
            // Check the type of the expressions (if they aren't boolean, raise an error)
            if (
                getDataType(andExpr->getLeft()) != Types::TYPE_BOOL || 
                getDataType(andExpr->getRight()) != Types::TYPE_BOOL
            ) {
                throw TypeError(andExpr->getLine(), andExpr->getColumn(), "Operands of 'and' must be boolean");
            }
            // Evaluate the left and right expressions
            EvaluatedElement* leftValue = eval(andExpr->getLeft());

            // Short-circuit evaluation
            if (!leftValue->getBoolValue()) {
                return new EvaluatedElement(false);
            }
            // If leftValue is true, evaluate the right expression
            EvaluatedElement* rightValue = eval(andExpr->getRight());
            if (!rightValue) {
                throw InternalError(andExpr->getRight()->getLine(), andExpr->getRight()->getColumn(), "Failed to evaluate right operand of 'and'");
            }
            return new EvaluatedElement(rightValue->getBoolValue()); // (True) AND (X) = (X)

        } else if (join->getJoinType() == JoinType::EQUALITY) {
            Equality* equality = static_cast<Equality*>(join);
            if (equality->getEqualityType() == EqualityType::COMP_EQUALITY){
                EqualExpr* eqExpr = static_cast<EqualExpr*>(equality);
                // Check that both sides are of the same type (int or bool)
                Types leftType = getDataType(eqExpr->getLeft());
                Types rightType = getDataType(eqExpr->getRight());
                if(leftType == Types::TYPE_UNDEFINED || rightType == Types::TYPE_UNDEFINED || leftType != rightType) {
                    throw TypeError(eqExpr->getLine(), eqExpr->getColumn(), "Operands of '==' and '!=' must be of the same type (int or bool)");
                }
                // Evaluate the left and right expressions
                EvaluatedElement* leftValue = eval(eqExpr->getLeft());
                EvaluatedElement* rightValue = eval(eqExpr->getRight());
                if (!leftValue || !rightValue) {
                    throw InternalError(eqExpr->getLine(), eqExpr->getColumn(), "Failed to evaluate operands of '=='");
                }
                // Get the operator
                EqualExprType op = eqExpr->getType();
                if (op == EqualExprType::EQ_EXPR) {
                    if (leftValue->getType() == Types::TYPE_BOOL) {
                        return new EvaluatedElement(leftValue->getBoolValue() == rightValue->getBoolValue());
                    } else if (leftValue->getType() == Types::TYPE_INT) {
                        return new EvaluatedElement(leftValue->getIntValue() == rightValue->getIntValue());
                    } else {
                        throw InternalError(eqExpr->getLine(), eqExpr->getColumn(), "Unknown EvaluatedElement type in '==' expression");
                    }
                } else if (op == EqualExprType::NEQ_EXPR) {
                    if (leftValue->getType() == Types::TYPE_BOOL) {
                        return new EvaluatedElement(leftValue->getBoolValue() != rightValue->getBoolValue());
                    } else if (leftValue->getType() == Types::TYPE_INT) {
                        return new EvaluatedElement(leftValue->getIntValue() != rightValue->getIntValue());
                    } else {
                        throw InternalError(eqExpr->getLine(), eqExpr->getColumn(), "Unknown EvaluatedElement type in '!=' expression");
                    }
                } else {
                    throw InternalError(eqExpr->getLine(), eqExpr->getColumn(), "Unknown operator in '==' expression");
                }
            }
            else if (equality->getEqualityType() == EqualityType::REL) {
                Relation* relation = static_cast<Relation*>(equality);
                if (relation->getRelType() == RelationType::COMPARATIVE_RELATION) {
                    ComparativeRelation* compRel = static_cast<ComparativeRelation*>(relation);
                    // Check that both sides are integers
                    Types leftType = getDataType(compRel->getLeft());
                    Types rightType = getDataType(compRel->getRight());
                    if(leftType != Types::TYPE_INT || rightType != Types::TYPE_INT) {
                        throw TypeError(compRel->getLine(), compRel->getColumn(), "Operands of '<', '<=', '>', '>=' must be integers");
                    }
                    // Evaluate the left and right expressions
                    EvaluatedElement* leftValue = eval(compRel->getLeft());
                    EvaluatedElement* rightValue = eval(compRel->getRight());
                    if (!leftValue || !rightValue) {
                        throw InternalError(compRel->getLine(), compRel->getColumn(), "Failed to evaluate operands of relational expression");
                    }

                    // Get the operator
                    ComparativeRelationType op = compRel->getType();
                    if (op == ComparativeRelationType::LT_REL) {
                        return new EvaluatedElement(leftValue->getIntValue() < rightValue->getIntValue());
                    } else if (op == ComparativeRelationType::LE_REL) {
                        return new EvaluatedElement(leftValue->getIntValue() <= rightValue->getIntValue());
                    } else if (op == ComparativeRelationType::GT_REL) {
                        return new EvaluatedElement(leftValue->getIntValue() > rightValue->getIntValue());
                    } else if (op == ComparativeRelationType::GE_REL) {
                        return new EvaluatedElement(leftValue->getIntValue() >= rightValue->getIntValue());
                    } else {
                        throw InternalError(compRel->getLine(), compRel->getColumn(), "Unknown operator in relational expression");
                    }
                } else if (relation->getRelType() == RelationType::NUM_EXPR) {
                    NumExpr* numExpr = static_cast<NumExpr*>(relation);
                    if (numExpr->getNumExprType() == NumExprType::ARIT_EXPR) {
                        AritExpr* aritExpr = static_cast<AritExpr*>(numExpr);
                        // Check that both sides are integers
                        Types leftType = getDataType(aritExpr->getLeft());
                        Types rightType = getDataType(aritExpr->getRight());
                        if(leftType != Types::TYPE_INT || rightType != Types::TYPE_INT) {
                            throw TypeError(aritExpr->getLine(), aritExpr->getColumn(), "Operands of arithmetic expressions must be integers");
                        }
                        // Evaluate the left and right expressions
                        EvaluatedElement* leftValue = eval(aritExpr->getLeft());
                        EvaluatedElement* rightValue = eval(aritExpr->getRight());
                        if (!leftValue || !rightValue) {
                            throw InternalError(aritExpr->getLine(), aritExpr->getColumn(), "Failed to evaluate operands of arithmetic expression");
                        }
                        // Get the operator
                        AritExprType op = aritExpr->getAritExprType();
                        if (op == AritExprType::ADD_EXPR) {
                            return new EvaluatedElement(leftValue->getIntValue() + rightValue->getIntValue());
                        } else if (op == AritExprType::SUB_EXPR) {
                            return new EvaluatedElement(leftValue->getIntValue() - rightValue->getIntValue());
                        } else {
                            throw InternalError(aritExpr->getLine(), aritExpr->getColumn(), "Unknown operator in arithmetic expression");
                        }
                    } else if (numExpr->getNumExprType() == NumExprType::TERM) {
                        Term* term = static_cast<Term*>(numExpr);

                        if (term->getTermType() == TermType::MULDIV_TERM) {
                            MulDivTerm* mulDivTerm = static_cast<MulDivTerm*>(term);

                            // Check that both sides are integers
                            Types leftType = getDataType(mulDivTerm->getLeft());
                            Types rightType = getDataType(mulDivTerm->getRight());
                            if(leftType != Types::TYPE_INT || rightType != Types::TYPE_INT) {
                                throw TypeError(mulDivTerm->getLine(), mulDivTerm->getColumn(), "Operands of arithmetic expressions must be integers");
                            }
                            // Evaluate the left and right expressions
                            EvaluatedElement* leftValue = eval(mulDivTerm->getLeft());
                            EvaluatedElement* rightValue = eval(mulDivTerm->getRight());
                            if (!leftValue || !rightValue) {
                                throw InternalError(mulDivTerm->getLine(), mulDivTerm->getColumn(), "Failed to evaluate operands of arithmetic expression");
                            }
                            // Get the operator
                            MulDivTermType op = mulDivTerm->getMulDivTermType();
                            if (op == MulDivTermType::MUL_TERM) {
                                return new EvaluatedElement(leftValue->getIntValue() * rightValue->getIntValue());
                            } else if (op == MulDivTermType::DIV_TERM) {
                                if (rightValue->getIntValue() == 0) {
                                    throw ZeroDivisionError(mulDivTerm->getLine(), mulDivTerm->getColumn(), "Division by zero");
                                }
                                return new EvaluatedElement(leftValue->getIntValue() / rightValue->getIntValue());
                            } else {
                                throw InternalError(mulDivTerm->getLine(), mulDivTerm->getColumn(), "Unknown operator in arithmetic expression");
                            }
                        } else if (term->getTermType() == TermType::UNARY_TERM) {
                            Unary* unary = static_cast<Unary*>(term);
                            if (
                                unary->getUnaryType() == UnaryType::NOT_UNARY ||
                                unary->getUnaryType() == UnaryType::MINUS_UNARY
                            ) {
                                Unary* unaryExpr = static_cast<Unary*>(unary);
                                if (unaryExpr->getUnaryType() == UnaryType::NOT_UNARY) {
                                    NotUnary* notUnary = static_cast<NotUnary*>(unaryExpr);
                                    // Check that the operand is boolean
                                    Types unaryType = getDataType(notUnary->getUnary());
                                    if(unaryType != Types::TYPE_BOOL) {
                                        throw TypeError(notUnary->getLine(), notUnary->getColumn(), "Operand of 'not' must be boolean");
                                    }
                                    // Evaluate the operand
                                    EvaluatedElement* unaryValue = eval(notUnary->getUnary());
                                    if (!unaryValue) {
                                        throw InternalError(notUnary->getLine(), notUnary->getColumn(), "Failed to evaluate operand of 'not'");
                                    }

                                    return new EvaluatedElement(!unaryValue->getBoolValue());
                                } else if (unaryExpr->getUnaryType() == UnaryType::MINUS_UNARY) {
                                    MinusUnary* minusUnary = static_cast<MinusUnary*>(unaryExpr);
                                    // Check that the operand is integer
                                    Types unaryType = getDataType(minusUnary->getUnary());
                                    if(unaryType != Types::TYPE_INT) {
                                        throw TypeError(minusUnary->getLine(), minusUnary->getColumn(), "Operand of unary '-' must be integer");
                                    }
                                    // Evaluate the operand
                                    EvaluatedElement* unaryValue = eval(minusUnary->getUnary());
                                    if (!unaryValue) {
                                        throw InternalError(minusUnary->getLine(), minusUnary->getColumn(), "Failed to evaluate operand of unary '-'");
                                    }
                                    return new EvaluatedElement(-unaryValue->getIntValue());
                                } else {
                                    throw InternalError(unaryExpr->getLine(), unaryExpr->getColumn(), "Unknown unary expression type");
                                }
                            } else if (unary->getUnaryType() == UnaryType::FACTOR) {
                                Factor* factor = static_cast<Factor*>(unary);
                                if (factor->getFactorType() == FactorType::EXPR_FACTOR) {
                                    ExpressionFactor* exprFactor = static_cast<ExpressionFactor*>(factor);
                                    return eval(exprFactor->getExpression());
                                } else if (factor->getFactorType() == FactorType::NUMBER) {
                                    NumberFactor* number = static_cast<NumberFactor*>(factor);
                                    return new EvaluatedElement(number->getNumber()->getIntValue());
                                }
                                else if (factor->getFactorType() == FactorType::BOOL) {
                                    BoolFactor* boolFactor = static_cast<BoolFactor*>(factor);
                                    return new EvaluatedElement(boolFactor->getBool()->getBoolValue());
                                } 
                                else if (factor->getFactorType() == FactorType::LOCATION) {
                                    Location* locFactor = static_cast<Location*>(factor);
                                    if (locFactor->getLocationType() == LocationType::ID) {
                                        IdLocation* idLoc = static_cast<IdLocation*>(locFactor);
                                        std::string id = idLoc->getId();
                                        if (!isVariableDefined(id)) {
                                            throw SemanticError(idLoc->getLine(), idLoc->getColumn(), "Variable '" + id + "' is not defined");
                                        }
                                        return new EvaluatedElement(getVariableValue(id, idLoc->getLine(), idLoc->getColumn()));
                                    } else if (locFactor->getLocationType() == LocationType::LIST_ELEM) {
                                        ListElementLocation* listElemLoc = static_cast<ListElementLocation*>(locFactor);
                                        std::string id = listElemLoc->getId();
                                        if (!isListDefined(id)) {
                                            throw SemanticError(listElemLoc->getLine(), listElemLoc->getColumn(), "List '" + id + "' is not defined");
                                        }
                                        // Evaluate the index expression
                                        EvaluatedElement* indexValue = eval(listElemLoc->getIndex());
                                        if (!indexValue) {
                                            throw InternalError(listElemLoc->getLine(), listElemLoc->getColumn(), "Failed to evaluate index expression in list element access");
                                        }
                                        if (indexValue->getType() != Types::TYPE_INT) {
                                            throw TypeError(listElemLoc->getLine(), listElemLoc->getColumn(), "List index must be an integer");
                                        }
                                        if (indexValue->getIntValue() < 0 || indexValue->getIntValue() >= getListSize(id, listElemLoc->getLine(), listElemLoc->getColumn())) {
                                            throw SemanticError(listElemLoc->getLine(), listElemLoc->getColumn(), "List index out of bounds");
                                        }
                                        int index = indexValue->getIntValue();
                                        return new EvaluatedElement(getListElement(id, index, listElemLoc->getLine(), listElemLoc->getColumn()));
                                    } else {
                                        throw InternalError(locFactor->getLine(), locFactor->getColumn(), "Unknown LocationType in factor");
                                    }
                                } else {
                                    throw InternalError(factor->getLine(), factor->getColumn(), "Unknown FactorType in unary");
                                } 
                            } else {
                                throw InternalError(unary->getLine(), unary->getColumn(), "Unknown UnaryType in term");
                            }
                        } else {
                            throw InternalError(term->getLine(), term->getColumn(), "Unknown TermType in num expr");
                        }
                    } else {
                        throw InternalError(numExpr->getLine(), numExpr->getColumn(), "Unknown NumExprType in relation");
                    }
                } else {
                    throw InternalError(relation->getLine(), relation->getColumn(), "Unknown RelationType in equality");
                }
            } else {
                throw InternalError(equality->getLine(), equality->getColumn(), "Unknown EqualityType in join");
            }
        } else {
            throw InternalError(join->getLine(), join->getColumn(), "Unknown JoinType in or expr");
        }
    } else {
        throw InternalError(expr->getLine(), expr->getColumn(), "Unknown ExpressionType");
    }
    return nullptr; // Should never reach here
}

/**
 * @brief Determines the data type of an expression without evaluating it
 * @param expr The expression to check
 * @return The Types enum value representing the data type of the expression
 */
Types Visitor::getDataType(Expression* expr) {
    if (expr->getExprType() == ExpressionType::OR_EXPR) {
        OrExpr* orExpr = static_cast<OrExpr*>(expr);
        Types leftType = getDataType(orExpr->getLeft());
        Types rightType = getDataType(orExpr->getRight());
        if (leftType == Types::TYPE_BOOL && rightType == Types::TYPE_BOOL) {
            return Types::TYPE_BOOL;
        } else {
            return Types::TYPE_UNDEFINED;
        }
    } else if (expr->getExprType() == ExpressionType::JOIN) {
        Join* join = static_cast<Join*>(expr);
        if (join->getJoinType() == JoinType::AND_JOIN) {
            AndExpr* andExpr = static_cast<AndExpr*>(join);
            Types leftType = getDataType(andExpr->getLeft());
            Types rightType = getDataType(andExpr->getRight());
            if (leftType == Types::TYPE_BOOL && rightType == Types::TYPE_BOOL) {
                return Types::TYPE_BOOL;
            } else {
                return Types::TYPE_UNDEFINED;
            }
        } else if (join->getJoinType() == JoinType::EQUALITY) {
            Equality* equality = static_cast<Equality*>(join);
            if (equality->getEqualityType() == EqualityType::COMP_EQUALITY){
                EqualExpr* eqExpr = static_cast<EqualExpr*>(equality);
                Types leftType = getDataType(eqExpr->getLeft());
                Types rightType = getDataType(eqExpr->getRight());
                if(leftType == Types::TYPE_UNDEFINED || rightType == Types::TYPE_UNDEFINED || leftType != rightType) {
                    return Types::TYPE_UNDEFINED;
                }
                return Types::TYPE_BOOL;
            }
            else if (equality->getEqualityType() == EqualityType::REL) {
                Relation* relation = static_cast<Relation*>(equality);
                if (relation->getRelType() == RelationType::COMPARATIVE_RELATION) {
                    ComparativeRelation* compRel = static_cast<ComparativeRelation*>(relation);
                    Types leftType = getDataType(compRel->getLeft());
                    Types rightType = getDataType(compRel->getRight());
                    if(leftType == Types::TYPE_INT && rightType == Types::TYPE_INT) {
                        return Types::TYPE_BOOL;
                    } else {
                        return Types::TYPE_UNDEFINED;
                    }
                } else if (relation->getRelType() == RelationType::NUM_EXPR) {
                    NumExpr* numExpr = static_cast<NumExpr*>(relation);
                    if (numExpr->getNumExprType() == NumExprType::ARIT_EXPR) {
                        AritExpr* aritExpr = static_cast<AritExpr*>(numExpr);
                        Types leftType = getDataType(aritExpr->getLeft());
                        Types rightType = getDataType(aritExpr->getRight());
                        if(leftType == Types::TYPE_INT && rightType == Types::TYPE_INT) {
                            return Types::TYPE_INT;
                        } else {
                            return Types::TYPE_UNDEFINED;
                        }
                    } else if (numExpr->getNumExprType() == NumExprType::TERM) {
                        Term* term = static_cast<Term*>(numExpr);
                        if (term->getTermType() == TermType::MULDIV_TERM) {
                            MulDivTerm* mulDivTerm = static_cast<MulDivTerm*>(term);
                            Types leftType = getDataType(mulDivTerm->getLeft());
                            Types rightType = getDataType(mulDivTerm->getRight());
                            if(leftType == Types::TYPE_INT && rightType == Types::TYPE_INT) {
                                return Types::TYPE_INT;
                            } else {
                                return Types::TYPE_UNDEFINED;
                            }
                        } else if (term->getTermType() == TermType::UNARY_TERM) {
                            Unary* unary = static_cast<Unary*>(term);
                            if (
                                unary->getUnaryType() == UnaryType::NOT_UNARY ||
                                unary->getUnaryType() == UnaryType::MINUS_UNARY
                            ) {
                                if (unary->getUnaryType() == UnaryType::NOT_UNARY) {
                                    NotUnary* notUnary = static_cast<NotUnary*>(unary);
                                    Types unaryType = getDataType(notUnary->getUnary());
                                    if(unaryType == Types::TYPE_BOOL) {
                                        return Types::TYPE_BOOL;
                                    } else {
                                        return Types::TYPE_UNDEFINED;
                                    }
                                } else if (unary->getUnaryType() == UnaryType::MINUS_UNARY) {
                                    MinusUnary* minusUnary = static_cast<MinusUnary*>(unary);
                                    Types unaryType = getDataType(minusUnary->getUnary());
                                    if(unaryType == Types::TYPE_INT) {
                                        return Types::TYPE_INT;
                                    } else {
                                        return Types::TYPE_UNDEFINED;
                                    }
                                } else {
                                    return Types::TYPE_UNDEFINED;
                                }
                            } else if (unary->getUnaryType() == UnaryType::FACTOR) {
                                Factor* factor = static_cast<Factor*>(unary);
                                if (factor->getFactorType() == FactorType::EXPR_FACTOR) {
                                    ExpressionFactor* exprFactor = static_cast<ExpressionFactor*>(factor);
                                    return getDataType(exprFactor->getExpression());
                                } else if (factor->getFactorType() == FactorType::NUMBER) {
                                    return Types::TYPE_INT;
                                }
                                else if (factor->getFactorType() == FactorType::BOOL) {
                                    return Types::TYPE_BOOL;
                                } 
                                else if (factor->getFactorType() == FactorType::LOCATION) {
                                    Location* locFactor = static_cast<Location*>(factor);
                                    if (locFactor->getLocationType() == LocationType::ID) {
                                        IdLocation* idLoc = static_cast<IdLocation*>(locFactor);
                                        std::string id = idLoc->getId();
                                        if (!isVariableDefined(id)) {
                                            throw SemanticError(idLoc->getLine(), idLoc->getColumn(), "Variable '" + id + "' is not defined");
                                        }
                                        return getVariableValue(id, idLoc->getLine(), idLoc->getColumn()).getType();
                                    } else if (locFactor->getLocationType() == LocationType::LIST_ELEM) {
                                        ListElementLocation* listElemLoc = static_cast<ListElementLocation*>(locFactor);
                                        std::string id = listElemLoc->getId();
                                        if (!isListDefined(id)) {
                                            throw SemanticError(listElemLoc->getLine(), listElemLoc->getColumn(), "List '" + id + "' is not defined");
                                        }
                                        return symbolTable_.getListElement(id, eval(listElemLoc->getIndex())->getIntValue()).getType();
                                    } else {
                                        return Types::TYPE_UNDEFINED;
                                    }
                                } else {
                                    return Types::TYPE_UNDEFINED;
                                } 
                            } else {
                                return Types::TYPE_UNDEFINED;
                            }
                        } else {
                            return Types::TYPE_UNDEFINED;
                        }
                    } else {
                        return Types::TYPE_UNDEFINED;
                    }
                } else {
                    return Types::TYPE_UNDEFINED;
                }
            } else {
                return Types::TYPE_UNDEFINED;
            }
        } else {
            return Types::TYPE_UNDEFINED;
        }
    } else {
        return Types::TYPE_UNDEFINED;
    }
}  