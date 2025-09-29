/**
 * @file main.h
 * @brief Entry point of the Pyhon-Sublanguage interpreter
 * 
 * Opens the input file
 * Runs the tokenizer
 * 
 * @author Pietro Malerba (S5839759) 
 * @date 08-2025
 */

#include <iostream>
#include <fstream>
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "visitor.h"
#include "syntax.h"
#include "semantics.h"
#include "types.h"

int main(int argc, char* argv[]) {
    // Check for input arguments
    if(argc < 2){
        error(MissingFileError(0, 0, "No input file provided"));
    }

    // Try to open input file
    std::ifstream inputFile;
    inputFile.open(argv[1]);
    
    // Check if file is open
    if(!inputFile.is_open()){
        error(FileOpenError(0, 0, "Could not open input file: " + std::string(argv[1])));
    }

    // Initialize the lexer
    Lexer lexer(inputFile);
    // Initialize the vector and run the lexer
    std::vector<Token*> tokens;
    try{
        tokens = lexer();
    } catch(const Error& e){
        error(e);
    }

    // Close the input file
    inputFile.close();

    
    // Initialize the parser
    Parser parser(tokens);
    // Initialize the syntax tree and run the parser
    Program* program;
    try{
        program = parser();
    } catch(const Error& e){
        error(e);
    }
    
    // Initialize the visitor
    Visitor visitor(program);
    // Run the visitor
    try{
        visitor();
    } catch(const Error& e){
        error(e);
    }

    // Cleanup the tokens
    for(auto t : tokens) {
        delete t;
    }
    tokens.clear();
    
    // Clean up the syntax tree
    delete program;

    
    // Return success
    return EXIT_SUCCESS;
}