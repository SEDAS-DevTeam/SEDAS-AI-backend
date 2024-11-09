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
            std::string input = process_queue.get_element();

            std::cout << "Got input: " << input << std::endl;

            //TODO: add processing for simple command
            synth_queue.add_element(input);
            synth_queue.notify();
        }

    public:
        void run(){
            while (running){
                process_queue.wait();
                if (running){ // for last resort notification (when queue is getting emptied)
                    process_detection();
                }
            }
        }
};