// InputActionProcessor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include"InputProcessor.h"


std::string getAction(const std::string& input, const std::unordered_map<std::string, std::vector<std::string>>& synonymMap);
std::unordered_map<std::string, std::vector<std::string>> loadMapFromJSON(const std::string& filename);

// #define TOKEN_DEBUGGER

int main()
{
    /**
    std::cout << "Hello World!\n";

    std::unordered_map<std::string,std::vector<std::string>> c_umap{
    {"move", {"move", "go", "run", "walk", "head"}},
    {"look", {"look", "view"}}
    };
    json j_umap(c_umap);
    // {"one": 1.2, "two": 2.3, "three": 3.4}
    std::cout << std::setw(4) << j_umap;
    
    std::ofstream f("commands.json");
    
    f << std::setw(4) << j_umap;
    f.close();
    std::cin.get();
    **/

    std::unordered_map<std::string, std::vector<std::string>> c_umap = loadMapFromJSON("./commands.json");

    std::cout << "========  Command Map  =========\n\n" << json(c_umap) << "======================================\n\n";
    
    std::string input = {};

    while(true) {
        std::cout << "What do you want to do: > ";
        std::getline(std::cin,input);
#ifdef TOKEN_DEBUGGER
        std::cout << "Raw Input String: " << input << '\n';
#endif // TOKEN_DEBUGGER

        //tokenize input and parse command
        std::string command = getAction(input, c_umap);
        std::cout << command << '\n';
        if (command == "quit")
            break;
        
    }
}

/**  Possibly declare a new Type for actions later to make it easier to process?  For now Ill just use a std::vector<std::string>
struct ActionType {
    std::string Verb;
    std::string DirectObject;
    std::vector<std::string> IndirectObjects;
};
*/

// generated with ChatGPTs help


// converts an std::string to lowercase
std::string toLowercase(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

// parses the std::string input from the player into its component tokens
std::vector<std::string> tokenizeInput(const std::string& input) {
    // if the input is empty return an empty vector
    if (input.empty())
        return {};
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;

    // tokenize the input string while converting everything to lowercase
    while (iss >> token) {
        tokens.push_back(token);
    }

#ifdef TOKEN_DEBUGGER
    std::cout << "Tokens: | ";
    for (auto i = tokens.begin(); i != tokens.end(); i++)
        std::cout << *i << " | ";
    std::cout << "\n\n";
#endif  // TOKEN_DEBUGGER
  
    // return the token string
    return tokens;
}

std::string getAction(const std::string& input, const std::unordered_map<std::string, std::vector<std::string>>& synonymMap) {
 
    // Tokenize Input Sting
    std::vector<std::string> tokens = tokenizeInput(toLowercase(input));
    if (tokens.empty())
        return "Ok brain, tell me what to do please";

    for (const auto& actionSynonyms : synonymMap) {
        const std::string& action = actionSynonyms.first;
        const std::vector<std::string>& synonyms = actionSynonyms.second;

        // Check if input matches the action or any of its synonyms
        auto it = std::find_if(synonyms.begin(), synonyms.end(), [&](const std::string& synonym) {
            return tokens[0].compare(synonym) == 0;  // Case-insensitive comparison(WRONG!)  This is case-sensitive!
            });

        if (it != synonyms.end()) {
            return action;
        }
    }

    // No match found
    return "ERROR: I dont Understand that Command";  // Or you can define an "unknown" action
}


std::unordered_map<std::string, std::vector<std::string>> loadMapFromJSON(const std::string& filename) {
    std::unordered_map<std::string, std::vector<std::string>> myMap;

    std::ifstream inputFile(filename);
    if (inputFile.is_open()) {
        json jsonData;
        inputFile >> jsonData;

        for (const auto& [key, value] : jsonData.items()) {
            if (value.is_array()) {
                std::vector<std::string> values = value.get<std::vector<std::string>>();
                myMap[key] = values;
            }
        }

        inputFile.close();
        std::cout << "Map loaded from JSON file successfully.\n";
    }
    else {
        std::cerr << "Unable to open the file.\n";
    }

    return myMap;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
