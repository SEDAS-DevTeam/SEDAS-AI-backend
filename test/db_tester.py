import tkinter as tk
import threading
import time
import redis

main = tk.Tk()
main.title("SEDAC DB-tester")
main.geometry("500x500")

#variables
out_text = tk.StringVar()
out_voice = tk.StringVar()
out_speech = tk.StringVar()
start_stop = tk.StringVar()
error_message_text = tk.StringVar()
running = False
CATCH_TIMEOUT = 1 #s

out_text.set("Not running")
out_voice.set("Not running")
out_speech.set("Not running")
start_stop.set("START")

#other declarations
class DB_catcher(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.database = redis.Redis(host='localhost', decode_responses=True)

    def run(self):
        global running
        while running:
            time.sleep(CATCH_TIMEOUT)

            try:
                voice = self.database.get("out-voice")
                speech = self.database.get("out-speech")
                text = self.database.get("out-speech")
            except redis.ConnectionError:
                print("connection error!")
                error_message_text.set("Error connecting to database!")

                running = False
                start_stop.set("START")
                self.join()

            out_voice.set(voice)
            out_speech.set(speech)
            out_text.set(text)
            

def start_stop_DB_catch():
    global running
    print(running)
    if running:
        running = False
        start_stop.set("START")
        catcher_thread.join()
    else:
        running = True
        start_stop.set("STOP")
        catcher_thread.start()

def reset_values():
    out_text.set("None")
    out_voice.set("None")
    out_speech.set("None")

def get_values():
    pass

catcher_thread = DB_catcher()

label_text = tk.Label(main, text="TEXT MODEL OUTPUT", borderwidth=1,bg="red")
label_voice = tk.Label(main, text="VOICE MODEL OUTPUT", borderwidth=1,bg="red")
label_speech = tk.Label(main, text="SPEECH MODEL OUTPUT", borderwidth=1,bg="red")

label_text.grid(row=0, column=0)
label_voice.grid(row=0, column=1)
label_speech.grid(row=0, column=2)

label_out_text = tk.Label(main, textvariable=out_text)
label_out_voice = tk.Label(main, textvariable=out_voice)
label_out_speech = tk.Label(main, textvariable=out_speech)

label_out_text.grid(row=1, column=0)
label_out_voice.grid(row=1, column=1)
label_out_speech.grid(row=1, column=2)

start_stop_button = tk.Button(main, textvariable=start_stop, width=15, command=start_stop_DB_catch)
start_stop_button.grid(row=2, column=0, pady=(10))
reset_button = tk.Button(main, text="RESET", width=15, command=reset_values)
reset_button.grid(row=2, column=1, pady=(10))

error_message = tk.Label(main, textvariable=error_message_text, fg="red")
error_message.grid(row=3, column=0, columnspan=2)

#main code

main.grid_columnconfigure(0, weight=1)
main.grid_columnconfigure(1, weight=1)
main.grid_columnconfigure(2, weight=1)

main.mainloop()