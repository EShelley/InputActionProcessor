#ifndef INPUTPROCESSOR_H__
#define INPUTPROCESSOR_H__

#include<vector>
#include<unordered_map>
#include<algorithm>
#include<string>
#include<sstream>
#include<fstream>
#include<iostream>

// Inlcude Niels Lohmann's JSON header
#include"nlohmann/json.hpp"
using json = nlohmann::json;



// InputProcessor:
//  - Processes the raw input from player into a useful format
// 
// TODO: add ability to attach a logger eventually?
//
class InputProcessor
{
public:
	InputProcessor() {};
	~InputProcessor() {};

    // Loads the JSON data from commands.json directly into this->commandMap
    //
    // TODO: Implement valid verb lookup table?
    //
    bool loadMapFromJSON(const std::string& filename) {
        std::ifstream inputFile(filename);
        if (inputFile.is_open()) {
            json jsonData;
            inputFile >> jsonData;

            for (const auto& [key, value] : jsonData.items()) {
                if (value.is_array()) {
                    std::vector<std::string> values = value.get<std::vector<std::string>>();
                    this->commandMap[key] = values;
                }
            }

            inputFile.close();
            std::cout << "Map loaded from JSON file successfully.\n";
        }
        else {
            std::cerr << "Unable to open the file.\n";
            return false;
        }
        
        return true;
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
    std::string getAction(const std::string& input) {
        // Check if we have a valid commandMap
        if (this->commandMap.empty())
            return "ERROR: No command map";

        // Tokenize Input Sting
        std::vector<std::string> tokens = this->tokenizeInput(this->toLowercase(input));
        if (tokens.empty())
            return "Ok brain, tell me what to do please";

        for (const auto& actionSynonyms : this->commandMap) {
            const std::string& action = actionSynonyms.first;
            const std::vector<std::string>& synonyms = actionSynonyms.second;

            // Check if input matches the action or any of its synonyms
            auto it = std::find_if(synonyms.begin(), synonyms.end(), [&](const std::string& synonym) {
                return tokens[0].compare(synonym) == 0;  
                });

            if (it != synonyms.end()) {
                return action;
            }
        }

        // No match found
        return "ERROR: I dont Understand that Command";  // Or you can define an "unknown" action
    }

    

    // used for testing
    void DisplayCommands() {
        for (const auto& genCmd : this->commandMap) {
            std::cout << "  General Command: " << genCmd.first <<
                "\n============================\n";
            const std::vector<std::string>& synonyms = genCmd.second;
            for (const auto& syn : synonyms)
                std::cout << syn << '\n';

            std::cout << '\n';
        }
    }



private:
	std::unordered_map<std::string, std::vector<std::string>> commandMap = {};

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
};

#endif