class ProcessData : public SEDThread {
    private:
        void process_detection(){
            std::string input = comm_queue.front();
            comm_queue.pop();

            std::cout << "Got input: " << input << std::endl;
        }

    public:
        void run(){
            while (running){
                std::unique_lock<std::mutex> lock(mtx);
                condition_var.wait(lock, [] { return !comm_queue.empty(); });
                process_detection();
            }
        }
};