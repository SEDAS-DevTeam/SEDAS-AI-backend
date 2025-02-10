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

#include <sys/socket.h>
#include <netinet/in.h>

int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

void signalHandler(int signum) {
    std::cout << "\nReceived signal " << signum << ", exiting...\n";
    close(serverSocket);
    exit(0);
}

int main(int argc, char* argv[]){
    std::signal(SIGINT, signalHandler);

    // number of arguments will always be 5
    std::array<std::string, 5> output = process_args(argv);
    
    const std::string asr_path = output[0];
    const std::string tts_path = output[1];
    const std::string config_path = output[2];
    const std::string temp_out_path = output[3];

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(65432);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Bind failed, error: " << strerror(errno) << std::endl;
        return 1;
    }

    if (listen(serverSocket, 1) < 0) {
        std::cerr << "Listen failed, error: " << strerror(errno) << std::endl;
        return 1;
    }


    std::cout << "Waiting for connection..." << std::endl;
    
    int clientSocket = accept(serverSocket, nullptr, nullptr);

    std::cout << "Python connected" << std::endl;

    char buffer[1024] = {0};
    while (true){
        int bytesread = read(clientSocket, buffer, sizeof(buffer) - 1);
        if (bytesread > 0){
            buffer[bytesread] = '\0';
            std::cout << "Received" << buffer << std::endl;
        }
        else{
            break;
        }
    }

    std::cout << "Pipe closed, exiting program";
    close(clientSocket);
    close(serverSocket);
    return 0;
    
    /*
    keypress_detector.mainloop(recorder,
                      recognizer,
                      processor,
                      classifier,
                      synthesizer);    
    */
}