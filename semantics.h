#if !defined(SEMANTICS_H)
#define SEMANTICS_H

#include <map>
#include <string>
#include "syntax.h"
#include "error.h"
#include "types.h"

/**
 * @file semantics.h
 * @brief Defines the Semantics component of the Python-Sublanguage interpreter
 * 
 * This file contains the declaration of the Semantics class, which is responsible for 
 * performing semantic analysis on the Syntax Tree.
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */

/**
 * @class EvaluatedElement
 * @brief Represents an evaluated element in the semantic analysis
 */
class EvaluatedElement{
    public:
        // Constructors
        EvaluatedElement() = delete;
        EvaluatedElement(int value);
        EvaluatedElement(bool value);
        EvaluatedElement(EvaluatedElement const& other) = default;

        // Destructor
        ~EvaluatedElement() = default;

        // Methods
        int getIntValue();
        bool getBoolValue();
        Types getType();
        void setIntValue(int value);
        void setBoolValue(bool value);
        void setType(Types type);

    private:
        Types type_; // Type of the evaluated element
        int intValue_; // Integer value (if type is TYPE_INT)
        bool boolValue_; // Boolean value (if type is TYPE_BOOL)
};

/**
 * @class SymbolTable
 * @brief Represents a symbol table for semantic analysis
 */
class SymbolTable {
    public:
        // Constructors
        SymbolTable() = default;

        // Destructor
        ~SymbolTable() = default;

        // Methods for variable management
        bool isVariableDefined(const std::string& id);
        void addVariable(const std::string& id, int element);
        void addVariable(const std::string& id, bool element);
        void updateVariable(const std::string& id, int element);
        void updateVariable(const std::string& id, bool element);
        EvaluatedElement getVariableValue(const std::string& id) const;

        // Methods for list management
        bool isListDefined(const std::string& id) const;
        void addList(const std::string& id);
        void appendToList(const std::string& id, EvaluatedElement element);
        void updateListElement(const std::string& id, int index, EvaluatedElement element);
        EvaluatedElement getListElement(const std::string& id, int index) const;
        int getListSize(const std::string& id);
        void clear(const std::string& id);


    private:
        // Int Variables => pointer to int
        std::map<std::string, int*> intVariables_;

        // Bool Variables => pointer to bool
        std::map<std::string, bool*> boolVariables_;

        // Lists => vector of pointers to EvaluatedElement
        std::map<std::string, std::vector<EvaluatedElement*>> lists_;
};


#endif