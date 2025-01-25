/*
    Sample testing file to test all AI models
*/
#include "./lib/json/single_include/nlohmann/json.hpp"
#include "./include/utils.h"
#include "./include/record.h"
#include "./include/source_check.h"

#include "./PlaneResponse/voice_recog.h"
#include "./PlaneResponse/process.h"
#include "./PlaneResponse/speech_synth.h"

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

    /*
    AudioData data = initialize_data();

    PaStream* stream = start_stream(data);

    Pa_StartStream(stream);
    Pa_Sleep(5000);
    Pa_StopStream(stream);
    Pa_CloseStream(stream);

    Pa_Terminate();

    save_to_wav("test.wav", data);
    */
    std::cout << "Test!" << std::endl;

    /*
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);

    scrollok(stdscr, TRUE);
    while (true){
        if (detect_keypress()) {
            int ch = getch();

            // bind for killing program
            if (ch == 'q'){
                break;
            }

            if (ch == 'a'){
                if (recording){
                    printw("Stopped recording \n");
                    recording = false;
                }
                else{
                    printw("Started recording \n");
                    recording = true;
                }
            }

            refresh();
        }
    }
    */

    /*
    // loading configurations
    std::string recog_path = main_path / fs::path("PlaneResponse/config/config_recog.json");
    std::string process_path = main_path / fs::path("PlaneResponse/config/config_process.json");
    std::string synth_path = main_path / fs::path("PlaneResponse/config/config_synth.json");

    json config_recog = load_config(recog_path);
    json config_process = load_config(process_path);
    json config_synth = load_config(synth_path);
    

    // initialize models
    voice_recog.load_params(config_recog);
    
    // TODO: no usage for this feature (right now)
    // str_matrix install_list = check_models(config_synth);
    // refetch_missing(install_list);


    speech_synth.setup_model_registry();

    std::thread thread_recog(&VoiceRecognition::run, &voice_recog);
    std::thread thread_process(&ProcessData::run, &text_process);
    std::thread thread_synth(&SpeechSynthesis::run, &speech_synth);

    // TODO: just a sample how should the pseudopilot be initialized
    speech_synth.init_pseudopilot("OKL4545", 0.5f);

    voice_recog.start();
    text_process.start();
    speech_synth.start();

    //A simple loop to keep everything running
    while (running){
        sleep(1);
    }

    voice_recog.stop();
    text_process.stop();
    speech_synth.stop();

    speech_synth.cleanup();

    process_queue.terminate();
    synth_queue.terminate();

    thread_recog.join();
    thread_process.join();
    thread_synth.join();

    speech_synth.remove_pseudopilot("OKL4545");
    */

    std::cout << "Main program terminated." << std::endl;
    return 0;
}