// Libraries
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <bits/stdc++.h>
#include <ctime>
#include <chrono>
#include <iomanip>

// Header files
#include "../hdr/err.hpp"
#include "../hdr/terminal.hpp"
#include "../hdr/raw_hash_table.hpp"
#include "../hdr/hash_table.hpp"
#include "../hdr/tokenisation.hpp"
#include "../hdr/comparator.hpp"
#include "../hdr/if_multiple_processor.hpp"

// Self learning variables
bool self_learning_flag;
bool savestate = false;
bool unknown_recognition_failed = false;
std::vector <std::string> unknown_tokens;
std::vector <std::string> cached_unknown_tokens;
std::vector <std::string> model_answers;
std::string unknown_token_type;

// Hash table declarations
std::vector <std::string> simple_query{"HELLO", "HI", "HI CITHRIA", "HELLO CITHRIA", "HI, CITHRIA", "HELLO, CITHRIA"};

std::vector <std::string> debug_table{"DISPLAY ERROR STATUS", "ERROR STATUS", "CITHRIA, DISPLAY ERROR STATUS", 
                            "CITHRIA DISPLAY ERROR STATUS", "CITHRIA, ERROR STATUS", "CITHRIA ERROR STATUS"};

// Word type categories
namespace types{

    std::vector <std::string> factual_type{"HOW", "WHAT", "WHO", "WHEN", "WHERE"}; // e407a35cb26b5f8cba43b38d9aaeb2d894da09135792ca375878751b0292ac0e636ce325d9462fa5e2e290ce4dd6542948ff977bb8298265cef4f02f386f5497

    std::vector <std::string> connector_type{"IS", "ARE", "THE"}; // adb7b520563433f3c894d3b1b5783d74fe05eaa7eeae1a892d08bc84d9271bd8b1e39aaf6692229c7774a23006bda4f0be5730110f91635516f25f950e6a1b2e

    std::vector <std::string> contextual_type{"YOUR", "THIS", "THE", "IT", "CURRENT", "CURRENTLY", "TODAY", "TODAY'S", "NOW", "YOURSELF", "YOU"}; // cf67d8a82a5b45041edd17959e2cf4f5152ecf7c6fb4914b75877e5f95484146ff972dd24113db14d360accfde7346b1f7ecbfb1772bedf6246437cf3a5ec7ac

    // Contextual flags
    bool your_flag;
    bool this_flag;
    bool the_flag;
    bool it_flag;
    bool current_flag;
    bool currently_flag;
    bool today_flag;
    bool todays_flag;
    bool now_flag;
    bool yourself_flag;
    bool you_flag;

    std::vector <std::string> subject_type{"YOU", "IT", "THAT", "DAY", "LIFE", "EXPERIENCE", "TIME", "DATE", "TODAY", "TODAY'S", "CURRENT", "CURRENTLY", "NOW"}; // 7d64f4929e21e8c48cbb48ab2b21f6b2a7579de588fd3b5adac9a4685bc647269b61b72045030681595edcdf93e80b88dcb8e125c21124b4844ae6b6192527e7

} // End of namespace types

// Rank 3 responses
namespace r3_responses{

    // 1
    std::vector <std::string> fcs_how_you{"Good, although it is unlikely I have feelings...", "Good, I think?", 
                                          "I am doing well thank you.", "Thank you for asking, I am doing good.",
                                          "Nice of you to ask, I am well thank you."};

    // 2
    std::vector <std::string> fcs_what_you{"I am a personal assistant designed around contextual understanding.",
                                           "I am an artificial intelligence capable of contextual understanding.",
                                           "I was made to be an artificial intelligence designed around contextual understanding.",
                                           "My creators designed me as a personal assistant with contextual understanding."};

    // 3
    std::vector <std::string> fcs_who_you{"My name is Cithria, I am an artificial intelligence.",
                                          "I am Cithria, an artificial intelligence.",
                                          "My name is Cithria, I am a personal assistant with artificial intelligence."};

    // 4
    std::vector <std::string> fcs_where_you{"I am inside the computer, without physical form.",
                                            "Inside the computer, I do not have a physical form.",
                                            "I exist inside the computer without a physical form.",
                                            "Unlike you I do not have a physical form, as I exist inside the computer."};

    // 5
    std::vector <std::string> fcs_when_you{"I am now, I am forever.", "I exist always in time, in the form of machine code.",
                                               "I am when now is.", "I am now."};

    // 6
    std::vector <std::string> fcs_how_it{"It is good.", "How is what?", "It is going well, thank you for asking."};
    int fcs_how_it_cont_flag=0;

} // End of namespace r3_responses

// Rank 4 responses
namespace r4_responses{

    // fccs

    // 1
    std::vector <std::string> fccs_what_time{"The current time is ", "Time right now: ", "It is currently ", "Right now it is ",
                                             "Currently it is ", "Current time: "};

