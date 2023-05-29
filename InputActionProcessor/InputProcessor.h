#ifndef INPUTPROCESSOR_H__
#define INPUTPROCESSOR_H__

#include<vector>
#include<unordered_map>
#include<algorithm>
#include<string>
#include <sstream>

// Inlcude Niels Lohmann's JSON header
#include"nlohmann/json.hpp"
using json = nlohmann::json;

class InputProcessor
{
public:
	InputProcessor() {};
	~InputProcessor() {};

private:
	std::unordered_map<std::string, std::vector<std::string>> commandMap = {};
};

#endif