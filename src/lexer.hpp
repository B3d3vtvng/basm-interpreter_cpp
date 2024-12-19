#pragma once

#include <vector>
#include <iostream>
#include <cctype>
#include <unordered_map>
#include <tuple>
#include "error.hpp"
#include "token.hpp"

std::string strip(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();

    while (start < end && std::isspace(static_cast<unsigned char>(str[start]))) {
        start++;
    }

    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        end--;
    }

    return str.substr(start, end - start);
}

std::vector<std::string> TOKENS = {"inc", "dec", "jmp", "tst", "hlt"};

class Lexer{
private:
    Error error;
    std::vector<std::string> raw_code;
    std::string file_name;

    Token make_line_token(std::string line, const int line_num){
        std::string opcode = get_token_opcode(line, line_num);
        if (!error.get_empty()){
            return Token(TT_HLT, -1, -1);
        }
        token_t token_type = get_token_type(opcode);
        int value;
        if (token_type != TT_HLT){
            value = get_token_value(line, line_num, token_type);
            if (!error.get_empty()){
                return Token(TT_HLT, -1, -1);
            }
        }
        else{
            value = -1;
        }

        return Token(token_type, value, line_num);
    }

    int get_token_value(std::string line, const int line_num, const token_t token_type){
        line = strip(line);
        line = line.substr(3, line.length()-3);
        line = strip(line);
        int value;
        try{
            value = std::stoi(line);
        }
        catch (...){
            error = Error("TypeError", "Invalid operand type", line_num, file_name, false);
            return -1;
        }
        return value;
    }

    token_t get_token_type(const std::string opcode) const{
        if (opcode == "inc"){
            return TT_INC;
        }
        else if (opcode == "dec"){
            return TT_DEC;
        }
        else if (opcode == "jmp"){
            return TT_JMP;
        }
        else if (opcode == "tst"){
            return TT_TST;
        }
        else{
            return TT_HLT;
        }
    }

    std::string get_token_opcode(std::string line, const int line_num){
        line = strip(line);
        std::string opcode = "";
        for (int i = 0; i<3; i++){
            opcode += line[i];
        }
        std::string token_type = "";
        for (const std::string possible_token_type : TOKENS){
            if (opcode == possible_token_type){
                token_type = opcode;
            }
        }
        if (token_type == ""){
            error = Error("Syntax Error", "Invalid Opcode", line_num, file_name, false);
            return "";
        }

        return opcode;
    }

    int get_section_idx(){
        for (int i = 0;i<raw_code.size();i++){
            if (strip(raw_code[i]) == "section .data"){
                return i;
            }
        }
        return -1;
    }

    std::vector<Token> make_tokens(const std::vector<std::string> lines){
        std::vector<Token> tokens;

        for (int i = 0; i<lines.size();i++){
            if (strip(lines[i]) == ""){
                continue;
            }
            Token line_token = make_line_token(lines[i], i+1);
            if (!error.get_empty()){
                return {};
            }
            tokens.push_back(line_token);
        }
        return tokens;
    }

    int safe_stoi(const std::string left, int ln_idx){
        int var_idx;

        try{
            var_idx = std::stoi(strip(left));
        }
        catch (...){
            error = Error("TypeError", "Invalid Type in variable declaration", ln_idx+1, file_name, false);
            return -1;
        }

        return var_idx;
    }

    std::tuple<int, int> make_var(std::string line, int ln_idx){
        line = strip(line);

        int colon_idx;

        for (int i = 0; i<line.length();i++){
            if (line[i] == ':'){
                colon_idx = i;
                break;
            }
        }

        int var_idx = safe_stoi(std::string(line.begin(), line.begin()+colon_idx), ln_idx);
        int var_val = safe_stoi(std::string(line.begin()+colon_idx+1, line.end()), ln_idx);

        if (!error.get_empty()){
            return {};
        }

        return {var_idx, var_val};
    }

    std::unordered_map<int, int> make_vars(const std::vector<std::string> lines, int ln_adjust){
        std::unordered_map<int, int> vars;

        for (int i = 0; i<lines.size();i++){
            if (strip(lines[i]) == ""){
                continue;
            }
            std::tuple<int, int> var = make_var(lines[i], i+ln_adjust);
            if (!error.get_empty()){
                return {};
            }
            vars[std::get<0>(var)] = std::get<1>(var);
        }

        return vars;
    }

public:
    Lexer(const std::string new_file_name, const std::vector<std::string> new_file_content)
        : file_name(new_file_name), raw_code(new_file_content) {}

    std::tuple<std::vector<Token>, std::unordered_map<int, int> > make_init(){
        int section_idx = get_section_idx();
        if (section_idx == -1){
            error = Error("Fatal Error", "File does not contain data section", -1, file_name, false);
            return {};
        }

        std::vector<Token> tokens;
        std::unordered_map<int, int> vars;

        std::vector<std::string> text_section(raw_code.begin(), raw_code.begin()+section_idx);
        std::vector<std::string> data_section(raw_code.begin()+section_idx+1, raw_code.end());

        tokens = make_tokens(text_section);
        vars = make_vars(data_section, section_idx+1);

        if (!error.get_empty()){
            return {};
        }

        return {tokens, vars};

        
    }

    bool has_error(){
        return !error.get_empty();
    }

    Error get_error(){
        return error;
    }
};