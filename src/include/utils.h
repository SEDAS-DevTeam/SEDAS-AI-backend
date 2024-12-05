#include <iostream>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <csignal>
#include <queue>
#include <filesystem>
#include <regex>
#include <condition_variable>
#include <mutex>
#include <map>
#include <tuple>
#include <cstdlib>
#include <algorithm>
#include <random>

#include <curl/curl.h>

// definitions/aliases
namespace fs = std::filesystem;
using json = nlohmann::json;
typedef std::vector<std::vector<std::string>> str_matrix;
bool running = true;
const std::string main_path = fs::current_path().u8string() + "/";

// definitions for ATC
std::map<std::string, std::string> nato_map = {
    { "alpha", "A" },
    { "beta", "B" },
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

std::map<std::string, std::string> num_map = {
    { "zero", "0" },
    { "one", "1" },
    { "two", "2" },
    { "three", "3" },
    { "four", "4" },
    { "five", "5" },
    { "six", "6" },
    { "seven", "7" },
    { "eight", "8" },
    { "niner", "9" }
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

std::string execute_command(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
        std::cout << buffer.data();
    }
    return result;
}

int rand_choice(uint32_t npos){
    srand(time(NULL));
    return rand() % npos;
}

json load_config(std::string config_name){
    std::string config_path = main_path + "PlaneResponse/config/" + config_name + ".json";
    std::ifstream config_file(config_path);
    
    return json::parse(config_file);
}

bool search_vector(std::vector<std::string> vec, std::string input){
    return std::find(vec.begin(), vec.end(), input) != vec.end();
}

bool search_string(std::string str, std::string substr){
    return str.find(substr) != std::string::npos;
}

std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

size_t download_callback(void* contents, size_t size, size_t nmemb, std::string* output){
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

void download_file_from_url(std::string& url, std::string& path){

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

// runtime definitions
SEDQueue process_queue;
SEDQueue synth_queue;