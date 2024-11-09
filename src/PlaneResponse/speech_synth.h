class SpeechSynthesis : public SEDThread {
    private:
        std::string COMMAND_STREAM   = main_path + "PlaneResponse/model/speech_synth/piper";
        std::string COMMAND_MODEL    = main_path + "PlaneResponse/model/speech_synth/model_source";
        std::string COMMAND_TEMP_OUT = main_path + "PlaneResponse/temp_out/out.wav";
        std::map<int, std::string> model_registry;

        void process_synthesis(){
            std::string input = synth_queue.get_element();
            std::cout << "Got input2: " << input << std::endl;

            //make_command_synth(input);
            //add_noise();
            //make_command_play();
        }

        void add_noise(){

        }

        std::string make_command_synth(std::string input){
            std::string command_result = "echo '" + input + "' | "; //source text
            
            command_result += COMMAND_STREAM;
            command_result += " --model " + COMMAND_MODEL;
            command_result += " --output_file " + COMMAND_TEMP_OUT;
            return command_result;
        }

        std::string make_command_play(){
            return "aplay " + COMMAND_TEMP_OUT;
        }

    public:
        void setup_model_registry(){
            // Load all models
        }

        void run(){
            while (running){
                synth_queue.wait();
                if (running){ // for last resort notification (when queue is getting emptied)
                    process_synthesis();
                }
            }
        }
};