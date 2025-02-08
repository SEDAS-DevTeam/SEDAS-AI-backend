#ifdef TESTING
#include <ncurses.h>
#endif

// for X11
#include <X11/Xlib.h>
#include "X11/keysym.h"

// for Wayland

// for Windows

// for mac

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
                            //synthesizer.run(plane_command, logger);

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

class Detect_generic{
    private:
        bool detect_keypress(KeySym ks){
            Display *dpy = XOpenDisplay(":0");
            char keys_return[32];
            XQueryKeymap(dpy, keys_return);
            KeyCode kc2 = XKeysymToKeycode(dpy, ks);
            bool isPressed = !!(keys_return[kc2 >> 3] & (1 << (kc2 & 7)));
            XCloseDisplay(dpy);
            return isPressed;
        }

    public:
        void setup(){

        }

        void mainloop(){
            while (true){
                if (detect_keypress(XK_q)){
                    std::cout << "q pressed!" << std::endl;
                    break;
                }
            }
        }
};