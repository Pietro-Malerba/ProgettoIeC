#if !defined(TYPES_H)
#define TYPES_H

/**
 * @file types.h
 * @brief Defines the types used in the Python-Sublanguage interpreter
 *
 * This file contains the definition of the Types enum, which represents the different data types
 * used in the interpreter.
 * 
 * Types have been separated in their own file to avoid circular dependencies between syntax.h and semantics.h
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */



/**
 * @enum Types
 * @brief Represents the different data types in the Python-Sublanguage interpreter
 */
enum Types {
    TYPE_BOOL,
    TYPE_INT,
    TYPE_UNDEFINED
};

#endif 