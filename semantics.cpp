/**
 * @file semantics.cpp
 * @brief Implements the Semantics component of the Python-Sublanguage interpreter
 * 
 * This file contains the implementation of the Semantics class, which is responsible for 
 * performing semantic analysis on the Syntax Tree.
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */

#include "semantics.h"
#include "error.h"

EvaluatedElement::EvaluatedElement(int value){
    type_ = TYPE_INT;
    intValue_ = value;
}

EvaluatedElement::EvaluatedElement(bool value){
    type_ = TYPE_BOOL;
    boolValue_ = value;
}

int EvaluatedElement::getIntValue() {
    if (type_ != TYPE_INT) {
        throw InternalError(0, 0, "Attempt to get int value from non-int EvaluatedElement");
    }
    return intValue_;
}

bool EvaluatedElement::getBoolValue() {
    if (type_ != TYPE_BOOL) {
        throw InternalError(0, 0, "Attempt to get bool value from non-bool EvaluatedElement");
    }
    return boolValue_;
}

Types EvaluatedElement::getType() {
    return type_;
}

void EvaluatedElement::setIntValue(int value) {
    intValue_ = value;
    type_ = TYPE_INT;
}

void EvaluatedElement::setBoolValue(bool value) {
    boolValue_ = value;
    type_ = TYPE_BOOL;
}


bool SymbolTable::isVariableDefined(const std::string& id) {
    // Compare the id with the keys of both maps and return true if found (if find() does not return end())
    return 
    (intVariables_.find(id) != intVariables_.end()) || 
    (boolVariables_.find(id) != boolVariables_.end());
}

void SymbolTable::addVariable(const std::string& id, int element) {
    // Check if the viariable is already defined as int or bool
    if (isVariableDefined(id)) {
        throw InternalError(0, 0, "Variable " + id + " is already defined");
    }
    // Create a new int and add it to the map
    int* newInt = new int(element);
    intVariables_[id] = newInt;
}

void SymbolTable::addVariable(const std::string& id, bool element) {
    // Check if the viariable is already defined as int or bool
    if (isVariableDefined(id)) {
        throw InternalError(0, 0, "Variable " + id + " is already defined");
    }
    // Create a new bool and add it to the map
    bool* newBool = new bool(element);
    boolVariables_[id] = newBool;
}

void SymbolTable::updateVariable(const std::string& id, int element) {
    // Check if the variable is already defined
    if (intVariables_.find(id) != intVariables_.end()) {
        // if it is an int, update its value
        *(intVariables_[id]) = element;
    } else if (boolVariables_.find(id) != boolVariables_.end()) {
        // if it is a bool, delete it and define it as int
        delete boolVariables_[id];
        boolVariables_.erase(id);
        int* newInt = new int(element);
        intVariables_[id] = newInt;
    } else {
        throw InternalError(0, 0, "Variable " + id + " is not defined");
    }
}

void SymbolTable::updateVariable(const std::string& id, bool element) {
    // Check if the variable is already defined
    if (boolVariables_.find(id) != boolVariables_.end()) {
        // if it is a bool, update its value
        *(boolVariables_[id]) = element;
    } else if (intVariables_.find(id) != intVariables_.end()) {
        // if it is an int, delete it and define it as bool
        delete intVariables_[id];
        intVariables_.erase(id);
        bool* newBool = new bool(element);
        boolVariables_[id] = newBool;
    } else {
        throw InternalError(0, 0, "Variable " + id + " is not defined");
    }
}

EvaluatedElement SymbolTable::getVariableValue(const std::string& id) const {
    if (intVariables_.find(id) != intVariables_.end()) {
        return EvaluatedElement(*(intVariables_.at(id)));
    } else if (boolVariables_.find(id) != boolVariables_.end()) {
        return EvaluatedElement(*(boolVariables_.at(id)));
    } else {
        throw InternalError(0, 0, "Variable " + id + " is not defined");
        return EvaluatedElement(0); // to avoid compiler warning, will never be reached
    }
}

bool SymbolTable::isListDefined(const std::string& id) const {
    // Compare the id with the keys of the lists map and return true if found (if find() does not return end())
    return lists_.find(id) != lists_.end();
}

void SymbolTable::addList(const std::string& id) {
    // Check if the list is already defined, if it is, do nothing
    if (isListDefined(id)) {
        return;
    }
    // Create a new vector of EvaluatedElement pointers and add it to the map
    std::vector<EvaluatedElement*> newList;
    lists_[id] = newList;
}

void SymbolTable::appendToList(const std::string& id, EvaluatedElement element) {
    // Check if the list is defined
    if(!isListDefined(id)) {
        throw InternalError(0, 0, "List " + id + " is not defined");
    }
    // Create a new EvaluatedElement and append it to the list
    EvaluatedElement* newElement = new EvaluatedElement(element);
    lists_[id].push_back(newElement);
}

void SymbolTable::updateListElement(const std::string& id, int index, EvaluatedElement element) {
    // Check if the list is defined
    if(!isListDefined(id)) {
        throw InternalError(0, 0, "List " + id + " is not defined");
    }
    // Check if the index is within bounds
    if(index < 0 || index >= lists_[id].size()) {
        throw InternalError(0, 0, "List index out of range");
    }
    // Update the element at the specified index
    *(lists_[id][index]) = element;
}

EvaluatedElement SymbolTable::getListElement(const std::string& id, int index) const {
    // Check if the list is defined
    if(!isListDefined(id)) {
        throw InternalError(0, 0, "List " + id + " is not defined");
    }
    // Check if the index is within bounds
    if(index < 0 || index >= lists_.at(id).size()) {
        throw InternalError(0, 0, "List index out of range");
    }
    // Return the element at the specified index
    return *(lists_.at(id)[index]);
}

int SymbolTable::getListSize(const std::string& id) {
    // Check if the list is defined
    if(!isListDefined(id)) {
        throw InternalError(0, 0, "List " + id + " is not defined");
    }
    // Return the size of the list
    return lists_.at(id).size();
}

void SymbolTable::clear(const std::string& id) {
    // Check if the list is defined
    if(!isListDefined(id)) {
        throw InternalError(0, 0, "List " + id + " is not defined");
    }
    // Delete each element in the vector
    for(auto element : lists_.at(id)) {
        delete element;
    }
    // Clear the vector
    lists_[id].clear();
    // Remove the list from the map
    lists_.erase(id);
}