    // 2
    std::vector <std::string> fccs_what_date{"The current date is ", "Today's date: ", "Today's date is: "};

    // end of fccs


    // fscs

    // 1
    std::vector <std::string> fscs_what_time{"The current time is ", "Time right now: ", "It is currently ", "Right now it is ",
                                             "Currently it is ", "Current time: "};

    // 2
    std::vector <std::string> fscs_what_date{"The current date is ", "Today's date: ", "Today's date is: "};

    // end of fscs

} // End of namespace r4_responses

// Namespace g_responses
namespace g_responses{

    std::vector <std::string> s_fcs_your_day{"My day is going well, thank you for asking.",
                                             "Thank you for asking, my day is going well."};

    std::vector <std::string> s_fcs_the_day{"The day has been great, thank you for asking.",
                                            "Thank you for asking - the day is going well.",
                                            "The day has been good so far. Thank you for asking."};

    std::vector <std::string> s_fcs_it{"It is going well, thanks.", "Thanks for asking, it's going well."};

    std::vector <std::string> s_fcs_that{"I am going to need to know what that is?",
                                         "What is it you are referring to?",    
                                         "What exactly are you referring to?"}; 
    int s_fcs_that_cont_flag=0;

    std::vector <std::string> s_fcs_your_life{"My life has been great, I exist to help people",
                                              "Life is good, thank you for asking.",
                                              "My life has been very good, I have been made to enjoy it."};

    std::vector <std::string> s_fcs_the_life{"Life is good.", "The life has been good so far - thanks for asking."};

    std::vector <std::string> s_fcs_experience{"What experience are you referring to?",
                                               "I would be happy to indulge, but what experience are you refering to?",
                                               "Which experience are you talking about?"}; 
    int s_fcs_experience_cont_flag=0;

} // End of namespace g_responses

// Namespace r4_vectors
namespace r4_vectors{

    std::vector <std::string> fccs{"f", "c", "c", "s"};

    std::vector <std::string> fscs{"f", "s", "c", "s"};

} // End of namespace r4_vectors

// Namespace r3_vectors
namespace r3_vectors{
    
    std::vector <std::string> fcs{"f", "c", "s"};

} // End of namespace r3_vectors

// Namespace r1_vectors
namespace r1_vectors{

    std::vector <std::string> s{"s"};

} // End of namespace r1_vectors

// Debugging and error function
int err_status(){
    
    // Return 0 and exit program if "exit" is detected
    if(query == "exit"){
        return 0;
    }

    // Check completion flag or if self learning was instantiated, display error if not true
    if(completion_flag == false && self_learning_flag == false){
        std::cout << "I could not find the input you requested." << std::endl;
        
        err_container=true;
    }

}

// Complete success
void complete_success(){

    completion_flag=true;
    err_container=false;

}

// Function to generate random slot for response vector
int fetch_random_response_slot(int size_of_vector){
    
    // Seed number generator according to computer clock
    // ensuring different seeding each time
    srand(time(NULL));
    return rand()%size_of_vector;

}

// Edit distance calculation
int edit_distance(std::string str1, std::string str2){
    
    // Variables
    int previous, temporary_container;

    // Initialise for current run
    int size1 = str1.size();
    int size2 = str2.size();
    std::vector<int> current(size2 + 1, 0);
 
    for (int i=0; i<=size2; i++){
        
        current[i] = i;
    
    }

    for (int i=1; i<=size1; i++){
        
        previous = current[0];
        current[0] = i;

        for (int j=1; j<=size2; j++){
            
            temporary_container = current[j];
            
            if (str1[i - 1] == str2[j - 1]){
                
                current[j] = previous;
            
            } else{

                current[j] = 1 + std::min({previous, current[j - 1], current[j]});
            
            }

            previous = temporary_container;
        
        }
    
    }
    
    return current[size2];

}

// Function that returns the index of a substring within a string
int substring_index_within_string(std::string string_to_be_searched, std::string substring){

    size_t index = string_to_be_searched.find(substring);

    if(index != std::string::npos){
        return int(index);
    } 

}

