#include <iostream>
#include <vector>
#include <string>
#include <fstream>

int main(){

    std::string str;
    std::vector<std::string> file_contents;

    bool testbool = false;
    if(testbool == false){

        testbool = true;

    }
    
    std::fstream file;
    file.open("testsample.cpp",std::ios::in);

    while(getline(file, str))
    {
        file_contents.push_back(str) ;
    }

}

// std::vector <std::string> factual_type{"HOW", "WHAT", "WHO", "WHEN", "WHERE"}; // FACTUALHASH
