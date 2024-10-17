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
        bool running = true;
        bool thread_stop = true;

        void start(){
            thread_stop = false;
        }

        void end(){
            running = false;
        }

        void stop(){
            thread_stop = true;
        }

        void run(){
            while(running){
                if (!thread_stop){
                    sleep(1);
                    std::cout << "I am running!" << std::endl;
                }
            }
        }
};

int main(){
    Base base;
    std::thread thread_main(&Base::run, &base);
    base.start();

    sleep(10); //Wait for 10 seconds

    base.end();
    thread_main.join();
    return 0;
}