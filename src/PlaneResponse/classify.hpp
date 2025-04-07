#pragma once

#include <vector>
#include <string>
#include <map>
#include <sstream>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Classifier{
    private:
        std::vector<std::string> read_json_array(json obj){
            std::vector<std::string> out;
            for (const auto& elem : obj) out.push_back(elem);

            return out;
        }

    public:
        std::map<std::string, std::vector<std::string>> classifier_rules;

        std::string process_rule_based(std::string input){
            /*
            This classifier is just for testing with primitive rule-based classifications
            in future, this will definitely be changed (TODO)
            */

            auto check_substr = [](std::string str, std::string substr){
                if (str.find(substr) != std::string::npos) return true;
                else return false;
            };

            std::string command_out;
            for (const auto& rule : classifier_rules) {
                bool passing = true;
                for (int i = 0; i < rule.second.size(); i++){
                    if (rule.second[i].find(',') != std::string::npos){
                        // there are multiple OR cases
                        std::string or_cases = rule.second[i];

                        std::vector<std::string> or_cases_vector;
                        std::stringstream ss(or_cases);
                        std::string or_case;

                        bool internal_passing = false;

                        while (std::getline(ss, or_case, ',')){
                            or_cases_vector.push_back(or_case);
                        }

                        for (int i = 0; i < or_cases_vector.size(); i++){
                            if (check_substr(input, or_cases_vector[i])){
                                internal_passing = true;
                                break;
                            }
                        }

                        if (!internal_passing) passing = false;
                    }
                    else{
                        // only one case
                        if (!check_substr(input, rule.second[i])){
                            passing = false;
                        }
                    }
                }

                if (passing){
                    command_out = rule.first;
                    break;
                }
            }

            return command_out;
        }

        void set_rules(json rules){
            for (const auto& rule : rules["dictionary"]){
                std::vector<std::string> keywords = read_json_array(rule["keywords"]);
                std::string command = rule["command"];

                classifier_rules[command] = keywords;
            }
        }

        std::vector<std::string> run(std::string input){
            std::vector<std::string> commands;

            std::string command_out = process_rule_based(input);
            commands.push_back(command_out);

            return commands;
        }
};