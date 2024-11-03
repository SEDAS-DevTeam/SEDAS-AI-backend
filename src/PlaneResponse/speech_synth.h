class SpeechSynthesis : public SEDThread {
    private:
        void process_synthesis(){
            std::string input = synth_queue.get_element();
            std::cout << "Got input2: " << input << std::endl;
        }
    public:
        void run(){
            while (running){
                synth_queue.wait();
                process_synthesis();
            }
        }
};