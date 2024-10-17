/*
    Sample testing file to test all AI models
*/
#include "./include/utils.h"

#include "./PlaneResponse/voice_recog.h"
#include "./PlaneResponse/process.h"
#include "./PlaneResponse/speech_synth.h"

void simple_callback(){
    std::cout << "Invoking Callback!" << std::endl;
    sleep(1);
}

int main(){
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
    return 0;
}