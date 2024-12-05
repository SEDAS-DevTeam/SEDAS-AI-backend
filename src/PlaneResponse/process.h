std::string trim_spaces(const std::string& input) {
    std::stringstream result;
    bool inWord = false; // Keeps track of whether we are inside a word

    for (char ch : input) {
        if (ch != ' ') {
            result << ch;    // Add non-space characters directly
            inWord = true;   // Mark that we are inside a word
        } else if (inWord) {
            result << ' ';   // Add a single space after a word
            inWord = false;  // Reset to outside word
        }
    }

    std::string output = result.str();

    // Remove trailing space, if any
    if (!output.empty() && output.back() == ' ') {
        output.pop_back();
    }

    return output;
}

class ProcessData : public SEDThread {
    private:
        std::string temp_res = ""; // temporary result for processing

        void process_detection(){
            std::string input = process_queue.get_element();
            // trim of useless spaces
            input = trim_spaces(input);

            // add to resulting string
            temp_res += " " + input;

            std::cout << "Processing: " << temp_res << std::endl;

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