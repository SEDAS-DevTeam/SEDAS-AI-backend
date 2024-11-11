class SpeechSynthesis : public SEDThread {
    private:
        std::string COMMAND_STREAM   = main_path + "PlaneResponse/model/speech_synth/piper";
        std::string COMMAND_MODEL_DIR    = main_path + "PlaneResponse/model/speech_synth/model_source";
        std::string COMMAND_TEMP_OUT = main_path + "PlaneResponse/temp_out/out.wav";
        std::vector<std::vector<std::string, std::string>> model_registry;

        void process_synthesis(){
            std::string input = synth_queue.get_element();
            //std::cout << "Got input2: " << input << std::endl;

            //make_command_synth(input);
            //add_noise();
            //make_command_play();
        }

        void add_noise(){

        }

        std::string choose_pseudopilot(const std::string& type){
            if (type == "random"){
                return model_registry[0][1]; //TODO
            }
            else if (type == "designated"){

            }
        }

        std::string make_command_synth(const std::string& input){
            std::string command_result = "echo '" + input + "' | "; //source text
            
            command_result += COMMAND_STREAM;
            command_result += " --model " + choose_pseudopilot("random"); //TODO: add pilot designated system
            command_result += " --output_file " + COMMAND_TEMP_OUT;
            return command_result;
        }

        std::string make_command_play(){
            return "aplay " + COMMAND_TEMP_OUT;
        }

    public:
        void setup_model_registry(){
            // Load all models
            for (const auto& model_path : fs::directory_iterator(COMMAND_MODEL_DIR)){
                std::string model_json_filename = model_path.path().filename();
                std::string model_onnx_filename = model_json_filename;

                if (model_path.is_regular_file() && model_json_filename.find(".json") != std::string::npos){
                    // search only by .json files, then find .onnx file pairs
                    size_t pos = model_json_filename.find(".json");
                    if (pos != std::string::npos) { // Check if substring exists
                        model_onnx_filename.erase(pos, model_onnx_filename.length()); // Remove the substring
                    }
                    std::string onnx_record = COMMAND_MODEL_DIR + "/" + model_onnx_filename;
                    std::string json_record = COMMAND_MODEL_DIR + "/" + model_json_filename;

                    model_registry.push_back({json_record, onnx_record});
                }
            }
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