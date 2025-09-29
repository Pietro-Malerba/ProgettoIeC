#if !defined(ERROR_H)
#define ERROR_H

/**
 * @file error.h
 * @brief Defines error handling functions for the Python-Sublanguage interpreter
 * 
 * This file contains the declaration of error handling functions, which are responsible for
 * reporting errors encountered during the interpretation process.
 * 
 * @author Pietro Malerba (S5839759)
 * @date 08-2025
 */

/**
 * Enumeration of error codes
 */
enum ErrorCode {
    MISSING_FILE_ERROR,
    FILE_OPEN_ERROR,
    INDENTATION_ERROR,
    LEXICAL_ERROR,
    RESERVED_KEYWORD_ERROR,
    SYNTAX_ERROR,
    INTERNAL_ERROR,
    SEMANTIC_ERROR,
    INDEX_ERROR,
    EVALUATION_ERROR,
    ZERO_DIVISION,
    TYPE_ERROR
};

/**
 * @class Error
 * @brief Error handling class for the Python-Sublanguage interpreter
 */
class Error : public std::exception {
    public:
        // constructors
        Error() = delete;
        Error(int line, int column, int error_code, const std::string& message = "") 
            : line_{line}, column_{column}, error_code_{error_code}, message_{message} {}
        Error(const Error& e) = default;

        // destructor
        ~Error() = default;

        // methods
        int getLine() const { return line_; }
        int getColumn() const { return column_; }
        int getErrorCode() const { return error_code_; }

        // override what() method from std::exception
        const char* what() const noexcept override {
            return message_.c_str();
        }

    private:
        int line_;
        int column_;
        int error_code_;
        std::string message_;
};

/**
 * @class MissingFileError
 * @brief Error class for missing input file
 */
class MissingFileError : public Error {
    public:
        MissingFileError(int line, int column, const std::string& message = "")
            : Error(line, column, MISSING_FILE_ERROR, message) {}
};

/**
 * @class FileOpenError
 * @brief Error class for file opening errors
 */
class FileOpenError : public Error {
    public:
        FileOpenError(int line, int column, const std::string& message = "")
            : Error(line, column, FILE_OPEN_ERROR, message) {}
};

/**
 * @class IndentationError
 * @brief Error class for indentation errors
 */
class IndentationError : public Error {
    public:
        IndentationError(int line, int column, const std::string& message = "")
            : Error(line, column, INDENTATION_ERROR, message) {}
};

/**
 * @class LexicalError
 * @brief Error class for lexical errors
 */
class LexicalError : public Error {
    public:
        LexicalError(int line, int column, const std::string& message = "")
            : Error(line, column, LEXICAL_ERROR, message) {}
};

/**
 * @class ReservedKeywordError
 * @brief Error class for reserved keyword errors
 */
class ReservedKeywordError : public Error {
    public:
        ReservedKeywordError(int line, int column, const std::string& message = "")
            : Error(line, column, RESERVED_KEYWORD_ERROR, message) {}
};

/**
 * @class SyntaxError
 * @brief Error class for syntax errors
 */
class SyntaxError : public Error {
    public:
        SyntaxError(int line, int column, const std::string& message = "")
            : Error(line, column, SYNTAX_ERROR, message) {}
};

/**
 * @class InternalError
 * @brief Error class for internal errors
 */
class InternalError : public Error {
    public:
        InternalError(int line, int column, const std::string& message = "")
            : Error(line, column, INTERNAL_ERROR, message) {}
};

/**
 * @class SemanticError
 * @brief Error class for semantic errors
 */
class SemanticError : public Error {
    public:
        SemanticError(int line, int column, const std::string& message = "")
            : Error(line, column, SEMANTIC_ERROR, message) {}
};

/**
 * @class IndexError
 * @brief Error class for index errors
 */
class IndexError : public Error {
    public:
        IndexError(int line, int column, const std::string& message = "")
            : Error(line, column, INDEX_ERROR, message) {}
};

/**
 * @class EvaluationError
 * @brief Error class for evaluation errors
 */
class EvaluationError : public Error {
    public:
        EvaluationError(int line, int column, const std::string& message = "")
            : Error(line, column, EVALUATION_ERROR, message) {}
};

/**
 * @class ZeroDivisionError
 * @brief Error class for division by zero errors
 */
class ZeroDivisionError : public Error {
    public:
        ZeroDivisionError(int line, int column, const std::string& message = "")
            : Error(line, column, ZERO_DIVISION, message) {}
};

/**
 * @class TypeError
 * @brief Error class for type errors
 */
class TypeError : public Error {
    public:
        TypeError(int line, int column, const std::string& message = "")
            : Error(line, column, TYPE_ERROR, message) {}
};

/**
 * Outputs an error message to stderr and exits the program
 * @param e The Error object containing error details
 */
void error(const Error& e);

/**
 * Returns a string representation of the error code
 * @param error_code The error code to be converted to string
 * @return A string representing the error code
 */
std::string ErrorName(int error_code);

#endif