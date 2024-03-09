import speech_recognition as sr
import whisper
import numpy as np
from pocketsphinx import LiveSpeech

import threading

class Base(object):
    #just a sample class that every other derives from
    def __init__(self, in_queue, out_queue):
        self.in_queue = in_queue #for incoming communication with core.py
        self.out_queue = out_queue #for out communication with core.py

    def log(self, message):
        self.out_queue.put(message)

    def process(self):
        while True:
            if not self.in_queue.empty():
                out = self.in_queue.get()
                if out == "interrupt":
                    self.log("interrupt")
                    break
            
            self.model_process()

class Whisper(Base):
    def __init__(self, queue):
        super(Whisper, self).__init__(queue)

        self.type = "test" #TODO
        self.model = whisper.load_model(type)
        self.Recognizer = sr.Recognizer()

    def process(self):
        #transcription queue
        ModelThread = threading.Thread(target=self.recognition, args=(self.data_queue, self.db_instance))

        with sr.Microphone() as source:
            while True:
                value = self.db_instance.get("start")

                #recognition check
                if value == "true": #start recognition
                    running = True
                    ModelThread.start()
                    self.db_instance.set("start-voice", "none") #prevents invoking random functions
                elif value == "false": #stop recognition
                    running = False
                    self.db_instance.set("start-voice", "none")
                    ModelThread.join()

                #the recognizer part
                try:
                    if running == True:
                        audio = self.Recognizer.listen(source, phrase_time_limit=4)
                        audio_data = audio.get_wav_data()

                        numpydata = np.frombuffer(audio_data, np.int16).copy()
                        numpydata = numpydata.flatten().astype(np.float32) / 32768.0

                        self.data_queue.put(numpydata)
                except KeyboardInterrupt:
                    ModelThread.join()

    def recognition(self, spec_queue, r_instance):
        while True:
            if not spec_queue.empty():
                numpydata = spec_queue.get()

                numpydata = whisper.pad_or_trim(numpydata)

                result = self.model.transcribe(numpydata, language="en", fp16=True, verbose=False)
                print("decoded text: " + result["text"])
                r_instance.set("out-voice", result["text"])

class CMUSphinx(Base):
    def __init__(self, queue):
        super(CMUSphinx, self).__init__(queue)

        self.running = False

    def process(self, debug = False):
        ModelThread = threading.Thread(target=self.recognize, args=(debug,))
        if not debug:
            while True:
                value = self.db_instance.get("start-voice")
                if value == "true" and not self.running:# and not self.running:
                    ModelThread.start()
                    self.running = True

                elif value == "false":
                    ModelThread.join()
                    self.running = False
        else: #debug == False
            ModelThread.start()

    def recognize(self, debug):
        for phrase in LiveSpeech():
            self.db_instance.set("out-voice", str(phrase))
        

class DeepSpeech(Base):
    def __init__(self, queue):
        super(DeepSpeech, self).__init__(queue)


class GoogleSpeechToText(Base):
    def __init__(self, in_queue, out_queue):
        super(GoogleSpeechToText, self).__init__(in_queue, out_queue)

        self.recognizer = sr.Recognizer()

    def model_process(self):
            with sr.Microphone() as source:
                print("Listening...")

                # Adjust for ambient noise
                self.recognizer.adjust_for_ambient_noise(source)

                # Listen to the user's input
                audio_data = self.recognizer.listen(source)

                try:
                    # Recognize the speech using Google Speech Recognition
                    text = self.recognizer.recognize_google(audio_data)
                    self.out_queue.set(text)
                    self.log("Text succesfully processed")
                except sr.UnknownValueError:
                    self.log("could not request results")
                except sr.RequestError as e:
                    self.log("could not request results")
                except Exception as e:
                    self.log("unknown error occured")

VOICE_MODEL_DICT = {
    "OpenAI Whisper": Whisper,
    "CMUSphinx": CMUSphinx,
    "DeepSpeech": DeepSpeech,
    "GoogleSTT": GoogleSpeechToText
}

if __name__ == "__main__":

    pass