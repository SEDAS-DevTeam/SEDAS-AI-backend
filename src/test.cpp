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

static void signal_handler(int signal){
    if (signal == SIGINT){
        voice_recog.stop();
        running = false;
    }
}

int main(){
    /*
    SEDThread base(simple_callback);
    std::thread thread_main(&SEDThread::run, &base);
    base.start();
    sleep(10); //Wait for 10 seconds

    base.pause();
    sleep(2);

    base.start();
    sleep(5);

    base.stop();
    thread_main.join();
    */

    std::cout << "Test?" << std::endl;

    std::signal(SIGINT, signal_handler);

    std::thread thread_recog(&VoiceRecognition::run, &voice_recog);
    std::thread thread_process(&ProcessData::run, &text_process);
    //std::thread thread_synth();

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

    thread_recog.join();
    thread_process.join();

    std::cout << "Main program terminated." << std::endl;
    return 0;
}