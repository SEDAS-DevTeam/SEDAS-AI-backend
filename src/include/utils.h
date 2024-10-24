#include <iostream>
#include <thread>
#include <unistd.h>
#include <csignal>
#include <queue>
#include <filesystem>
#include <regex>
#include <condition_variable>
#include <mutex>

// thread communication variables
bool running = true;
std::condition_variable condition_var;
std::mutex mtx;
std::queue<std::string> comm_queue;

namespace fs = std::filesystem;

const std::string main_path = fs::current_path().u8string() + "/";

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