class Recognizer {
    private:
        const std::string COMMAND_BIN = main_path + "PlaneResponse/models/asr/whisper-cli";
        const std::string COMMAND_MODEL = main_path + "PlaneResponse/models/asr/atc-whisper-ggml.bin";
        const std::string COMMAND_INP = main_path + "PlaneResponse/temp_out/controller.wav"; 

        std::string process_stdout(const char* data, Logger &logger){
            std::string data_str(data);
            logger.log("Model out: " + data_str);

            // some processing
            size_t endBracket = data_str.find("]");
            if (endBracket == std::string::npos) return data_str; // If bracket not found, return as is
            
            std::string main_trans = data_str.substr(endBracket + 1); // Extract after the bracket
            size_t firstChar = main_trans.find_first_not_of(" ");
            if (firstChar != std::string::npos) {
                main_trans = main_trans.substr(firstChar);
            }

            return main_trans;
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

            return out;
        }
};