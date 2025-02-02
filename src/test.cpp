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

// initialize threads
ProcessData text_process;
SpeechSynthesis speech_synth;

static void signal_handler(int signal){
    if (signal == SIGINT){
        running = false;
    }
}

int main(){
    // set sigint for graceful stop // TODO: maybe remove (we are now stopping using q button)
    std::signal(SIGINT, signal_handler);

    Logger logger; // set primary event logger > logs into file

    // load configurations
    std::string process_path = main_path / fs::path("PlaneResponse/config/config_process.json");
    std::string synth_path = main_path / fs::path("PlaneResponse/config/config_synth.json");

    json config_process = load_config(process_path);
    json config_synth = load_config(synth_path);

    // initialize models
    speech_synth.setup_model_registry();

    Recorder recorder; // recording handler through keypress
    Recognizer recognizer; // main ASR
    recorder.initialize();
    
    setup_ncurses();
    
    // TODO: no usage for this feature (right now)
    // str_matrix install_list = check_models(config_synth);
    // refetch_missing(install_list);

    std::thread thread_process(&ProcessData::run, &text_process);
    std::thread thread_synth(&SpeechSynthesis::run, &speech_synth);

    // TODO: just a sample how should the pseudopilot be initialized
    speech_synth.init_pseudopilot("OKL4545", 0.5f);

    text_process.start();
    speech_synth.start();

    // main loop to keep everything running
    keypress_mainloop(recorder,
                      recognizer,
                      logger);

    text_process.stop();
    speech_synth.stop();

    speech_synth.cleanup();

    process_queue.terminate();
    synth_queue.terminate();

    thread_process.join();
    thread_synth.join();

    // TODO: same
    speech_synth.remove_pseudopilot("OKL4545");

    logger.log("Program terminated");
    logger.end();
    return 0;
}