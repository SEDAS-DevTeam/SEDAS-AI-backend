class VoiceRecognition : public SEDThread {
    private:
        std::string COMMAND_STREAM = main_path + "PlaneResponse/model/stream";
        std::string COMMAND_MODEL  = main_path + "PlaneResponse/model/ggml-base.en.bin";
        
        // model configuration
        uint32_t t = 8;
        uint64_t step = 2500;
        uint64_t length = 5000;

        static const size_t ignore_len = 4;
        std::string ignore[ignore_len] = {
            "[",
            "]",
            "(",
            ")"
        };

        std::string make_command(){
            std::string command_result = COMMAND_STREAM + " -m " + COMMAND_MODEL;
            
            // arg T
            command_result += " -t " + std::to_string(t);
            command_result += " --step " + std::to_string(step);
            command_result += " --length " + std::to_string(length);
            return command_result;
        }

        void process_stdout(const char* data){
            std::string data_str(data);
            bool found = false;

            std::cout << data_str << std::flush;
            for (auto i = 0; i < ignore_len; i++){
                if (data_str.find(ignore[i]) != std::string::npos){
                    found = true;
                }
            }

            if (!found){
                std::cout << data_str << std::flush;
                result += data_str;
            }
        }

    public:
        std::string result;
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