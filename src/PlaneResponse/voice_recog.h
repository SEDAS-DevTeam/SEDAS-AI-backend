class VoiceRecognition : public SEDThread {
    public:
        std::string result;

        void run(){
            std::array<char, 128> buffer;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("ping -c 5 google.com", "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr && running) {
                result += buffer.data();
                std::cout << buffer.data();
            }
        }
};