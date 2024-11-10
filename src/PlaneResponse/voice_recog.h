class VoiceRecognition : public SEDThread {
    private:
        std::string COMMAND_STREAM = main_path + "PlaneResponse/model/voice_recog/stream_recog";
        std::string COMMAND_MODEL  = main_path + "PlaneResponse/model/voice_recog/ggml-small.bin";
        
        // model configuration
        uint32_t t = 8; // number of threads
        uint64_t length = 10000; // length of processed audio chunk
        float vth = 0.6;
        
        std::string convert_float(float spec_float){
            std::ostringstream out;
            out << std::setprecision(1) << spec_float;
            return out.str();
        }

        std::string make_command(){
            std::string command_result = COMMAND_STREAM + " -m " + COMMAND_MODEL;
            
            command_result += " -t " + std::to_string(t);
            command_result += " --length " + std::to_string(length);
            command_result += " -vth " + convert_float(vth);
            command_result += " --keep 0";
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

        std::string remove_leading_spaces(std::string string) {
            size_t start = string.find_first_not_of(' '); // Find the first non-space character
            return (start == std::string::npos) ? "" : string.substr(start);
        }

        std::string remove_fillers(std::string string, std::string char_comb){
            auto filler_start = string.find(char_comb[0]);
            auto filler_end = string.find(char_comb[1]);

            if (filler_start != std::string::npos && filler_end != std::string::npos){
                string.erase(filler_start, filler_end - filler_start + 1);
            }

            return string;
        }

        void process_stdout(const char* data){
            std::string data_str(data);
            bool found = false;

            data_str = remove_str(data_str, "[Start speaking]");
            data_str = remove_str(data_str, "\n");
            data_str = remove_str(data_str, "\033[2K\r");
            data_str = remove_str(data_str, "\033[2K\rSPE");
            data_str = remove_leading_spaces(data_str);
            data_str = remove_fillers(data_str, "()");
            data_str = remove_fillers(data_str, "[]");

            if (data_str.size() == 0){
                std::cout << "Recog ignored" << std::endl;
                if (!process_queue.is_empty()){
                    process_queue.notify();
                }
            }
            else{
                std::cout << "Recog output: " << data_str << std::endl;

                process_queue.add_element(data_str);
                process_queue.notify();
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