#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <map>
#include "error.hpp"
#include "lexer.hpp"

class Interpreter{
private:
    std::string file_name;
    std::vector<std::string> file_content;
    Error error;

    void get_file_content(){
        try{
            std::ifstream target_file(file_name);
            std::string line;
            while (std::getline(target_file, line)){
                file_content.push_back(line);
            }
        }
        catch (...){
            std::cout << "Provided file does not exist!!!" << "\n";
            exit(1);
        }
    }

    void show_end_state(const std::unordered_map<int, int> init_state, const std::unordered_map<int, int> end_state){
        std::map<int, int> init_state_sorted(init_state.begin(), init_state.end());
        std::cout << "Init State: \n" << std::endl;
        for (const std::pair<int, int> var : init_state_sorted){
            std::cout << var.first << ": " << var.second << std::endl;
        }

        std::map<int, int> end_state_sorted(end_state.begin(), end_state.end());
        std::cout << "\nEnd State: \n" << std::endl;
        for (const std::pair<int, int> var : end_state_sorted){
            std::cout << var.first << ": " << var.second << std::endl;
        }
    }

    std::unordered_map<int, int> make_end_state(std::tuple<std::vector<Token>, std::unordered_map<int, int> > init_state){
        std::vector<Token> tokens = std::get<0>(init_state);
        std::unordered_map<int, int> vars = std::get<1>(init_state);
        Token cur_token = tokens[0];
        int cur_pos = 0;
        int steps = 0;

        while (cur_token.get_type() != TT_HLT){
            if (steps > 1000){
                error = Error("RecursionError", "Infinite recursion loop in given program", cur_token.get_ln(), file_name, false);
                return {};
            }

            std::tuple<int, int, bool> instruction_res = process_instruction(cur_token, cur_pos, vars);

            if (std::get<2>(instruction_res)){
                vars[cur_token.get_value()] = std::get<1>(instruction_res);
                cur_pos++;
            }
            else{
                cur_pos = std::get<0>(instruction_res);
            }
            if (cur_pos >= tokens.size()){
                error = Error("Fatal Error", "Instruction target out of range", cur_token.get_ln(), file_name, false);
                return {};
            }
            steps++;
            cur_token = tokens[cur_pos];
        }

        return vars;
    }

    std::tuple<int, int, bool> process_instruction(Token token, int pos, std::unordered_map<int, int>& vars){
        switch (token.get_type()){
            case TT_INC: return {-1, vars[token.get_value()]+1, true};
            case TT_DEC: return {-1, vars[token.get_value()]-1, true};
            case TT_JMP: return {token.get_value()-1, -1, false};
            case TT_TST:
                if (vars[token.get_value()] == 0){
                    return {pos+2, -1, false};
                }
                else{
                    return {pos+1, -1, false};
                }
        }
        
        return {0, 0, false};
    }

public:
    Interpreter(const std::string new_file_name){
        file_name = new_file_name;
        get_file_content();
    }

    int interpret(){
        Lexer lexer = Lexer(file_name, file_content);
        std::tuple<std::vector<Token>, std::unordered_map<int, int> > init_state = lexer.make_init();
        if (lexer.has_error()){
            std::cout << lexer.get_error().repr() << "\n";
            return 1;
        }

        std::unordered_map<int, int> end_state = make_end_state(init_state);

        if (!error.get_empty()){
            std::cout << error.repr() << std::endl;
            return 1;
        }

        show_end_state(std::get<1>(init_state), end_state);

        return 0;
    }
};