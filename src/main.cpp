/*
    Integration file for SEDAS-manager
*/

#include <filesystem>

#include "./PlaneResponse/voice_recog.hpp"
#include "./PlaneResponse/process.hpp"
#include "./PlaneResponse/classify.hpp"
#include "./PlaneResponse/speech_synth.hpp"

#include "./include/record.hpp"
#include "./include/socket_utils.hpp"
#include "./include/utils.hpp"


namespace fs = std::filesystem;

int main(int argc, char* argv[]){

    // number of arguments will always be 5
    std::array<std::string, 5> output = process_args(argv);
    
    const std::string asr_path = output[0];
    const std::string tts_path = output[1];
    const std::string config_path = output[2];
    const std::string temp_out_path = output[3];

    /*
    Localhost network settings
    */

    int server_socket = initialize_server();
    sockaddr_in server_address = set_address(65432);

    if (!enable_socket_reuse(server_socket)) return 1;
    if (!bind_socket(server_address, server_socket)) return 1;
    if (!socket_listen(server_socket)) return 1;
    std::cout << "ready" << std::endl;
    
    int client_socket = accept_socket(server_socket);
    std::cout << "connected" << std::endl;

    /*
    Model configurations
    */

    Logger logger(temp_out_path);

    // load configurations
    std::string classify_path = fs::path(config_path + "/config_classify.json");
    std::string synth_path = fs::path(config_path + "/config_synth.json");
    std::string response_path = fs::path(config_path + "/config_response.json");

    // for saving currently utilised TTS models
    std::string session_memory_path = fs::path(temp_out_path + "/session.json");

    json config_classify = load_config(classify_path);
    json config_synth = load_config(synth_path);
    json config_response = load_config(response_path);

    Recorder recorder(temp_out_path); // setup recording handler through keypress
    recorder.initialize();

    Recognizer recognizer(asr_path, temp_out_path); // setup main ASR
    
    Processor processor; // setup processor
    Classifier classifier; // setup classifier
    classifier.set_rules(config_classify);

    Synthesizer synthesizer(tts_path, temp_out_path, session_memory_path); // setup speech synthesizer
    synthesizer.setup_model_registry();
    synthesizer.setup_responses(config_response);

    mainloop(recorder,
             recognizer,
             processor,
             classifier,
             synthesizer,
             logger,
             client_socket);

    std::cout << "Pipe closed, exiting program" << std::endl;
    synthesizer.remove_all();
    close(client_socket);
    close(server_socket);
    return 0;
}