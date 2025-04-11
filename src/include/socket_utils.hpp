#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <any>

#include "../include/utils.hpp"
#include "../include/record.hpp"
#include "../PlaneResponse/voice_recog.hpp"
#include "../PlaneResponse/speech_synth.hpp"
#include "../PlaneResponse/process.hpp"
#include "../PlaneResponse/classify.hpp"

inline bool recording = false;

inline std::vector<std::string> separate_by_spaces(std::string input){
    std::string substr;
    std::vector<std::string> out;
    std::stringstream ss(input);
    while (getline(ss, substr, ' ')){
        out.push_back(substr);
    }

    return out;
}

inline bool string_contains(std::string str, std::string substr){
    if (str.find(substr) != std::string::npos) return true;
    else return false;
}

inline int initialize_server(){
    return socket(AF_INET, SOCK_STREAM, 0);
}

inline sockaddr_in set_address(int port){
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    return serverAddress;
}

inline bool bind_socket(sockaddr_in server_address, int server_socket){
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Bind failed, error: " << strerror(errno) << std::endl;
        return false;
    }
    else return true;
}

inline bool socket_listen(int server_socket){
    if (listen(server_socket, 1) < 0) {
        std::cerr << "Listen failed, error: " << strerror(errno) << std::endl;
        return false;
    }
    else return true;
}

inline bool enable_socket_reuse(int server_socket){
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        std::cerr << "setsockopt(SO_REUSEADDR) failed, error: " << strerror(errno) << std::endl;
        return false;
    }
    else return true;
}

inline int accept_socket(int server_socket){
    return accept(server_socket, nullptr, nullptr);
}

inline void mainloop(Recorder &recorder,
              Recognizer &recognizer,
              Processor &processor,
              Classifier &classifier,
              Synthesizer &synthesizer,
              Logger &logger,
              int client_socket){
    char buffer[1024] = {0};
    while (true){
        int bytesread = read(client_socket, buffer, sizeof(buffer) - 1);
        if (bytesread > 0){
            buffer[bytesread] = '\0';
            std::string message(buffer);
            std::cout << "Received " << message << std::endl;

            if (message == "start-mic"){
                recording = true;
                recorder.start();
            }
            else if (message == "stop-mic"){
                logger.log("PTT event");

                recording = false;
                recorder.stop();

                Pa_Sleep(100);

                std::string transcription = recognizer.run(logger); // infer the recording output

                auto [processor_out, values] = processor.run(transcription, logger);
                std::string callsign = processor_out[0];
                std::string classifier_input = processor_out[1];

                std::vector<std::string> commands = classifier.run(classifier_input);

                // input for synthesizer
                std::vector<std::string> synth_commands(commands);
                std::vector<std::string> synth_values(values);

                // just for logging
                std::map<std::string, std::any> out_dict;
                out_dict["callsign"] = callsign;
                out_dict["values"] = values;
                out_dict["commands"] = commands;

                // TODO: just works for one command at the time

                // exception for levels - converting FL (TODO: only treating TL as 1000ft) REWORK LATER
                if (search_string(commands[0], "descend-fl") || search_string(commands[0], "climb-fl")){values[0] = values[0] + "00";}
                if (search_string(commands[0], "descend") || search_string(commands[0], "climb")){commands[0] = "level-any";}

                std::string comm_main = callsign + " " + values[0] + " " + commands[0];

                // send to client socket
                send(client_socket, comm_main.c_str(), comm_main.size(), 0);

                // log to file
                log_values(out_dict, logger);

                // respond to command
                synthesizer.run(synth_commands[0],
                                synth_values[0],
                                callsign,
                                logger); // just respond to one command [TODO]

                logger.pad();
            }
            else if (string_contains(message, "register") && !string_contains(message, "unregister")){
                std::vector<std::string> args = separate_by_spaces(message);
                args.erase(args.begin());

                synthesizer.init_pseudopilot(args[0], std::stof(args[1]));
                std::cout << "Added pseudopilot to registry" << std::endl;
            }
            else if (string_contains(message, "unregister")){
                std::vector<std::string> args = separate_by_spaces(message);
                args.erase(args.begin());
                
                synthesizer.remove_pseudopilot(args[0]);
                std::cout << "Removed pseudopilot from registry" << std::endl;
            }
            else if (string_contains(message, "unregister-all")){
                synthesizer.remove_all();
                std::cout << "Removed all pseudopilot from registry" << std::endl;
            }

            
            if (message == "quit"){
                std::cout << "killing thread" << std::endl;
                break;
            }
        }
    }
}