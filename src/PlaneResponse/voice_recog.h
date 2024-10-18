class VoiceRecognition : public SEDThread {
    public:
        std::string result;
        std::queue<std::string> out;

        void run(){
            std::array<char, 128> buffer;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("ping google.com", "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr && running) {
                //result += buffer.data();
                std::cout << buffer.data() << std::flush;
            }
        }
};