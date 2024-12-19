#pragma once

#include <iostream>

class Error{
private:
    std::string error_type;
    std::string error_msg;
    int error_ln;
    std::string error_file;
    bool empty;

public:
    Error()
        : error_type(""), error_msg(""), error_ln(-1), error_file(""), empty(true) {}
    Error(const std::string& new_error_type, const std::string& new_error_msg, int new_error_ln, const std::string& new_error_file, bool is_empty)
        : error_type(new_error_type), error_msg(new_error_msg), error_ln(new_error_ln), error_file(new_error_file), empty(is_empty) {}


    std::string repr(){
        return error_file + ":" + std::to_string(error_ln) + ": " + error_type + ": " + error_msg;
    }

    bool get_empty(){
        return empty;
    }
};