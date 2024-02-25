# Import Module 
import tkinter as tk
import time 
import multiprocessing
import redis
import sys
import os

#append all cache files to PATH
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "src"))


#emulated models
from PlaneResponse import speech_models, text_models, voice_models
  
main = tk.Tk()
main.title("SEDAS DB-tester")
main.geometry("500x300")

out_text = tk.StringVar()
out_voice = tk.StringVar()
out_speech = tk.StringVar()
start_stop = tk.StringVar()
error_message_text = tk.StringVar()
running = False
terminate = False
CATCH_TIMEOUT = 1 #s

thread = None
thread_backend = None
thread_voice = None
thread_speech = None
thread_text = None

core_terminate = False

out_text.set("Not running")
out_voice.set("Not running")
out_speech.set("Not running")
start_stop.set("START")

#connect to database
database = redis.Redis(host='localhost', decode_responses=True)
  
def thread_run(): 
    global running, thread, terminate, core_terminate

    if running:
        running = False
        start_stop.set("START")
        out_text.set("Not running")
        out_voice.set("Not running")
        out_speech.set("Not running")

        terminate = True

        start_stop.set("START")
        out_text.set("Not running")
        out_voice.set("Not running")
        out_speech.set("Not running")

        reset_values()

        database.set("terminate", "true") #terminate all model threads
        core_terminate = True #terminate backend

        thread.terminate()
        thread_backend.terminate()
        thread_voice.terminate()
        thread_text.terminate()
        thread_speech.terminate()
    else:
        running = True
        terminate = False
        start_stop.set("STOP")

        #start core emulation
        emulate_core()

        thread=multiprocessing.Process(target=db_catcher) 
        thread.start()

    print("************")
    print("Is main thread alive? ", thread.is_alive())
    print("Is backend thread alive? ", thread_backend.is_alive())
    print("Is voice thread alive? ", thread_voice.is_alive())
    print("Is text thread alive? ", thread_text.is_alive())
    print("Is speech thread alive? ", thread_speech.is_alive())
    print("************")
  
def reset_values():
    database.set("out-voice", "None")
    database.set("out-speech", "None")
    database.set("out-text", "None")

# work function 
def db_catcher(): 
    global running
  
    while True:
        time.sleep(CATCH_TIMEOUT)     
        print("catching")   
        if terminate:
            break

        try:
            voice = database.get("out-voice")
            speech = database.get("out-speech")
            text = database.get("out-text")

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
            break

def emulate_backend():
    last_value_voice = ""
    last_value_command = ""

    while True:
        time.sleep(500)

        if core_terminate:
            print("termination")
            break

        value_voice = database.get("out-voice")
        value_command = database.get("proc-voice-out")

        if value_voice == last_value_voice:
            database.set("proc-voice", value_voice)
            last_value_voice = value_voice

        if value_command == last_value_command:
            print("command change, processing...")
            #database.set("gen-speech", value_command)
            print(value_command)

#emulating core.py and backend.ts solely for debugging purposes
def emulate_core():
    global thread_backend, thread_voice, thread_speech, thread_text

    voice_model = "GoogleSTT"
    text_model = "simplePOS"
    speech_model = "GoogleTTS"

    #model selection
    m_voice_instance = voice_models.VOICE_MODEL_DICT[voice_model](database)
    m_text_instance = text_models.TEXT_MODEL_DICT[text_model](database)
    m_speech_instance = speech_models.SPEECH_MODEL_DICT[speech_model](database)

    #set all channels to default
    database.set("start", "false")
    database.set("terminate", "false") #used by core.py when terminating all threads
    database.set("gen-speech", "")
    database.set("proc-voice", "")
    database.set("out-voice", "")
    database.set("in-terrain", "")
    database.set("out-terrain", "")
    database.set("proc-voice-out", "")

    #starting backend
    thread_backend = multiprocessing.Process(target=emulate_backend)
    thread_backend.start()

    thread_voice = multiprocessing.Process(target=m_voice_instance.process)
    thread_text = multiprocessing.Process(target=m_text_instance.process)
    thread_speech = multiprocessing.Process(target=m_speech_instance.process)

    thread_voice.start()
    thread_text.start()
    thread_speech.start()

def send_command():
    value = query.get()
    database.set("out-voice", value)
  
#connect to database
database = redis.Redis(host='localhost', decode_responses=True)

#elements
label_query = tk.Label(main, text="Input a query:")
label_query.grid(row=0, column=0, pady=10, padx=10)

query = tk.Entry(main)
query.grid(row=0, column=1)

query_button = tk.Button(main, text="Send", command=send_command)
query_button.grid(row=0, column=2)

start_stop_button = tk.Button(main, textvariable=start_stop, width=15, command=thread_run)
start_stop_button.grid(row=1, column=0, pady=(10))
reset_button = tk.Button(main, text="RESET", width=15, command=reset_values)
reset_button.grid(row=1, column=1, pady=(10))

error_message = tk.Label(main, textvariable=error_message_text, fg="red")
error_message.grid(row=2, column=0, columnspan=2, pady=(20))

label_text = tk.Label(main, text="TEXT MODEL OUTPUT", borderwidth=1, bg="red")
label_voice = tk.Label(main, text="VOICE MODEL OUTPUT", borderwidth=1, bg="red")
label_speech = tk.Label(main, text="SPEECH MODEL OUTPUT", borderwidth=1, bg="red")

label_text.grid(row=3, column=0)
label_voice.grid(row=3, column=1)
label_speech.grid(row=3, column=2)

frame_text = tk.Frame(main, highlightbackground="black", highlightthickness=2)
frame_voice = tk.Frame(main, highlightbackground="black", highlightthickness=2)
frame_speech = tk.Frame(main, highlightbackground="black", highlightthickness=2)

label_out_text = tk.Label(frame_text, textvariable=out_text)
label_out_voice = tk.Label(frame_voice, textvariable=out_voice)
label_out_speech = tk.Label(frame_speech, textvariable=out_speech)

label_out_text.pack()
label_out_voice.pack()
label_out_speech.pack()

frame_text.grid(row=4, column=0, sticky="nsew")
frame_voice.grid(row=4, column=1, sticky="nsew")
frame_speech.grid(row=4, column=2, sticky="nsew")

main.grid_columnconfigure(0, weight=1)
main.grid_columnconfigure(1, weight=1)
main.grid_columnconfigure(2, weight=1)

main.mainloop()