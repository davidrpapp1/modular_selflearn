#ifndef TOKENISATION_H
#define TOKENISATION_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Error variables
extern bool type_pos_complete_flag;

// Tokenisation containers
extern std::string query_upper;
extern std::vector <std::string> type_position;
extern std::vector <std::string> token;
extern std::vector <std::string> unknown_tokens;
extern std::vector <int> unknown_token_positions;

// Tokenisation functions
extern std::vector <std::string> parse_tokenisation(std::vector <std::string> token);
extern std::vector <std::string> tokenise(std::string query_upper);
extern std::vector <std::string> fetch_unknown_tokens(std::vector <std::string> token);

#endif