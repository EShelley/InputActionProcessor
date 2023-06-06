// InputActionProcessor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include"InputProcessor.h"


std::string getAction(const std::string& input, const std::unordered_map<std::string, std::vector<std::string>>& synonymMap);
std::unordered_map<std::string, std::vector<std::string>> loadMapFromJSON(const std::string& filename);


int main()
{   
    
    InputProcessor b = InputProcessor("./commands.json");
    
    b.DisplayCommands();

    std::string input = {};

    while(true) {
        std::cout << "What do you want to do: > ";
        std::getline(std::cin,input);
#ifdef TOKEN_DEBUGGER
        std::cout << "Raw Input String: " << input << '\n';
#endif // TOKEN_DEBUGGER

        //tokenize input and parse command
        std::string command = b.getAction(input);
        std::cout << command << '\n';
        if (command == "quit")
            break;
        
    }
    
    
}
