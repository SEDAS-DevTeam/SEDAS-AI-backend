
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

        std::string extract_callsign(std::string input){
            std::string callsign;

            // convert all to nato alphabet
            std::stringstream ss(input);
            std::string chunk;
            bool nato_stop = false;
            while (ss >> chunk){
                if (nato_map.find(chunk) != nato_map.end()){ // found nato
                    callsign += nato_map[chunk];
                }
                else{ // look for num
                    // written in numerical expression (joined)
                    if (is_number(chunk) && chunk.length() > 1){
                        callsign += chunk;
                        return callsign;
                    }
                    // written in numerical expression (not joined)
                    else if (is_number(chunk) && chunk.length() == 1) callsign += chunk;
                    // written in word expression
                    else if (num_map.find(chunk) != num_map.end()) callsign += num_map[chunk];
                    else return callsign; // end of callsign word
                }
            }
            return callsign;
        }

    public:
        std::vector<std::string> run(std::string input, Logger &logger){
            std::string callsign = extract_callsign(input);

            logger.log("Plane callsign: " + callsign);
            return { callsign };
        }
};