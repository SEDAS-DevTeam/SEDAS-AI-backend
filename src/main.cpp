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
#include "./include/socket_utils.hpp"

int main(int argc, char* argv[]){

    // number of arguments will always be 5
    std::array<std::string, 5> output = process_args(argv);
    
    const std::string asr_path = output[0];
    const std::string tts_path = output[1];
    const std::string config_path = output[2];
    const std::string temp_out_path = output[3];

    int server_socket = initialize_server();
    sockaddr_in server_address = set_address(65432);

    if (!enable_socket_reuse(server_socket)) return 1;
    if (!bind_socket(server_address, server_socket)) return 1;
    if (!socket_listen(server_socket)) return 1;
    std::cout << "Waiting for connection..." << std::endl;
    
    int client_socket = accept_socket(server_socket);
    std::cout << "Python connected" << std::endl;

    char buffer[1024] = {0};
    while (true){
        int bytesread = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytesread > 0){
            buffer[bytesread] = '\0';
            std::string message(buffer);
            std::cout << "Received " << message << std::endl;

            if (message == "test"){
                std::cout << "Invoked test!" << std::endl;
            }
            else if (message == "quit"){
                std::cout << "killing thread" << std::endl;
                break;
            }
        }
    }

    std::cout << "Pipe closed, exiting program";
    close(client_socket);
    close(server_socket);
    return 0;
    
    /*
    keypress_detector.mainloop(recorder,
                      recognizer,
                      processor,
                      classifier,
                      synthesizer);    
    */
}