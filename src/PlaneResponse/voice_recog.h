class VoiceRecognition : public SEDThread {
    private:
        std::string COMMAND_STREAM = main_path + "PlaneResponse/model/stream";
        std::string COMMAND_MODEL  = main_path + "PlaneResponse/model/ggml-base.en.bin";
        
        // model configuration
        uint32_t t = 8;
        uint64_t step = 2500;
        uint64_t length = 5000;

        static const size_t ignore_len = 4;

        std::string make_command(){
            std::string command_result = COMMAND_STREAM + " -m " + COMMAND_MODEL;
            
            // arg T
            command_result += " -t " + std::to_string(t);
            command_result += " --step " + std::to_string(step);
            command_result += " --length " + std::to_string(length);
            return command_result;
        }

        std::string remove_str(std::string string, std::string searched_string){
            while (string.find(searched_string) != std::string::npos){
                auto word_start = string.find(searched_string);
                auto word_len = searched_string.size();

                string.erase(word_start, word_len);
            }
            return string;
        }

        void process_stdout(const char* data){
            std::string data_str(data);
            bool found = false;

            // remove [Start speaking] header
            data_str = remove_str(data_str, "[Start speaking]");

            // remove unnecessary newlines
            data_str = remove_str(data_str, "\n");

            data_str = remove_str(data_str, "\033[2K\r");
            data_str = remove_str(data_str, "\033[2K\rSPE");

            // remove leading spaces
            bool was_space = false;
            uint32_t start_index = 0;
            uint32_t stop_index = 0;
            for (auto i = 0; data_str.size(); i++){
                bool is_space = std::isspace(data_str[i]);
                if (was_space && is_space){ start_index = i; } // First section of spaces
                if (was_space && !is_space){
                    stop_index = i;
                    data_str.erase(start_index, stop_index - start_index);
                } // Last section of spaces
                was_space = is_space;
            }            

            if (data_str.size() == 0){
                std::cout << "Recog ignored" << std::endl;
            }
            else{
                std::cout << "Recog output: " << data_str;
                std::cout << data_str.size() << std::endl;
            }
        }

    public:
        std::queue<std::string> out;

        void run(){
            std::array<char, 128> buffer;

            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(make_command().c_str(), "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr && running) {
                process_stdout(buffer.data());
            }
        }
};