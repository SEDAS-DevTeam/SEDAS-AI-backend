#include <ncurses.h>

bool recording = false;

class Detect_ncurses{
    private:
        int detect_keypress(){
            int ch = getch();

            if (ch != ERR) {
                ungetch(ch);
                return 1;
            } else {
                return 0;
            }
        }

    public:
        void setup(){
            initscr();
            cbreak();
            noecho();
            nodelay(stdscr, TRUE);
            scrollok(stdscr, TRUE);
        }

        void mainloop(Recorder &recorder, 
                               Recognizer &recognizer,
                               Processor &processor,
                               Classifier &classifier,
                               Synthesizer &synthesizer,
                               Logger &logger){
            while (true){
                if (detect_keypress()) {
                    int ch = getch();

                    // bind for killing the program (TODO: just for testing)
                    if (ch == 'q'){
                        refresh(); // flush before terminate
                        endwin();
                        recorder.terminate();
                        break;
                    }

                    if (ch == 'a'){
                        if (recording){
                            printw("Stopped recording \n");
                            logger.log("Stopped recording");

                            recording = false;
                            recorder.stop();

                            Pa_Sleep(100);

                            std::string transcription = recognizer.run(logger); // infer the recording output

                            auto [processor_out, values] = processor.run(transcription, logger);
                            std::string callsign = processor_out[0];
                            std::string classifier_input = processor_out[1];

                            std::vector<std::string> commands = classifier.run(classifier_input);
                            synthesizer.run(commands[0],
                                            values[0],
                                            callsign,
                                            logger); // just respond to one command [TODO]

                            std::map<std::string, std::any> out_dict;
                            out_dict["callsign"] = callsign;
                            out_dict["values"] = values;
                            out_dict["commands"] = commands;

                            log_values(out_dict, logger);
                        }
                        else{
                            printw("Started recording \n");
                            logger.log("Started recording");

                            recording = true;
                            recorder.start();
                        }
                    }

                    refresh();
                }
            }
        }
};