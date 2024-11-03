/*
    Sample testing file to test all AI models
*/
#include "./include/utils.h"

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

    std::cout << "Test?" << std::endl;

    std::signal(SIGINT, signal_handler);

    std::thread thread_recog(&VoiceRecognition::run, &voice_recog);
    std::thread thread_process(&ProcessData::run, &text_process);
    std::thread thread_synth(&SpeechSynthesis::run, &speech_synth);

    voice_recog.start();
    text_process.start();

    /*
        A simple loop to keep everything running
    */
    while (running){
        sleep(1);
    }

    voice_recog.stop();
    text_process.stop();
    speech_synth.stop();

    process_queue.terminate();
    synth_queue.terminate();

    thread_recog.join();
    thread_process.join();
    thread_synth.join();

    std::cout << "Main program terminated." << std::endl;
    return 0;
}