#include <vector>
#include <string>
#include <map>

#include "../lib/json/single_include/nlohmann/json.hpp"
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
                    if (!check_substr(input, rule.second[i])){
                        passing = false;
                        break;
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