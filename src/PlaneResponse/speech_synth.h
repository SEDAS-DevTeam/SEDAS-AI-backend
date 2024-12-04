std::string COMMAND_STREAM   = main_path + "PlaneResponse/model/speech_synth/piper";
std::string COMMAND_TEMP_OUT = main_path + "PlaneResponse/temp_out/";

struct WavHeader {
    char riff[4];                // "RIFF"
    uint32_t chunkSize;          // File size - 8 bytes
    char wave[4];                // "WAVE"
    char fmt[4];                 // "fmt "
    uint32_t subChunk1Size;      // 16 for PCM
    uint16_t audioFormat;        // PCM = 1
    uint16_t numChannels;        // Mono = 1, Stereo = 2
    uint32_t sampleRate;         // 44100, 48000, etc.
    uint32_t byteRate;           // SampleRate * NumChannels * BitsPerSample / 8
    uint16_t blockAlign;         // NumChannels * BitsPerSample / 8
    uint16_t bitsPerSample;      // 8, 16, 24, or 32
    char data[4];                // "data"
    uint32_t dataSize;           // Audio data size
};

void add_noise_to_wav(const std::string& input_file, float noiseLevel) {
    // Open input file
    std::ifstream in(input_file, std::ios::binary);
    if (!in) {
        std::cerr << "Error: Cannot open input file\n";
        return;
    }

    // Read header
    WavHeader header;
    in.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Verify WAV format
    if (std::string(header.riff, 4) != "RIFF" || std::string(header.wave, 4) != "WAVE") {
        std::cerr << "Error: Invalid WAV file\n";
        return;
    }

    // Read audio data
    std::vector<int16_t> audioData(header.dataSize / sizeof(int16_t));
    in.read(reinterpret_cast<char*>(audioData.data()), header.dataSize);
    in.close();

    // Add noise
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-noiseLevel, noiseLevel);

    for (auto& sample : audioData) {
        float noise = dist(gen) * INT16_MAX * 0.1f; // Scale noise to 16-bit range
        int32_t noisySample = static_cast<int32_t>(sample) + static_cast<int32_t>(noise);

        // Clamp to valid range for 16-bit audio
        if (noisySample > INT16_MAX) noisySample = INT16_MAX;
        if (noisySample < INT16_MIN) noisySample = INT16_MIN;

        sample = static_cast<int16_t>(noisySample);
    }

    // Write output file
    std::ofstream out(input_file, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot open output file\n";
        return;
    }

    // Write header
    out.write(reinterpret_cast<char*>(&header), sizeof(header));

    // Write modified audio data
    out.write(reinterpret_cast<char*>(audioData.data()), header.dataSize);
    out.close();
}

class Pseudopilot {
    private:
        void synth(const std::string& input){
            std::string command_result = "echo '" + input + "' | "; //source text
            
            command_result += COMMAND_STREAM;
            command_result += " --model " + onnx_path;
            command_result += " --output_file " + COMMAND_TEMP_OUT + callsign + ".wav";
            
            execute_command(command_result.c_str());
        }

        void play(){
            std::string result = "aplay " + COMMAND_TEMP_OUT + callsign + ".wav";
            execute_command(result.c_str());
        }

        void add_noise(){
            add_noise_to_wav(COMMAND_TEMP_OUT + callsign + ".wav", noise_intensity);
        }

    public:
        std::string callsign;
        float noise_intensity;

        std::string onnx_path;
        std::string json_path;

        Pseudopilot(std::string init_callsign, float init_intensity){
            callsign = init_callsign;
            noise_intensity = init_intensity;
        }

        void assign_voice(std::string onnx_config, std::string json_config){
            onnx_path = onnx_config;
            json_path = json_config;
        }

        void respond(std::string &query){
            synth(query);
            add_noise();
            play();
        }
};

class SpeechSynthesis : public SEDThread {
    private:
        std::string COMMAND_MODEL_DIR = main_path + "PlaneResponse/model/speech_synth/model_source";
        str_matrix model_registry; // register all models
        str_matrix remaining_models; // keep track of what models were registered

        std::vector<Pseudopilot> pseudopilot_registry;

        void process_synthesis(){
            std::string input = synth_queue.get_element();
            //std::cout << "Got input2: " << input << std::endl;
        }

        std::string choose_pseudopilot(const std::string& type){
            if (type == "random"){
                //return model_registry[0][1]; //TODO
            }
            else if (type == "designated"){

            }
        }

        std::tuple<std::string, std::string> choose_random_configuration(){
            int idx = rand_choice(remaining_models.size());
            std::string json_record = remaining_models[idx][0];
            std::string onnx_record = remaining_models[idx][1];

            remaining_models.erase(remaining_models.begin() + idx);

            return {json_record, onnx_record};
        }

    public:
        void setup_model_registry(){
            // Load all models
            for (const auto& model_path : fs::directory_iterator(COMMAND_MODEL_DIR)){
                std::string model_json_filename = model_path.path().filename();
                std::string model_onnx_filename = model_json_filename;

                if (model_path.is_regular_file() && model_json_filename.find(".json") != std::string::npos){
                    // search only by .json files, then find .onnx file pairs
                    size_t pos = model_onnx_filename.find(".json");
                    if (pos != std::string::npos) { // Check if substring exists
                        model_onnx_filename.erase(pos, model_onnx_filename.length()); // Remove the substring
                    }
                    std::string onnx_record = COMMAND_MODEL_DIR + "/" + model_onnx_filename;
                    std::string json_record = COMMAND_MODEL_DIR + "/" + model_json_filename;

                    model_registry.push_back({json_record, onnx_record});
                    remaining_models.push_back({json_record, onnx_record});
                }
            }
        }

        /* Pilot commands */
        void init_pseudopilot(std::string callsign, float noise_intensity){
            // setup pseudopilot
            Pseudopilot spec_pseudopilot("OKL4545", 0.6);

            auto [json, onnx] = choose_random_configuration();
            spec_pseudopilot.assign_voice(onnx, json);
            pseudopilot_registry.push_back(spec_pseudopilot);
        }

        void pseudopilot_respond(std::string callsign, std::string input){
            for (Pseudopilot pseudopilot : pseudopilot_registry){
                if (pseudopilot.callsign == callsign){
                    pseudopilot.respond(input);
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