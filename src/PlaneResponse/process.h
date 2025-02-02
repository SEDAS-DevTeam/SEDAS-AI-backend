
class Processor {
    private:
        std::string parse_input(std::string input){
            std::stringstream ss(input);
            std::string nato_converted_str;
            
            std::string word;
            while(ss >> word){
                if(nato_map.find(word) != nato_map.end()){ // found nato key
                    nato_converted_str += nato_map[word] + " ";
                }
                else{
                    nato_converted_str += word + " ";
                }
            }

            return nato_converted_str;
        }

    public:
        std::string run(std::string input, Logger &logger){
            input = to_lower(input);
            input = parse_input(input);

            logger.log("Processed: " + input);
        }
};