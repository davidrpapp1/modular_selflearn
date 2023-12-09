// Libraries
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>

// Header files
#include "../hdr/hash_table.hpp"
#include "../hdr/tokenisation.hpp"
#include "../hdr/raw_hash_table.hpp"

std::vector <std::string> tokenise(std::string query_upper){

    // Reset token vector for new query
    token.clear();

    // Remove potential inconsistencies from input string
    query_upper.erase(remove(query_upper.begin(), query_upper.end(), '?'), query_upper.end());
    query_upper.erase(remove(query_upper.begin(), query_upper.end(), ','), query_upper.end());
    query_upper.erase(remove(query_upper.begin(), query_upper.end(), '.'), query_upper.end());

    // Clear indication tokens for self learning protocol
    std::string left_string = "[[";
    std::string right_string = "]]";
    
    // Left indicator
    std::string::size_type left_indicator = query_upper.find(left_string);
    if (left_indicator != std::string::npos) query_upper.erase(left_indicator, left_string.length());

    // Right indicator
    std::string::size_type right_indicator = query_upper.find(right_string);
    if (right_indicator != std::string::npos) query_upper.erase(right_indicator, right_string.length());

    // Intermediate tokenisation container
    std::stringstream tokenisation(query_upper);
    std::string intermediate;

    // Tokenise and vectorise string
    while(getline(tokenisation, intermediate, ' ')){
        token.push_back(intermediate);
    }

    return token;

}

std::vector <std::string> parse_tokenisation(std::vector <std::string> token){

    // Reset type postition vector and completion flag
    type_position.clear();
    bool type_pos_complete_flag = false;

    // Token recognition check
    bool token_recognized_flag;

    // Look for string vector match
    for(int i=0; i<token.size(); i++){

        // Reset token recognition boolean
        token_recognized_flag = false;
                    
        // Comparison with rank 3 factual type functions
        for(int n=0; n<types::factual_type.size(); n++){

            if(token[i]==types::factual_type[n]){
                            
                type_position.push_back("f"); // Factual
                type_pos_complete_flag = true; // Type position complete flag
                token_recognized_flag = true; // Flag to identify token recognition as contextual

            }

        }

        // Comparison with rank 3 connector type functions
        for(int n=0; n<types::connector_type.size(); n++){

            if(token[i]==types::connector_type[n]){
                            
                type_position.push_back("c"); // Connector
                type_pos_complete_flag = true;
                token_recognized_flag = true; 

            }

        }

        // Comparison with rank 3 subject type functions
        for(int n=0; n<types::subject_type.size(); n++){

            if(token[i]==types::subject_type[n]){
                            
                type_position.push_back("s"); // Subject
                type_pos_complete_flag = true;
                token_recognized_flag = true;

            }

        }

        // Comparison with contextual type functions
        for(int n=0; n<types::contextual_type.size(); n++){

            if(token[i]==types::contextual_type[n]){
                            
                if(n==0) types::your_flag = true; // Your

                if(n==1) types::this_flag = true; // This

                if(n==2) types::the_flag = true; // The
                
                if(n==3) types::it_flag = true; // It

                if(n==4) types::current_flag = true; // Current

                if(n==5) types::currently_flag = true; // Currently

                if(n==6) types::today_flag = true; // Today

                if(n==7) types::todays_flag = true; // Today's

                if(n==8) types::now_flag = true; // Now

                if(n==9) types::yourself_flag = true; // Yourself

                if(n==10) types::you_flag = true; // You

                // Special case with "you" and "yourself" in statements
                if(types::yourself_flag == true && types::you_flag == true){
                    types::you_flag = false;
                }

                // Flag to identify token recognition as contextual
                token_recognized_flag = true;
            }

        }

        // If token not matched to anything in dictionary, return ? in that token slot  
        if(token_recognized_flag == false){
            type_pos_complete_flag = true;
            type_position.push_back("?");
        }
    }

    // If type position not detected, return error message via a key
    if (type_pos_complete_flag==true){
        return type_position;
    } else{

        // UPDATE THIS ERROR FORMATTING (Not sure about this?)
        std::vector <std::string> error_string{"query_error"};
        return error_string;
    }

}

std::vector <int> unknown_token_positions;
std::vector <std::string> fetch_unknown_tokens(std::vector <std::string> token){

    // Clear unknown tokens between uses
    unknown_tokens.clear();
    unknown_token_positions.clear();

    bool token_recognized_flag;

    // Look for string vector match
    for(int i=0; i<token.size(); i++){
        
        // Reset token recognized flag
        token_recognized_flag = false;

        // Comparison with rank 3 factual type functions
        for(int n=0; n<types::factual_type.size(); n++){

            if(token[i]==types::factual_type[n]) token_recognized_flag = true;

        }

        // Comparison with rank 3 connector type functions
        for(int n=0; n<types::connector_type.size(); n++){

            if(token[i]==types::connector_type[n]) token_recognized_flag = true;

        }

        // Comparison with rank 3 subject type functions
        for(int n=0; n<types::subject_type.size(); n++){

            if(token[i]==types::subject_type[n]) token_recognized_flag = true;

        }

        // Comparison with contextual type functions
        for(int n=0; n<types::contextual_type.size(); n++){

            if(token[i]==types::contextual_type[n]) token_recognized_flag = true;

        }

        // If token not matched to anything in dictionary, return ? in that token slot  
        if(token_recognized_flag == false) {
            unknown_tokens.push_back(token[i]);
            unknown_token_positions.push_back(i);
        }

    }

    return unknown_tokens;

}