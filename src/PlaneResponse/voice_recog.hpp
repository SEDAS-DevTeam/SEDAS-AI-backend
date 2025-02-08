class Recognizer {
    private:
        const std::string COMMAND_BIN = main_path + "PlaneResponse/models/asr/whisper-cli";
        const std::string COMMAND_MODEL = main_path + "PlaneResponse/models/asr/atc-whisper-ggml.bin";
        const std::string COMMAND_INP = main_path + "PlaneResponse/temp_out/controller.wav"; 
        
        std::string extract_after_bracket(std::string input){
            size_t endBracket = input.find("]");
            if (endBracket == std::string::npos) return input; // If bracket not found, return as is
            else return input.substr(endBracket + 1); // Extract after the bracket
        }

        std::string remove_newlines(std::string input){
            std::string output;
            // remove newlines in string
            for (int i = 0; i < input.length(); i++){
                if (i <= input.length() - 2){
                    if (input[i] != '\\' || input[i + 1] != 'n'){
                        output += input[i];
                    }
                }
            }
            return output;
        }

        std::string to_lower(std::string input){
            std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c){ return std::tolower(c); });
            return input;
        }

        std::string remove_indentation(std::string input){
            for (int i = 0; i < input.length(); i++){
                if (!isspace(input[i])){
                    return input.substr(i);
                }
            }
            return input;
        }

        std::string remove_char(char char_to_find, std::string input){
            input.erase(std::remove(input.begin(), input.end(), char_to_find), input.end());
            return input;
        }

        std::string process_stdout(const char* data, Logger &logger){
            std::string data_str(data);
            if (data_str == "\n"){ // output is blank
                return "";
            }

            logger.log("Model out: " + data_str);

            // some processing
            data_str = extract_after_bracket(data_str);
            data_str = remove_newlines(data_str);
            data_str = remove_indentation(data_str);
            data_str = to_lower(data_str);

            // useless character removal
            data_str = remove_char('.', data_str);
            data_str = remove_char(',', data_str);
            data_str = remove_char('-', data_str);

            return data_str;
        }

    public:
        std::string run(Logger &logger){
            std::string command = COMMAND_BIN + " -m " + COMMAND_MODEL + " " + COMMAND_INP;
            std::array<char, 128> buffer;

            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }

            std::string out = "";
            while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr && running) {
                out += process_stdout(buffer.data(), logger);
            }

            logger.log("Transcription: " + out);
            return out;
        }
};