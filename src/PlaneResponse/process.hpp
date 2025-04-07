#pragma once

#include <string>
#include "../include/utils.hpp"

class Processor {
    private:
        std::string per_word_loop(std::string input, str_map mod_map, std::string(*func)(std::string, str_map)){
            std::stringstream ss(input);
            std::string restructured;

            std::string chunk;
            while (ss >> chunk){
                restructured += func(chunk, mod_map);
            }

            return restructured;
        }

        static std::string convert(std::string chunk, str_map conversion_table){
            if (conversion_table.find(chunk) != conversion_table.end()){ // found key
                return conversion_table[chunk] + " ";
            }
            else{return chunk + " ";}
        }

        bool is_number(const std::string& input) {
            return !input.empty() && std::all_of(input.begin(), input.end(), ::isdigit);
        }

        std::string remove_substr(std::string input, std::string substring){
            size_t pos = input.find(substring);
            if (pos != std::string::npos){
                input.erase(pos, substring.length() + 1);
            }
            return input;
        }

        std::pair<std::string, std::string> extract_callsign(std::string input){
            std::string callsign;
            std::string other_words = input;

            // convert all to nato alphabet
            std::stringstream ss(input);
            std::string chunk;
            while (ss >> chunk){
                if (nato_map.find(chunk) != nato_map.end()){ // found nato
                    callsign += nato_map[chunk];
                    other_words = remove_substr(other_words, chunk);
                }
                else{ // look for num
                    // written in numerical expression (joined)
                    if (is_number(chunk) && chunk.length() > 1){
                        callsign += chunk;
                        other_words = remove_substr(other_words, chunk);
                    }
                    // written in numerical expression (not joined)
                    else if (is_number(chunk) && chunk.length() == 1){
                        callsign += chunk;
                        other_words = remove_substr(other_words, chunk);
                    }
                    // written in word expression
                    else if (num_map.find(chunk) != num_map.end()){
                        callsign += num_map[chunk];
                        other_words = remove_substr(other_words, chunk);
                    }
                    else return { callsign, other_words }; // end of callsign word
                }
            }
            return { callsign, other_words };
        }

        std::pair<std::vector<std::string>, std::string> extract_values(std::string input){
            std::vector<std::string> values;
            std::string other_words = input;

            std::string curr_nato = "";
            std::string curr_num = "";
            
            std::stringstream ss(input);
            std::string chunk;
            // take all necessary values (numerical or nato alpha)
            while (ss >> chunk){
                // nato
                if (nato_map.find(chunk) != nato_map.end()){
                    curr_nato += nato_map[chunk];
                    other_words = remove_substr(other_words, chunk);
                }
                else{
                    if (curr_nato.length() > 0) values.push_back(curr_nato);
                    curr_nato = "";
                }

                // numeric
                // written in numerical expression (joined)
                if (is_number(chunk) && chunk.length() > 1){
                    curr_num += chunk;
                    other_words = remove_substr(other_words, chunk);
                }
                // written in numerical expression (not joined)
                else if (is_number(chunk) && chunk.length() == 1){
                    curr_num += chunk;
                    other_words = remove_substr(other_words, chunk);
                }
                // written in word expression
                else if (num_map.find(chunk) != num_map.end()){
                    curr_num += num_map[chunk];
                    other_words = remove_substr(other_words, chunk);
                }
                else{
                    if (curr_num.length() > 0) values.push_back(curr_num);
                    curr_num = "";
                }

            }

            // buffers not flushed
            if (curr_nato.length() > 0) values.push_back(curr_nato);
            if (curr_num.length() > 0) values.push_back(curr_num);

            return { values, other_words };
        }

    public:
        std::pair<std::vector<std::string>, std::vector<std::string>> run(std::string input, Logger &logger){
            auto [callsign, value_extractor_input] = extract_callsign(input);
            auto [values, classifier_input] = extract_values(value_extractor_input);
            
            if (values.size() == 0) values.push_back(""); // in case of incorrect segmentation push empty string

            return { {callsign, classifier_input}, values };
        }
};