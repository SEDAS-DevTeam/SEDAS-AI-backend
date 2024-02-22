import tkinter as tk
import threading
import time
import redis
import atexit

main = tk.Tk()
main.title("SEDAS DB-tester")
main.geometry("500x200")

#variables
out_text = tk.StringVar()
out_voice = tk.StringVar()
out_speech = tk.StringVar()
start_stop = tk.StringVar()
error_message_text = tk.StringVar()
running = False
terminate = False
CATCH_TIMEOUT = 1 #s

out_text.set("Not running")
out_voice.set("Not running")
out_speech.set("Not running")
start_stop.set("START")

#other declarations
class DB_catcher(threading.Thread):
    def __init__(self, database):
        threading.Thread.__init__(self)
        self.database = database

    def run(self):
        global running, terminate
        while True:
            if terminate:
                self.database.close()
                print("not terminated")
                break

            if running:
                time.sleep(CATCH_TIMEOUT)

                try:
                    voice = self.database.get("out-voice")
                    speech = self.database.get("out-speech")
                    text = self.database.get("out-text")

                    out_voice.set(voice)
                    out_speech.set(speech)
                    out_text.set(text)

                    #everything is fine
                    if error_message_text.get() == "Error connecting to database!":
                        error_message_text.set("")
                except redis.ConnectionError:
                    print("connection error!")
                    error_message_text.set("Error connecting to database!")

                    running = False
                    start_stop.set("START")

def start_stop_DB_catch():
    global running
    if running:
        running = False
        start_stop.set("START")
        out_text.set("Not running")
        out_voice.set("Not running")
        out_speech.set("Not running")
    else:
        running = True
        start_stop.set("STOP")

    print(running)

def reset_values():
    database.set("out-voice", "None")
    database.set("out-speech", "None")
    database.set("out-text", "None")



def get_values():
    pass

def proper_exit():
    global terminate
    terminate = True
    time.sleep(1)
    catcher_thread.join()

#connect to database
database = redis.Redis(host='localhost', decode_responses=True)

catcher_thread = DB_catcher(database)
catcher_thread.start()

label_text = tk.Label(main, text="TEXT MODEL OUTPUT", borderwidth=1, bg="red")
label_voice = tk.Label(main, text="VOICE MODEL OUTPUT", borderwidth=1, bg="red")
label_speech = tk.Label(main, text="SPEECH MODEL OUTPUT", borderwidth=1, bg="red")

label_text.grid(row=0, column=0)
label_voice.grid(row=0, column=1)
label_speech.grid(row=0, column=2)

frame_text = tk.Frame(main, highlightbackground="black", highlightthickness=2)
frame_voice = tk.Frame(main, highlightbackground="black", highlightthickness=2)
frame_speech = tk.Frame(main, highlightbackground="black", highlightthickness=2)

label_out_text = tk.Label(frame_text, textvariable=out_text)
label_out_voice = tk.Label(frame_voice, textvariable=out_voice)
label_out_speech = tk.Label(frame_speech, textvariable=out_speech)

label_out_text.pack()
label_out_voice.pack()
label_out_speech.pack()

frame_text.grid(row=1, column=0, sticky="nsew")
frame_voice.grid(row=1, column=1, sticky="nsew")
frame_speech.grid(row=1, column=2, sticky="nsew")

start_stop_button = tk.Button(main, textvariable=start_stop, width=15, command=start_stop_DB_catch)
start_stop_button.grid(row=2, column=0, pady=(10))
reset_button = tk.Button(main, text="RESET", width=15, command=reset_values)
reset_button.grid(row=2, column=1, pady=(10))

error_message = tk.Label(main, textvariable=error_message_text, fg="red")
error_message.grid(row=3, column=0, columnspan=2)

#main code
main.protocol("WM_DELETE_WINDOW", proper_exit)

main.grid_columnconfigure(0, weight=1)
main.grid_columnconfigure(1, weight=1)
main.grid_columnconfigure(2, weight=1)

main.mainloop()