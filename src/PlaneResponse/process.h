std::map<char, char> my_map = {
    { 'alpha', 'A' },
    { 'beta', 'B' },
    { 'charlie', 'C' },
    { 'delta', 'D' },
    { 'echo', 'E' },
    { 'foxtrot', 'F' },
    { 'golf', 'G' },
    { 'hotel', 'H' },
    { 'india', 'I' },
    { 'juliet', 'J' },
    { 'kilo', 'K' },
    { 'lima', 'L' },
    { 'mike', 'M' },
    { 'november', 'N' },
    { 'oscar', 'O' },
    { 'papa', 'P' },
    { 'quebec', 'Q' },
    { 'romeo', 'R' },
    { 'sierra', 'S' },
    { 'tango', 'T' },
    { 'uniform', 'U' },
    { 'victor', 'V' },
    { 'whiskey', 'W' },
    { 'x-ray', 'X' },
    { 'yankee', 'Y' },
    { 'zulu', 'Z' }
};

class ProcessData : public SEDThread {
    private:
        void process_detection(){
            std::string input = comm_queue.front();
            comm_queue.pop();

            std::cout << "Got input: " << input << std::endl;

            //TODO: add processing for simple command
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