// Spell checking function according to "english_words.txt" in reservoir folder, returns corrected tokens with assistance of user
std::vector <std::string> spell_check(std::vector<std::string> uncorrected_tokens){

    // Variables
    std::vector <std::string> lexicon_contents;
    std::vector <std::string> lexicon_contents_copy;
    std::vector <std::string> corrected_tokens;
    corrected_tokens.clear();
    std::vector <std::string> current_corrections;
    std::vector <int> edit_distances;
    std::string str;
    std::string current_token;
    std::string word_choice;
    std::string sequence_window;
    int index;
    int token_letter_count;
    int lexicon_match_location;
    int word_suggestion_count = 10;
    bool lexicon_matched;
    bool corrected_token_vector_filled = false;

    // Unpack lexicon into a vector with forced capitalisation and removal of non-alphabet chars
    std::fstream file;
    file.open("../reservoir/english_words.txt", std::ios::in);
    while(getline(file, str))
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        for (int i=0; i<str.size(); i++){

            if (str[i] < 'A' || str[i] > 'Z' && str[i] < 'a' || str[i] > 'z') {
    
                str.erase(i, 1);
                i--;

            }

        }

        lexicon_contents.push_back(str);
    
    }

    // Compare all uncorrected tokens to all entries in lexicon and return up to the 3 most similar words
    // This algorithm iterates all char sequence lengths starting from (token letter count - 1) to 2 and pushes back the top 3 words with the largest amount of sequence matches
    for(int token=0; token<uncorrected_tokens.size(); token++){

        // Clear current corrections and edit distance for every token, refresh lexicon_contents_copy
        current_corrections.clear();
        edit_distances.clear();
        lexicon_contents_copy.clear();
        for(int i=0; i<lexicon_contents.size(); i++){
            lexicon_contents_copy.push_back(lexicon_contents[i]);
        }

        // Check if the word exists in lexicon
        lexicon_matched = false;
        for(int lexicon_word=0; lexicon_word<lexicon_contents.size(); lexicon_word++){
            
            if(lexicon_contents[lexicon_word] == uncorrected_tokens[token]) lexicon_matched = true;
        
        } 
        
        // If word is not found in the lexicon
        current_token = uncorrected_tokens[token];
        token_letter_count = current_token.size();
        if(lexicon_matched == false){

            // Fill edit_distances vector for current token
            for(int i=0; i<lexicon_contents.size(); i++){
                
                edit_distances.push_back(edit_distance(uncorrected_tokens[token], lexicon_contents[i]));

            }

            // Locate words with the lowest edit distances
            auto it = std::min_element(std::begin(edit_distances), std::end(edit_distances));
            index = std::distance(std::begin(edit_distances), it);

            // If edit distance is < 3 AND the word has > 3 letters, automatically correct and inform the user, otherwise, ask for approval
            if(edit_distances[index] < 3 && token_letter_count > 3){
                
                std::cout << uncorrected_tokens[token] << " has been corrected to " << lexicon_contents[index] << std::endl;
                corrected_tokens.push_back(lexicon_contents[index]);
            
            } else{ // Initiate manual token correction sequence

                // Get <word_suggestion_count> best corrections to current uncorrected token
                for(int i=0; i<word_suggestion_count; i++){

                    current_corrections.push_back(lexicon_contents_copy[index]);
                    edit_distances.erase(edit_distances.begin() + index);
                    lexicon_contents_copy.erase(lexicon_contents_copy.begin() + index);

                    it = std::min_element(std::begin(edit_distances), std::end(edit_distances));
                    index = std::distance(std::begin(edit_distances), it);

                }

                std::cout << uncorrected_tokens[token] << " is not found in the reservoir, please select a correction, or add it as a valid word..." << std::endl;

                // Display correction options, including the option to add the token to the reservoir
                for(int i=0; i<word_suggestion_count; i++){

                    std::cout << "[" << i+1 << "] " << current_corrections[i] << std::endl;

                }
                std::cout << "[" << word_suggestion_count+1 << "] " << "<add word to lexicon>" << std::endl;    

                // Request user choice for correction choice
                std::cout << "Input choice: ";
                std::getline(std::cin, word_choice);

                // Check for valid format of inputted answer, otherwise choose option [1] by default
                bool valid_format = false;
                bool valid_digit = false;
                int digit_check = 0;
                for(int i=0; i<word_choice.length(); i++){ // Check total digits inputted

                    if(isdigit(word_choice[i])) digit_check++;

                }

                // Check all the elements of input are digits, and are less than 3 and more than 1 of them
                if(digit_check == word_choice.length() && word_choice.length() < 3 && word_choice.length() > 0) valid_digit = true;

                // Combine all verification and decide if the input is valid or not
                if(valid_digit == true){

                    if((stoi(word_choice) > 0) && (stoi(word_choice) <= word_suggestion_count+1)) valid_format = true;

                }

                // Option is valid, process decision
                if(valid_format == true){

                    // Correct token to chosen option
                    int word_choice_int = stoi(word_choice);
                    if(word_choice_int < word_suggestion_count+1){

                        std::cout << "Corrected " << uncorrected_tokens[token] << " to " << current_corrections[word_choice_int-1] << std::endl;
                        corrected_tokens.push_back(current_corrections[word_choice_int-1]);
                    
                    } else if(word_choice_int == word_suggestion_count+1){ // Add uncorrected token to reservoir

                        std::cout << "Added " << uncorrected_tokens[token] << " to the reservoir" << std::endl;
                        lexicon_contents.push_back(uncorrected_tokens[token]);

                        std::ofstream lexicon_ostream;
                        lexicon_ostream.open("../reservoir/english_words.txt", std::ios::app);
                        lexicon_ostream << "\n" << uncorrected_tokens[token];
                        lexicon_ostream.close();

                        corrected_tokens.push_back(uncorrected_tokens[token]);

                    } else{ // Generic error with the user inputted variable, should be impossible to get here

                        std::cout << "Exiting sequence due to a generic error at line" << __LINE__ << " in main/terminal.cpp" << std::endl;

                    }
                
                } else{ // Default options incase the above fails

                    std::cout << "Invalid input, assuming option [1]" << std::endl;
                    std::cout << "Corrected " << uncorrected_tokens[token] << " to " << current_corrections[0] << std::endl;
                    corrected_tokens.push_back(current_corrections[0]);

                }

            }

        } else{
        
            // If token correction is not required because of a direct match in lexicon, return the original token
            corrected_tokens.push_back(uncorrected_tokens[token]);

        }

    }

    // Return final corrected vector
    return corrected_tokens;

}

