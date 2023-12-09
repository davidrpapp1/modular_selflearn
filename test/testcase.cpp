#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

int substring_index_within_string(std::string string_to_be_searched, std::string substring){

    size_t index = string_to_be_searched.find(substring);

    if(index != std::string::npos){
        return int(index);
    } 

}

int main(){

    std::string str;
    std::vector<std::string> file_contents;

    bool testbool = false;
    if(testbool == false){

        testbool = true;

    }
    
    std::fstream file;
    //HASH
    file.open("testsample.cpp",std::ios::in);

    while(getline(file, str))
    {
        file_contents.push_back(str);
    }

    int index2 = 0;

    // Look for }; // FACTUALHASH keyphrase, and add , pasta to the vector list on that line
    std::string changetext = ", pasta";
    std::string initialtext = "}; // FACTUALHASH";

    /*
    for(int i=0; i<file_contents.size(); i++){
        
        if(file_contents[i].find(initialtext) != file_contents[i].npos){

            index2 = substring_index_within_string(file_contents[i], initialtext);

            file_contents[i].insert(index2, std::string(changetext));

        }
    
    }*/

    // Look for }; // FACTUALHASH keyphrase, and remove , pasta element from the vector
    for(int i=0; i<file_contents.size(); i++){
        
        if(file_contents[i].find(initialtext) != file_contents[i].npos){

            if(file_contents[i].find(changetext) != file_contents[i].npos){
                index2 = substring_index_within_string(file_contents[i], initialtext);

                file_contents[i].erase(index2-changetext.size(), changetext.size());
            }
        }
    
    }

    std::ofstream output_file("testsample.cpp");
    for(int i=0; i<file_contents.size(); i++){
        output_file << file_contents[i] << std::endl;
    }

}