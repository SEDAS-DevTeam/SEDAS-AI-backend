/*
    Sample testing file to test all AI models
*/
#include "./include/utils.h"

#include "./PlaneResponse/voice_recog.h"
#include "./PlaneResponse/process.h"
#include "./PlaneResponse/speech_synth.h"

// initialize threads
VoiceRecognition voice_recog;

static void signal_handler(int signal){
    if (signal == SIGINT){
        voice_recog.stop();
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

    std::signal(SIGINT, signal_handler);

    std::thread thread_main(&VoiceRecognition::run, &voice_recog);
    voice_recog.start();

    sleep(7);

    voice_recog.stop();

    thread_main.join();

    std::cout << voice_recog.result << std::flush;

    std::cout << "Main program terminated." << std::endl;
    return 0;
}