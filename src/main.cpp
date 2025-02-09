/*
    Integration file for SEDAS-manager
*/

#include "./include/utils.hpp"
#include "./PlaneResponse/voice_recog.hpp"
#include "./PlaneResponse/process.hpp"
#include "./PlaneResponse/classify.hpp"
#include "./PlaneResponse/speech_synth.hpp"

#include "./include/record.hpp"
#include "./include/keypress.hpp"

bool keepRunning = true;

void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", exiting...\n";
    keepRunning = false;
}

int main(int argc, char* argv[]){
    std::signal(SIGINT, signalHandler);

    // number of arguments will always be 5
    std::array<std::string, 5> output = process_args(argv);
    
    const std::string asr_path = output[0];
    const std::string tts_path = output[1];
    const std::string config_path = output[2];
    const std::string temp_out_path = output[3];

    std::string fifo_name = "comm";
    std::ifstream fifo(temp_out_path + fifo_name);

    std::string message;
    while (keepRunning && std::getline(fifo, message)) {  // Read continuously
        std::cout << "C++ received: " << message << std::endl;
    }

    std::cout << "Pipe closed, exiting program";
    return 0;
    
    /*
    keypress_detector.mainloop(recorder,
                      recognizer,
                      processor,
                      classifier,
                      synthesizer);    
    */
}