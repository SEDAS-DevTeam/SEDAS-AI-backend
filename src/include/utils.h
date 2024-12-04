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

namespace fs = std::filesystem;
using json = nlohmann::json;
bool running = true;
const std::string main_path = fs::current_path().u8string() + "/";

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
        json config_data = json::object();

        bool running = true;
        bool thread_stop = true;

        void start(){ thread_stop = false; }
        void stop(){ running = false; }
        void pause(){ thread_stop = true; }

        void load_config(std::string config_name){
            std::string config_path = main_path + "PlaneResponse/config/" + config_name + ".json";
            std::ifstream config_file(config_path);
            
            config_data = json::parse(config_file);
        }
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

// runtime definitions
SEDQueue process_queue;
SEDQueue synth_queue;