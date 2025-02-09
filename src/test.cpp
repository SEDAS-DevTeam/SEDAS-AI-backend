/*
    Sample testing file to test all AI models
*/
#include "./include/utils.hpp"

#include "./PlaneResponse/voice_recog.hpp"
#include "./PlaneResponse/process.hpp"
#include "./PlaneResponse/classify.hpp"
#include "./PlaneResponse/speech_synth.hpp"

#include "./include/record.hpp"
#include "./include/keypress.hpp"

#include "./lib/json/single_include/nlohmann/json.hpp"
using json = nlohmann::json;

static void signal_handler(int signal){
    if (signal == SIGINT){
        running = false;
    }
}

json load_config(std::string config_path){
    std::ifstream config_file(config_path);
    return json::parse(config_file);
}


int main(int argc, char* argv[]){
    // number of arguments will always be 5
    std::array<std::string, 5> output = process_args(argv);
    
    const std::string asr_path = output[0];
    const std::string tts_path = output[1];
    const std::string config_path = output[2];
    const std::string temp_out_path = output[3];

    Logger logger(temp_out_path); // set primary event logger > logs into file

    // load configurations
    std::string classify_path = fs::path(config_path + "/config_classify.json");
    std::string synth_path = fs::path(config_path + "/config_synth.json");
    std::string response_path = fs::path(config_path + "/config_response.json");

    json config_classify = load_config(classify_path);
    json config_synth = load_config(synth_path);
    json config_response = load_config(response_path);

    Recorder recorder(temp_out_path); // setup recording handler through keypress
    recorder.initialize();

    Recognizer recognizer(asr_path, temp_out_path); // setup main ASR
    
    Processor processor; // setup processor
    Classifier classifier; // setup classifier
    classifier.set_rules(config_classify);

    Synthesizer synthesizer(tts_path, temp_out_path); // setup speech synthesizer
    synthesizer.setup_model_registry();
    synthesizer.setup_responses(config_response);

    Detect_ncurses keypress_detector; // detecting keypress
    keypress_detector.setup();

    synthesizer.init_pseudopilot("CBA1127", 0.5f); // TODO: just a sample how should the pseudopilot be initialized

    keypress_detector.mainloop(recorder,
                      recognizer,
                      processor,
                      classifier,
                      synthesizer,
                      logger);

    
    // TODO: no usage for this feature (right now)
    // str_matrix install_list = check_models(config_synth);
    // refetch_missing(install_list);

    synthesizer.remove_pseudopilot("CBA1127");


    logger.log("Program terminated");
    logger.end();
    std::cout << "Main program terminated." << std::endl;
    return 0;
}