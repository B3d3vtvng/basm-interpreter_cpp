#include "src/interpreter.hpp"

int main(int argc, char** argv){
    if(argc < 2){
        std::cout << "Fatal Error: No file provided!!!" << "\n";
        exit(1);
    }
    std::string file_name = argv[1];
    Interpreter interpreter = Interpreter(file_name);
    int exit_code = interpreter.interpret();

    return exit_code;
}