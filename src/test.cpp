/*
    Sample testing file to test all AI models
*/
#include "./lib/json/single_include/nlohmann/json.hpp"
#include "./include/utils.hpp"

#include "./PlaneResponse/voice_recog.hpp"
#include "./PlaneResponse/process.hpp"
#include "./PlaneResponse/classify.hpp"
#include "./PlaneResponse/speech_synth.hpp"

#include "./include/record.hpp"


static void signal_handler(int signal){
    if (signal == SIGINT){
        running = false;
    }
}

int main(){
    // set sigint for graceful stop // TODO: remove (we are now stopping using q button)
    std::signal(SIGINT, signal_handler);

    Logger logger; // set primary event logger > logs into file

    // load configurations
    std::string classify_path = main_path / fs::path("PlaneResponse/config/config_classify.json");
    std::string synth_path = main_path / fs::path("PlaneResponse/config/config_synth.json");

    json config_classify = load_config(classify_path);
    json config_synth = load_config(synth_path);

    Recorder recorder; // setup recording handler through keypress
    recorder.initialize();
    Recognizer recognizer; // setup main ASR
    
    Processor processor; // setup processor
    Classifier classifier; // setup classifier
    classifier.set_rules(config_classify);

    Synthesizer synthesizer; // setup speech synthesizer
    synthesizer.setup_model_registry();

    synthesizer.init_pseudopilot("OKL4545", 0.5f); // TODO: just a sample how should the pseudopilot be initialized

    setup_ncurses();
    keypress_mainloop(recorder,
                      recognizer,
                      processor,
                      classifier,
                      synthesizer,
                      logger);

    
    // TODO: no usage for this feature (right now)
    // str_matrix install_list = check_models(config_synth);
    // refetch_missing(install_list);

    synthesizer.remove_pseudopilot("OKL4545");


    logger.log("Program terminated");
    logger.end();
    std::cout << "Main program terminated." << std::endl;
    return 0;
}