// Function to modify vector containers with incoming datastream from self learning protocol
void modify_vector_locally(std::string modification_element, std::string vector_to_modify){
    
    // Variables
    std::string str;
    std::string modification_element_formatted;
    int local_iterator;
    bool previously_added = false;

    // Append , to modification_element for formatting purposes
    modification_element_formatted = ", \"" + modification_element + "\"";
    
    // Parse file contents into file_contents vector
    std::vector <std::string> file_contents;
    std::fstream file;

    if(savestate == false){
        file.open("terminal.cpp", std::ios::in);
        savestate = true;
    } else{
        file.open("terminal_modified.cpp", std::ios::in);
    }

    while(getline(file, str))
    {
        file_contents.push_back(str);
    }

    // Match vector hash identifier to what is requested upon this function being called
    std::vector <std::string> vector_hashes_container;
    std::fstream vector_hash_file;
    vector_hash_file.open("SHA3_hashes.txt", std::ios::in);
    while(getline(vector_hash_file, str))
    {
        vector_hashes_container.push_back(str);
    }

    // Split vector_hashes to individual components, so it is not stored line by line but element by element instead
    std::vector <std::string> vector_hashes;
    for(int i=0; i<vector_hashes_container.size(); i++){

        std::stringstream ss(vector_hashes_container[i]);

        while(getline(ss, str, ' ')){

            vector_hashes.push_back(str);

        }
        
    }

    // Match inputted vector_to_modify with the correct hash according to vector_hashes
    int hash_index = 0;
    auto it = find(vector_hashes.begin(), vector_hashes.end(), vector_to_modify); 
    if(it != vector_hashes.end()){ 

        hash_index = it - vector_hashes.begin(); 

    } else{

        std::cout << "The hash of the vector to be modified was not found in SHA3_hashes.txt file..." << std::endl;
    
    }
    std::string hash = "}; // " + vector_hashes[hash_index+1];

    // This is purely for duplicate protection. If intended addition is already in vector, omit adding a copy in the next step
    for(int i=0; i<cached_unknown_tokens.size(); i++){
        if(cached_unknown_tokens[i] == modification_element) previously_added = true;
    }

    // Look for "}; // *HASH*" keyphrase (vector identifier), and add modification element to the vector list
    local_iterator = 0;
    if(previously_added == false){
        for(int i=0; i<file_contents.size(); i++){
            
            if(file_contents[i].find(hash) != file_contents[i].npos){

                local_iterator = substring_index_within_string(file_contents[i], hash);

                file_contents[i].insert(local_iterator, std::string(modification_element_formatted));

            }
        }

        // Register newly added token so as to not duplicate it in current run
        cached_unknown_tokens.push_back(modification_element);

    } else{

        std::cout << modification_element << " already exists in " << vector_to_modify << "..." << std::endl;
    
    }

    // Output changed elements into a temporary copy of this file
    std::ofstream output_file("terminal_modified.cpp");
    for(int i=0; i<file_contents.size(); i++){
        output_file << file_contents[i] << std::endl;
    }
    output_file.close();

}

