#include <iostream>
#include <thread>
#include <unistd.h>

class SEDThread {
    private:
        bool running = true;
        bool thread_stop = true;
        void (*sed_callback)(void);

    public:
        SEDThread(void (*callback)(void)){
            sed_callback = callback;
        }

        void run(){
            while(running){
                if (!thread_stop){
                    sed_callback();
                }
            }
        }

        void start(){ thread_stop = false; }
        void stop(){ running = false; }
        void pause(){ thread_stop = true; }
};