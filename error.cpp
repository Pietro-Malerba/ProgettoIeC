/**
 * @file error.cpp
 * @brief Implements error handling functions for the Python-Sublanguage interpreter
 * 
 * This file contains functions to output errors encountered during
 * the execution of the interpreter.
 * 
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */

#include <iostream>
#include "error.h"

/**
 * Outputs an error message to stderr
 * @param e The Error object containing error details
 */
void error(const Error& e) {
    std::cerr << "Error: " << ErrorName(e.getErrorCode()) << " [" << e.getLine() << ":" << e.getColumn() << "] - " << e.what() << std::endl;
    exit(EXIT_FAILURE);
}

/**
 * Returns a string representation of the error code
 * @param error_code The error code to be converted to string
 * @return A string representing the error code
 */
std::string ErrorName(int error_code) {
    switch (error_code) {
        case MISSING_FILE_ERROR: return "MISSING_FILE_ERROR";
        case FILE_OPEN_ERROR: return "FILE_OPEN_ERROR";
        case INDENTATION_ERROR: return "INDENTATION_ERROR";
        case LEXICAL_ERROR: return "LEXICAL_ERROR";
        case RESERVED_KEYWORD_ERROR: return "RESERVED_KEYWORD_ERROR";
        case SYNTAX_ERROR: return "SYNTAX_ERROR";
        case INTERNAL_ERROR: return "INTERNAL_ERROR";
        case SEMANTIC_ERROR: return "SEMANTIC_ERROR";
        case INDEX_ERROR: return "INDEX_ERROR";
        case EVALUATION_ERROR: return "EVALUATION_ERROR";
        case ZERO_DIVISION: return "ZERO_DIVISION";
        case TYPE_ERROR: return "TYPE_ERROR";
        default: return "UNKNOWN_ERROR";
    }
}