class SpeechSynthesis : public SEDThread {
    private:
        std::string COMMAND_STREAM = main_path + "PlaneResponse/model/speech_synth/piper";
        std::string COMMAND_MODEL  = main_path + "PlaneResponse/model/speech_synth/model_source";
        std::map<int, std::string> model_registry;

        void process_synthesis(){
            std::string input = synth_queue.get_element();
            std::cout << "Got input2: " << input << std::endl;
        }

        std::string make_command(std::string input){
            std::string command_result = "echo '" + input + "' | "; //source text
            
            command_result += COMMAND_STREAM;
            command_result += " --model " + COMMAND_MODEL;
            command_result += " --output-raw |";
            //Add the rest
        }
    public:
        void setup_model_registry(){
            // Load all models
        }

        void run(){
            while (running){
                synth_queue.wait();
                process_synthesis();
            }
        }
};