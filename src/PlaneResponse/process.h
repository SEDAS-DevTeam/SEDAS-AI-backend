
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

        static std::string remove_spaces(std::string input, str_map word_table){

        }

        std::string extract_callsign(std::string input){
            input = per_word_loop(input, nato_map, &convert); // convert words to nato
            input = per_word_loop(input, nato_map, &remove_spaces);
            input = remove_spaces(input, nato_map);

            return input;
        }

    public:
        std::string run(std::string input, Logger &logger){
            input = extract_callsign(input);

            logger.log("Processed: " + input);
        }
};