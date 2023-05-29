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

// below generated with ChatGPTs help


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


/**
    Currently takes the direct input from the player and parses it into an action matched againts the synonymMap
    As this is Tranfered into a container class most of this will be internal, and the interface api will most likel
    be one function that takes the raw input and processes it into actions the game model/engine understands.
    Notes about the return format:
        Maybe a struct?
        struct ActionType {
            std::string Verb;  - simplified verb the game model understands
            std::string OriginalVerb; - If the word was a synonym of the Verb possibly return for displaying later when returning the result to the player,l possible added snark?
            std::string DirectObject; - The entity that is the subject of the action: Command: look at table -> Verb: Look, DirectObject: Table
            std::vector<std::string> IndirectObjects; - Possible list of secondary entities required for an action
        };

        Command: Break chest with axe
        Interpretation:
            Verb: use
            OriginalVerb: break
            DirectObject: chest
            IndirectObjects[0]: axe

            - Checks could then be preformed to see if:
                a) The player has an Axe
                b) The state of the Chest: Is it open already? - Is it even locked? - Did the player check?  all flags that could shape the response given back to the player
                c) Can the axe even damage the chest etc.
                Finally the result is either pass or fail with the added benefit of immersive language/snarky comments to go with that.
                
                "After breaking into the chest, you relize it was unlocked this entire time."
                "You hit the metal chest with the axe, but it doesnt seem to have any effect"
                    - Player does it again?
                "You keep hitting the chest with the axe until you become tired. I dont think this is going to work" - Then provided a hint? "Maybe you should look for a key?"

        As the interpreter is parsing the input, it should ignore words that dont help determing the intent of an action ie(articles, extra adjectives etc):
        
        Building off the last example:
        Command: Break the damned chest with my axe
        Utimately this should result in the same interpretation:
        Interpretation:
            Verb: use
            OriginalVerb: break
            DirectObject: chest
            IndirectObjects[0]: axe

        One way that comes to mind in doing this will be to have the game build a list of valid "nouns" that can be acted upon.  This is then passed over to the input processor
        for use in validating input.

        Another way to handle this would be in the games grammar and syntax
        use axe on chest - breaks the chest open
        hit chest with axe - same

        For error handling/snark factor - if an input has words contained in a "profanity" list and no defined action verb etc.  The error could be "You sounds angry, you may want to take a breath (Remember HELP is always available if you get stuck)."  
            This hinting that the player should check the help for command suggestions etc, this could also be taylored in situations where certain items are needed for a puzzle etc.  "The door has an old lock on it with the imprint of a star above it"
            When the player has a "key" in their inventory that has a star on it.  The game should also be flexable enough to understand if the player were to say either use key on door, use the star key on the door etc.
**/
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

