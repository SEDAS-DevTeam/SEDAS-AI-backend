#include <random>
#include <map>

#include "../lib/json/single_include/nlohmann/json.hpp"
using json = nlohmann::json;

std::string COMMAND_STREAM   = main_path + "PlaneResponse/models/tts/piper";
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
            std::string callsign_dist;
            for (int i = 0; i < callsign.size(); i++){
                bool found_corr_char = false;

                // check for nato
                for (auto& pair : nato_map){
                    if (pair.second[0] == callsign[i]){
                        callsign_dist += (pair.first + " ");
                        found_corr_char = true;
                        break;
                    }
                }
                if (found_corr_char) { continue; }

                // check for nums
                for (auto& pair : num_map){
                    if (pair.second[0] == callsign[i]){
                        callsign_dist += (pair.first + " ");
                        break;
                    }
                }
            }

            std::string command_result = "echo '" + callsign_dist + ", " + input + "' | "; //source text
            
            command_result += COMMAND_STREAM;
            command_result += " --model " + onnx_path;
            command_result += " --output_file " + COMMAND_TEMP_OUT + callsign + ".wav";
            
            execute_command(command_result.c_str());
        }

        void play(){
            std::string result = "pw-play " + COMMAND_TEMP_OUT + callsign + ".wav";
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

class Synthesizer{
    private:
        std::string COMMAND_MODEL_DIR = main_path + "PlaneResponse/models/tts/voices";
        str_matrix model_registry; // register all models
        str_matrix remaining_models; // keep track of what models were registered
        std::map<std::string, std::string> command_responses;

        std::vector<Pseudopilot> pseudopilot_registry;

        std::tuple<std::string, std::string> choose_random_configuration(){
            int idx = rand_choice(remaining_models.size());
            std::string json_record = remaining_models[idx][0];
            std::string onnx_record = remaining_models[idx][1];

            remaining_models.erase(remaining_models.begin() + idx);

            return {json_record, onnx_record};
        }

        std::string convert_value(std::string input){
            std::string out = "";
            for (int i = 0; i < input.length(); i++){
                out += num_map2[std::string(1, input[i])] + ", ";
            }

            return out;
        }
    public:
        void run(std::string command,
                 std::string value,
                 std::string callsign,
                 Logger& logger){
                    
            std::string command_fin = command_responses[command] + " " + convert_value(value);
            logger.log("Pseudopilot response: " + command_fin);
            logger.log("Callsign: " + callsign);
            pseudopilot_respond(callsign, command_fin);
        }

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

        void setup_responses(json responses){
            for (const auto& response: responses["responses"]){
                std::string command = response["command"];
                std::string output = response["output"];

                command_responses[command] = output;
            }
        }

        /* Pilot commands */
        void init_pseudopilot(std::string callsign, float noise_intensity){
            // setup pseudopilot
            Pseudopilot spec_pseudopilot(callsign, 0.6);

            auto [json, onnx] = choose_random_configuration();
            spec_pseudopilot.assign_voice(onnx, json);
            pseudopilot_registry.push_back(spec_pseudopilot);
        }

        void remove_pseudopilot(std::string callsign){
            // remove pseudopilot record
            for (int i = 0; i < pseudopilot_registry.size(); i++){
                if (pseudopilot_registry[i].callsign == callsign){
                    pseudopilot_registry.erase(pseudopilot_registry.begin() + i);
                }
            }

            // remove pseudopilot trace
            std::string trace = COMMAND_TEMP_OUT + callsign + ".wav";
            if (fs::exists(trace)){ fs::remove(trace); }
        }

        void cleanup(){
            pseudopilot_registry.clear();

            if (fs::exists(COMMAND_TEMP_OUT) && fs::is_directory(COMMAND_TEMP_OUT)){
                for (auto& entry : fs::directory_iterator(COMMAND_TEMP_OUT)){
                    if (search_string(entry.path().u8string(), ".gitkeep")){ continue; }
                    fs::remove(entry);
                }
            }
        }

        void pseudopilot_respond(std::string callsign, std::string input){
            for (Pseudopilot pseudopilot : pseudopilot_registry){
                if (pseudopilot.callsign == callsign){
                    pseudopilot.respond(input);
                }
            }
        }
};