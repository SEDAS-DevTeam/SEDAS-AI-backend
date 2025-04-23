#pragma once

// general
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <csignal>
#include <map>
#include <algorithm>
#include <array>

// multithread rework
#include <queue>
#include <condition_variable>
#include <mutex>

#include <curl/curl.h>
#include <unistd.h> // for sleep
#include <nlohmann/json.hpp>

// definitions/aliases
typedef std::vector<std::vector<std::string>> str_matrix;
typedef std::map<std::string, std::string> str_map;

inline bool running = true;
using json = nlohmann::json;

// definitions for ATC
inline str_map nato_map = {
    { "alpha", "A" },
    { "bravo", "B" },
    { "charlie", "C" },
    { "delta", "D" },
    { "echo", "E" },
    { "foxtrot", "F" },
    { "golf", "G" },
    { "hotel", "H" },
    { "india", "I" },
    { "juliet", "J" },
    { "kilo", "K" },
    { "lima", "L" },
    { "mike", "M" },
    { "november", "N" },
    { "oscar", "O" },
    { "papa", "P" },
    { "quebec", "Q" },
    { "romeo", "R" },
    { "sierra", "S" },
    { "tango", "T" },
    { "uniform", "U" },
    { "victor", "V" },
    { "whiskey", "W" },
    { "x-ray", "X" },
    { "yankee", "Y" },
    { "zulu", "Z" }
};

inline str_map num_map = {
    { "zero", "0" },
    { "one", "1" },
    { "two", "2" },
    { "three", "3" },
    { "four", "4" },
    { "five", "5" },
    { "six", "6" },
    { "seven", "7" },
    { "eight", "8" },
    { "niner", "9" },
    { "nine", "9"} // for mild correction in transcription
};

// used for conversion back to speech synth
inline str_map num_map2 = {
    { "0", "zero" },
    { "1", "one" },
    { "2", "two" },
    { "3", "three" },
    { "4", "four" },
    { "5", "five" },
    { "6", "six" },
    { "7", "seven" },
    { "8", "eight" },
    { "9", "niner" },
};

class SEDQueue {
    public:
        std::condition_variable queue_cond_var;
        std::mutex queue_mutex;
        std::queue<std::string> queue;

        void add_element(std::string elem){
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue.push(elem);
        }

        std::string get_element(){
            if (!queue.empty()){
                std::string input = queue.front();
                queue.pop();

                return input;
            }
            else{
                return "empty";
            }
        }

        void notify(){
            queue_cond_var.notify_one();
        }

        void wait(){
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cond_var.wait(lock);
        }

        void terminate(){
            std::queue<std::string> empty;
            std::swap(queue, empty);

            queue_cond_var.notify_all();
        }

        bool is_empty(){
            return queue.size() == 0;
        }
};

class SEDThread {
    public:

        bool running = true;
        bool thread_stop = true;

        void start(){ thread_stop = false; }
        void stop(){ running = false; }
        void pause(){ thread_stop = true; }
};

struct command_result{
    std::string output;
    int exit_status;
};

inline command_result execute_command(const char* cmd, bool verbose = true) {
    std::array<char, 128> buffer;
    std::string result;
    std::string full_cmd = std::string(cmd) + " 2>&1"; // redirect stderr to stdout
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(full_cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
        if (verbose){
            std::cout << buffer.data();
        }
    }

    int exit_code = pclose(pipe.release());
    if (WIFEXITED(exit_code)) {
        exit_code = WEXITSTATUS(exit_code);
    } else {
        exit_code = -1; // Abnormal termination
    }

    return {result, exit_code};
}

inline std::array<std::string, 5> process_args(char* argv[]){
    const size_t argc = 5;
    std::array<std::string, argc> output;

    for (int i = 1; i < argc; i++) output[i - 1] = argv[i];
    return output;
}

inline int rand_choice(uint32_t npos){
    srand(time(NULL));
    return rand() % npos;
}

inline bool search_vector(std::vector<std::string> vec, std::string input){
    return std::find(vec.begin(), vec.end(), input) != vec.end();
}

inline bool search_string(std::string str, std::string substr){
    return str.find(substr) != std::string::npos;
}

inline std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

inline size_t download_callback(void* contents, size_t size, size_t nmemb, std::string* output){
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

inline std::string to_lower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

inline void download_file_from_url(std::string& url, std::string& path){

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return;
    }

    std::string content;
    
    try{
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, download_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Enable automatic redirects
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L); // Limit max number of redirects

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return;
        }

        std::ofstream output_file(path);

        if (!output_file) {
            std::cerr << "Unable to open file: " << path << std::endl;
            curl_easy_cleanup(curl);
            return;
        }
        output_file << content;
        output_file.close();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception occurred: " << e.what() << std::endl;
        curl_easy_cleanup(curl);
        return;
    }

    // Clean up
    curl_easy_cleanup(curl);
}

inline json load_config(std::string config_path){
    std::ifstream config_file(config_path);
    return json::parse(config_file);
}

inline void write_config(std::string config_path, json data){
    std::ofstream config_file(config_path);
    if (config_file.is_open()){
        config_file << data.dump(4);
        config_file.close();
    }
}

class Logger {
    private:
        FILE *logfile;
    public:
        std::string LOGFILE_PATH;

        Logger(std::string temp_out_path){
            LOGFILE_PATH = temp_out_path + "/log.txt";
            logfile = fopen(LOGFILE_PATH.c_str(), "w");
        }
        void log(std::string content){
            if (logfile){
                time_t timestamp = time(NULL);
                struct tm datetime = *localtime(&timestamp);
                
                size_t time_len = 15;
                char time_out[time_len];
                strftime(time_out, time_len, "[%H:%M:%S] ", &datetime);

                fprintf(logfile, "%s\n", (std::string(time_out) + content).c_str());
                fflush(logfile);
            }
        }

        void pad(){
            if (logfile){
                fprintf(logfile, "\n");
                fflush(logfile);
            }
        }

        void end(){
            if (logfile) fclose(logfile);
        }
};