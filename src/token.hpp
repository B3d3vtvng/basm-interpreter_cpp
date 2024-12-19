#pragma once

#include <iostream>

enum TOKENTYPES {
    TT_INC,
    TT_DEC,
    TT_JMP,
    TT_TST,
    TT_HLT
};

using token_t = TOKENTYPES;

class Token{
private:
    token_t token_type;
    int token_value;
    int token_ln;

public:
    Token(const token_t new_token_type, const int new_token_value, const int new_token_ln){
        token_type = new_token_type;
        token_value = new_token_value;
        token_ln = new_token_ln;
    }

    std::string repr() const{
        if (token_type == TT_INC){
            return "TT_inc: " + std::to_string(token_value);
        }
        else if (token_type == TT_DEC){
            return "TT_dec: " + std::to_string(token_value);
        }
        else if (token_type == TT_JMP){
            return "TT_jmp: " + std::to_string(token_value);
        }
        else if (token_type == TT_TST){
            return "TT_tst: " + std::to_string(token_value);
        }
        else{
            return "TT_hlt: None";
        }
    }

    token_t get_type(){
        return token_type;
    }

    int get_value(){
        return token_value;
    }

    int get_ln(){
        return token_ln;
    }
};