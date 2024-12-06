/*
    Sample testing file to test all AI models
*/
#include "./lib/json.hpp"
#include "./include/utils.h"

#include "./PlaneResponse/voice_recog.h"
#include "./PlaneResponse/process.h"
#include "./PlaneResponse/speech_synth.h"
#include "./PlaneResponse/source_check.h"

// initialize threads
VoiceRecognition voice_recog;
ProcessData text_process;
SpeechSynthesis speech_synth;

static void signal_handler(int signal){
    if (signal == SIGINT){
        voice_recog.stop();
        running = false;
    }
}

int main(){
    // set sigint for graceful stop
    std::signal(SIGINT, signal_handler);

    // models setup and initialization
    json config_recog = load_config("config_recog");
    json config_process = load_config("config_process");
    json config_synth = load_config("config_synth");

    // initialize models
    voice_recog.load_params(config_recog);
    str_matrix install_list = check_models(config_synth);
    refetch_missing(install_list);

    speech_synth.setup_model_registry();

    std::thread thread_recog(&VoiceRecognition::run, &voice_recog);
    std::thread thread_process(&ProcessData::run, &text_process);
    std::thread thread_synth(&SpeechSynthesis::run, &speech_synth);

    // TODO: just a sample how should the pseudopilot be initialized
    speech_synth.init_pseudopilot("OKL4545", 0.5f);

    voice_recog.start();
    text_process.start();
    speech_synth.start();

    /*
        A simple loop to keep everything running
    */
    while (running){
        sleep(1);
    }

    voice_recog.stop();
    text_process.stop();
    speech_synth.stop();

    speech_synth.remove_pseudopilot("OKL4545");
    speech_synth.cleanup();

    process_queue.terminate();
    synth_queue.terminate();

    thread_recog.join();
    thread_process.join();
    thread_synth.join();

    std::cout << "Main program terminated." << std::endl;
    return 0;
}