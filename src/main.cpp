/*
    Integration file for SEDAS-manager
*/

#include "./include/utils.hpp"
#include "./PlaneResponse/voice_recog.hpp"
#include "./PlaneResponse/process.hpp"
#include "./PlaneResponse/classify.hpp"
#include "./PlaneResponse/speech_synth.hpp"

#include "./include/record.hpp"
#include "./include/source_check.hpp"
#include "./include/keypress.hpp"

int main(int argc, char* argv[]){

    std::cout << "Now running the integration file!" << std::endl;
    
    // number of arguments will always be 5
    std::array<std::string, 5> output = process_args(argv);
    
    const std::string asr_path = output[0];
    const std::string tts_path = output[1];
    const std::string config_path = output[2];
    const std::string temp_out_path = output[3];

    std::cout << asr_path + "/atc-whisper-ggml.bin" << std::endl;

    return 0;
    
    /*
    keypress_detector.mainloop(recorder,
                      recognizer,
                      processor,
                      classifier,
                      synthesizer);    
    */
}