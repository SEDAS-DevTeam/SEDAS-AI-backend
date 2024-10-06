/*
    Sample testing file to test all AI models
*/

#include <iostream>
#include <thread>
#include <unistd.h>

#include "./PlaneResponse/voice_recog.h"
#include "./PlaneResponse/process.h"
#include "./PlaneResponse/speech_synth.h"

class Base {
    public: 
        void run(){
            sleep(2);
            std::cout << "I am running!" << std::endl;
        }
};

int main(){
    Base base;
    std::thread thread_main(&Base::run, &base);

    sleep(10); //Wait for 10 seconds

    thread_main.join();
    return 0;
}