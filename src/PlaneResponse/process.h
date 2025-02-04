
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

        std::vector<std::string> extract_callsign(std::string input){
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

        std::vector<std::string> extract_values(std::string input){
            std::vector<std::string> values;

            bool on_nato = false;
            bool on_num = false;
            
            std::stringstream ss(input);
            std::string chunk;
            // take all necessary values (numerical or nato alpha)
            while (ss >> chunk){
                
            }

            return values;
        }

    public:
        std::vector<std::string> run(std::string input, Logger &logger){
            std::vector<std::string> callsign_data = extract_callsign(input);
            std::vector<std::string> values = extract_values(callsign_data[1]);

            logger.log("Plane callsign: " + callsign_data[0]);
            return { callsign_data[0] };
        }
};