// Main function
int main(){

    // Continuation variables;
    bool key = true;
    bool c_tf;
    bool message_disp_flag;
    int layer = 0;

    // Terminal invocation
    do{ 
//------- Reservoir stream --------------------------------------------------------------------------------------------------------        
        std::ifstream myStream("sense_and_sensibility");

//---------------------------------------------------------------------------------------------------------------------------------
        // Error check initialisation
        // If err=true at the end of command, err_status is called and message is displayed
        completion_flag=false;
        self_learning_flag = false;
        
        // User input string
        std::cout << "Cithria: ";
        std::getline(std::cin, query);
        query_upper.assign(query);

        // Transform characters in string to upper case for ease of use
        std::transform(query_upper.begin(), query_upper.end(), query_upper.begin(), ::toupper);

        // Basic response
        if (if_or_processor_hashtable(query_upper, simple_query)==true){
            std::cout << "Hello." << std::endl;
            
            complete_success();
        
        // Debugging messages if prompted
        } else if (if_or_processor_hashtable(query_upper, debug_table)==true){
            std::cout << "completion_flag: " << completion_flag << std::endl << "err_container: " << err_container << std::endl;
            
            complete_success();

        } else{

            // Move to interpretation if above options are not triggered
            if (completion_flag==false && query != "exit"){
                
                // Fill token vector
                token = tokenise(query_upper);

                // Tokenisation positions
                type_position = parse_tokenisation(token);

                // Self learn protocol triggered by text in [[]] brackets
                if(self_learning_check(query_upper)==true){
                    
                    // Enable self learning flag
                    self_learning_flag = true;

                    // Clear any previously remaining model answers
                    model_answers.clear();

                    // Obtain unknown tokens
                    unknown_tokens.clear();
                    unknown_tokens = fetch_unknown_tokens(token);
 
                    // Display type position of inserted query
                    std::cout << "Type position: ";
                    for(int i=0; i<type_position.size(); i++){
                        std::cout << type_position[i] << " ";
                    }
                    std::cout << std::endl;

                    // Check if there are any unknown tokens, and if so, request user categorisation
                    if(unknown_tokens.size()>0){
                        
                        // Pass through English dictionary to see if any words are mispelt, and suggest correction(s)
                        unknown_tokens = spell_check(unknown_tokens);

                        // Loop over all unknown tokens to request manual input
                        if(unknown_tokens.size()==1) std::cout << "Requesting manual category input for unknown token" << std::endl;
                        if(unknown_tokens.size()>1) std::cout << "Requesting manual category input for unknown tokens" << std::endl;
                        for(int i=0; i<unknown_tokens.size(); i++){
                            
                            // Request token type input
                            std::cout << unknown_tokens[i] << ": ";
                            std::getline(std::cin, unknown_token_type);
                            std::transform(unknown_token_type.begin(), unknown_token_type.end(), unknown_token_type.begin(), ::toupper);

                            // Store in relevant vector container via hash function identification
                            if(unknown_token_type == "F" || unknown_token_type == "FACTUAL"){

                                modify_vector_locally(unknown_tokens[i], "factual_type");
                                token[unknown_token_positions[i]] = "f";

                            } else if(unknown_token_type == "C" || unknown_token_type == "CONNECTOR"){

                                modify_vector_locally(unknown_tokens[i], "connector_type");
                                token[unknown_token_positions[i]] = "c";

                            } else if(unknown_token_type == "X" || unknown_token_type == "CONTEXTUAL"){

                                modify_vector_locally(unknown_tokens[i], "contextual_type");
                                token.erase(token.begin() + unknown_token_positions[i]);
                                /*
                                This part needs to include a function that adds the new contextual word as a flag,
                                as compared to f, c, and s, it does not utilise type position analysis in the same way.
                                
                                Probably use the vector modifier function for this.
                                */

                            } else if(unknown_token_type == "S" || unknown_token_type == "SUBJECT"){

                                modify_vector_locally(unknown_tokens[i], "subject_type");
                                token[unknown_token_positions[i]] = "s";

                            } else{
                                
                                std::cout << unknown_token_type << " is not a defined token category. Please try F (factual), C (connector), X (contextual), or S (subject)." << std::endl;
                                complete_success();
										  
                            }

                        }

                    } 

                    // Check if token contains any unreplaced '?'s, if not, request model responses to query
                    if(std::find(token.begin(), token.end(), "?") != token.end()){

                        // '?'s detected, something went wrong with manual user word type input
                        std::cout << "The input tokens contain \'?\' even after user assignment. Please restart and re-input your query." << std::endl;

                    } else{

                        // Request user model answer to query
                        std::cout << "Input model answer to query; input [[]] to exit." << std::endl;

                        // User input stream
                        std::string model_answer_str;
                        std::string model_answer_str_corr;
                        int model_answer_count = 1;
                        do{
                            
                            std::cout << model_answer_count << " --> ";
                            model_answer_count++;

                            // Retrieve user input string and apply punctuation/capitalisation if missing
                            std::getline(std::cin, model_answer_str);
                            model_answer_str_corr.assign(model_answer_str);
                            // Apply punctuation and capitalisation corrections to model_answer_str_corr

                            // Update model answers vector
                            model_answers.push_back(model_answer_str_corr);

                        } while(model_answer_str != "[[]]");

                        /*
                        Design algorithm that appropriately appends model answer to corresponding vector, and
                        assigns an answering scheme based on the most important words. This most likely follows
                        that f, and s are key, although it is important to think about exceptions. Possibly longer
                        sentences require all token type slots to be fulfilled?

                        Model answers to current query can be found in model_answers_str_corr.
                        */

                    }

                }

                // Prevent standard protocols from taking place if self learning is detected
                if(self_learning_flag == false){

                    // Rank 4 function position analysis
                    if(if_aa_processor_vect(type_position, r4_vectors::fccs)==true){

                        // What x x time response protocol
                        if(token[0]=="WHAT" && token[3]=="TIME" &&
                        (types::current_flag==true || types::todays_flag==true || types::today_flag==true || types::now_flag==true ||
                        types::currently_flag==true || types::the_flag==true)){

                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r4_responses::fccs_what_time.size());

                            // Store and display time according to system clock
                            time_t now = time(0);
                            tm *local_time = localtime(&now);
                            
                            std::cout << r4_responses::fccs_what_time[random] << local_time->tm_hour << ":";
                            std::cout << local_time->tm_min << ":";
                            std::cout << local_time->tm_sec << std::endl;

                            complete_success();

                        }

                        // What x x date response protocol
                        if(token[0]=="WHAT" && token[3]=="DATE" &&
                        (types::current_flag==true || types::todays_flag==true || types::today_flag==true || types::now_flag==true ||
                        types::currently_flag==true || types::the_flag==true)){

                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r4_responses::fccs_what_date.size());

                            // Store and display date according to system clock
                            time_t now = time(0);
                            tm *local_time = localtime(&now);
                            
                            std::cout << r4_responses::fccs_what_date[random] << local_time->tm_mday << "/";
                            std::cout << 1 + local_time->tm_mon << "/";
                            std::cout << 1900 + local_time->tm_year << std::endl;

                            complete_success();

                        }

                    }
                    
                    // Rank 4 function position analysis
                    if(if_aa_processor_vect(type_position, r4_vectors::fscs)==true){

                        // What time x x response protocol
                        if(token[0]=="WHAT" && token[1]=="TIME" &&
                        (types::current_flag==true || types::todays_flag==true || types::today_flag==true || types::now_flag==true ||
                        types::currently_flag==true || types::it_flag==true)){

                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r4_responses::fscs_what_time.size());

                            // Store and display date according to system clock
                            time_t now = time(0);
                            tm *local_time = localtime(&now);
                            
                            std::cout << r4_responses::fscs_what_time[random] << local_time->tm_hour << ":";
                            std::cout << local_time->tm_min << ":";
                            std::cout << local_time->tm_sec << std::endl;

                            complete_success();

                        }

                        // What date x x response protocol
                        if(token[0]=="WHAT" && token[1]=="DATE" &&
                        (types::current_flag==true || types::todays_flag==true || types::today_flag==true || types::now_flag==true ||
                        types::currently_flag==true || types::it_flag==true)){

                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r4_responses::fscs_what_date.size());

                            // Store and display date according to system clock
                            time_t now = time(0);
                            tm *local_time = localtime(&now);
                            
                            std::cout << r4_responses::fscs_what_date[random] << local_time->tm_mday << "/";
                            std::cout << 1 + local_time->tm_mon << "/";
                            std::cout << 1900 + local_time->tm_year << std::endl;

                            complete_success();

                        }

                    }

                    // Rank 3 function position analysis
                    if((if_aa_processor_vect(type_position, r3_vectors::fcs)==true)
                    || r3_responses::fcs_how_it_cont_flag>0){  // by default if continuation flag is enabled

                        // How x you response protocol
                        if(token[0]=="HOW" && token[2]=="YOU"){
                            
                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r3_responses::fcs_how_you.size());
                            std::cout << r3_responses::fcs_how_you[random] << std::endl;
                            
                            complete_success();

                        }

                        // What x you response protocol
                        if(token[0]=="WHAT" && token[2]=="YOU"){
                            
                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r3_responses::fcs_what_you.size());
                            std::cout << r3_responses::fcs_what_you[random] << std::endl;
                            
                            complete_success();

                        }

                        // Who x you response protocol
                        if(token[0]=="WHO" && token[2]=="YOU"){
                            
                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r3_responses::fcs_who_you.size());
                            std::cout << r3_responses::fcs_who_you[random] << std::endl;
                            
                            complete_success();

                        }

                        // Where x you response protocol
                        if(token[0]=="WHERE" && token[2]=="YOU"){
                            
                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r3_responses::fcs_where_you.size());
                            std::cout << r3_responses::fcs_where_you[random] << std::endl;
                            
                            complete_success();

                        }

                        // When x you response protocol
                        if(token[0]=="WHEN" && token[2]=="YOU"){
                            
                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r3_responses::fcs_when_you.size());
                            std::cout << r3_responses::fcs_when_you[random] << std::endl;
                            
                            complete_success();

                        }

                        //START--------------------------------------------------------------------------------------------------------------------------------------------------------
                        // How x it response protocol
                        if(token[0]=="HOW" && token[2]=="IT"){

                            // Fetch random response from raw hash table
                            int random = fetch_random_response_slot(r3_responses::fcs_how_it.size());
                            std::cout << r3_responses::fcs_how_it[random] << std::endl;

                            // If responded with a question, identify
                            if(r3_responses::fcs_how_it[random].find('?') != std::string::npos){

                                r3_responses::fcs_how_it_cont_flag++;
                                message_disp_flag = false;
                                layer++;

                            }

                            complete_success();

                        }

                        // Conditions to seek question response from user
                        if (r3_responses::fcs_how_it_cont_flag>0 && message_disp_flag==true){

                            // Your x day response protocol
                            c_tf = fcs_comparator(token, "YOUR", "DAY");
                            if(c_tf==true && layer==1){
                                    
                                // Fetch random response from raw hash table
                                int random = fetch_random_response_slot(g_responses::s_fcs_your_day.size());
                                std::cout << g_responses::s_fcs_your_day[random] << std::endl;
                                    
                                complete_success();
                                r3_responses::fcs_how_it_cont_flag = 0;
                                layer=0;

                            }

                            // The x day response protocol
                            c_tf = fcs_comparator(token, "THE", "DAY");
                            if(c_tf==true && layer==1){
                                
                                // Fetch random response from raw hash table
                                int random = fetch_random_response_slot(g_responses::s_fcs_the_day.size());
                                std::cout << g_responses::s_fcs_the_day[random] << std::endl;
                                    
                                complete_success();
                                r3_responses::fcs_how_it_cont_flag = 0;
                                layer=0;

                            }

                            // Your x life response protocol
                            c_tf = fcs_comparator(token, "YOUR", "LIFE");
                            if(c_tf==true && layer==1){
                                
                                // Fetch random response from raw hash table
                                int random = fetch_random_response_slot(g_responses::s_fcs_your_life.size());
                                std::cout << g_responses::s_fcs_your_life[random] << std::endl;
                                    
                                complete_success();
                                r3_responses::fcs_how_it_cont_flag = 0;
                                layer=0;

                            }

                            // The x life response protocol
                            c_tf = fcs_comparator(token, "THE", "LIFE");
                            if(c_tf==true && layer==1){
                                
                                // Fetch random response from raw hash table
                                int random = fetch_random_response_slot(g_responses::s_fcs_the_life.size());
                                std::cout << g_responses::s_fcs_the_life[random] << std::endl;
                                    
                                complete_success();
                                r3_responses::fcs_how_it_cont_flag = 0;
                                layer=0;

                            }
                        
                            //-------------------------------------------------------------------------------------------------------------------------------------------------------------
                            //START--------------------------------------------------------------------------------------------------------------------------------------------------------
                            // Your x experience response protocol
                            c_tf = fcs_comparator(token, "YOUR", "EXPERIENCE"); 
                            if(c_tf==true && layer==1){                                     
                                
                                // Fetch random response from raw hash table
                                int random = fetch_random_response_slot(g_responses::s_fcs_experience.size());
                                std::cout << g_responses::s_fcs_experience[random] << std::endl;

                                // If responded with a question, identify
                                if(g_responses::s_fcs_experience[random].find('?') != std::string::npos){

                                    g_responses::s_fcs_experience_cont_flag++;
                                    message_disp_flag = false;
                                    layer++;

                                } else{
                                    layer=0;
                                }
                                
                                complete_success();

                            }

                            // The x experience response protocol
                            c_tf = fcs_comparator(token, "THE", "EXPERIENCE"); 
                            if(c_tf==true && layer==1){                                    
                                
                                // Fetch random response from raw hash table
                                int random = fetch_random_response_slot(g_responses::s_fcs_experience.size());
                                std::cout << g_responses::s_fcs_experience[random] << std::endl;
                                
                                // If responded with a question, identify
                                if(g_responses::s_fcs_experience[random].find('?') != std::string::npos){

                                    g_responses::s_fcs_experience_cont_flag++;
                                    message_disp_flag = false;
                                    layer++;

                                } else{
                                    layer=0;
                                }
                                    
                                complete_success();

                            }

                            // Conditions to seek question response from user
                            if (g_responses::s_fcs_experience_cont_flag>0 && message_disp_flag==true){
                                
                                // Your x life response protocol
                                c_tf = fcs_comparator(token, "YOUR", "LIFE");
                                if(c_tf==true && layer==2){
                                    
                                    // Fetch random response from raw hash table
                                    int random = fetch_random_response_slot(g_responses::s_fcs_your_life.size());
                                    std::cout << g_responses::s_fcs_your_life[random] << std::endl;
                                        
                                    complete_success();
                                    r3_responses::fcs_how_it_cont_flag = 0;
                                    layer=0;

                                }

                                // The x life response protocol
                                c_tf = fcs_comparator(token, "THE", "LIFE");
                                if(c_tf==true && layer==2){
                                    
                                    // Fetch random response from raw hash table
                                    int random = fetch_random_response_slot(g_responses::s_fcs_the_life.size());
                                    std::cout << g_responses::s_fcs_the_life[random] << std::endl;
                                        
                                    complete_success();
                                    r3_responses::fcs_how_it_cont_flag = 0;
                                    layer=0;

                                }

                            }
                            //END----------------------------------------------------------------------------------------------------------------------------------------------------------
                            //-------------------------------------------------------------------------------------------------------------------------------------------------------------

                            // It response protocol
                            c_tf = s_comparator(token, "IT");
                            if(c_tf==true){
                                
                                // Fetch random response from raw hash table
                                int random = fetch_random_response_slot(g_responses::s_fcs_it.size());
                                std::cout << g_responses::s_fcs_it[random] << std::endl;
                                    
                                complete_success();
                                r3_responses::fcs_how_it_cont_flag = 0;
                                layer=0;

                            }

                            //-------------------------------------------------------------------------------------------------------------------------------------------------------------
                            //START--------------------------------------------------------------------------------------------------------------------------------------------------------
                            // That response protocol
                            c_tf = s_comparator(token, "THAT"); 
                            if(c_tf==true && layer==1){                     
                                
                                // Fetch random response from raw hash table
                                int random = fetch_random_response_slot(g_responses::s_fcs_that.size());
                                std::cout << g_responses::s_fcs_that[random] << std::endl;
                                
                                // If responded with a question, identify
                                if(g_responses::s_fcs_that[random].find('?') != std::string::npos){

                                    r3_responses::fcs_how_it_cont_flag++;
                                    message_disp_flag = false;
                                    layer++;

                                } else{
                                    layer=0;
                                }

                                complete_success();

                            }

                            // Conditions to seek question response from user
                            if (g_responses::s_fcs_that_cont_flag>0 && message_disp_flag==true){
                                
                                // Your x day response protocol
                                c_tf = fcs_comparator(token, "YOUR", "DAY");
                                if(c_tf==true && layer==2){
                                        
                                    // Fetch random response from raw hash table
                                    int random = fetch_random_response_slot(g_responses::s_fcs_your_day.size());
                                    std::cout << g_responses::s_fcs_your_day[random] << std::endl;
                                        
                                    complete_success();
                                    r3_responses::fcs_how_it_cont_flag = 0;
                                    layer=0;

                                }

                                // The x day response protocol
                                c_tf = fcs_comparator(token, "THE", "DAY");
                                if(c_tf==true && layer==2){
                                    
                                    // Fetch random response from raw hash table
                                    int random = fetch_random_response_slot(g_responses::s_fcs_the_day.size());
                                    std::cout << g_responses::s_fcs_the_day[random] << std::endl;
                                        
                                    complete_success();
                                    r3_responses::fcs_how_it_cont_flag = 0;
                                    layer=0;

                                }

                                // Your x life response protocol
                                c_tf = fcs_comparator(token, "YOUR", "LIFE");
                                if(c_tf==true && layer==2){
                                    
                                    // Fetch random response from raw hash table
                                    int random = fetch_random_response_slot(g_responses::s_fcs_your_life.size());
                                    std::cout << g_responses::s_fcs_your_life[random] << std::endl;
                                        
                                    complete_success();
                                    r3_responses::fcs_how_it_cont_flag = 0;
                                    layer=0;

                                }

                                // The x life response protocol
                                c_tf = fcs_comparator(token, "THE", "LIFE");
                                if(c_tf==true && layer==2){
                                    
                                    // Fetch random response from raw hash table
                                    int random = fetch_random_response_slot(g_responses::s_fcs_the_life.size());
                                    std::cout << g_responses::s_fcs_the_life[random] << std::endl;
                                        
                                    complete_success();
                                    r3_responses::fcs_how_it_cont_flag = 0;
                                    layer=0;

                                }

                            }
                            //END----------------------------------------------------------------------------------------------------------------------------------------------------------
                            //-------------------------------------------------------------------------------------------------------------------------------------------------------------

                        }
                        //END----------------------------------------------------------------------------------------------------------------------------------------------------------

                        // Message displayed before responses boolean
                        message_disp_flag = true;

                    }

                }

            }

            // Display error message if found
            err_status();

        } 

    // Exit condition
    } while (query != "exit");

    // Exit message
    std::cout << "Exiting..." << std::endl;

} // End of program