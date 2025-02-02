/*
    Sample testing file to test all AI models
*/
#include "./lib/json/single_include/nlohmann/json.hpp"
#include "./include/utils.h"

#include "./PlaneResponse/voice_recog.h"
#include "./PlaneResponse/process.h"
#include "./PlaneResponse/speech_synth.h"

#include "./include/record.h"
#include "./include/source_check.h"


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
    std::string process_path = main_path / fs::path("PlaneResponse/config/config_process.json");
    std::string synth_path = main_path / fs::path("PlaneResponse/config/config_synth.json");

    json config_process = load_config(process_path);
    json config_synth = load_config(synth_path);

    Recorder recorder; // setup recording handler through keypress
    recorder.initialize();
    Recognizer recognizer; // setup main ASR
    Processor processor; // setup processor
    Synthesizer synthesizer; // setup speech synthesizer
    synthesizer.setup_model_registry();
    synthesizer.init_pseudopilot("OKL4545", 0.5f); // TODO: just a sample how should the pseudopilot be initialized

    setup_ncurses();
    keypress_mainloop(recorder,
                      recognizer,
                      processor,
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