import speech_recognition as sr
import whisper
import numpy as np
from pocketsphinx import LiveSpeech

import threading
import queue

class Base(object):
    #just a sample class that every other derives from
    def __init__(self, in_queue, out_queue, debug_queue):
        self.in_queue = in_queue #for incoming communication with core.py
        self.out_queue = out_queue #for out communication with core.py
        self.debug_queue = debug_queue
        self.running = True

    def log(self, message):
        self.debug_queue.append(f"VOICE-MODEL {message}")

    def process(self):
        while True:
            if len(self.in_queue) != 0:
                out = self.in_queue.get()
                if out == "interrupt":
                    self.log("interrupt")
                    break
            
            self.model_process()

class Whisper(Base):
    def __init__(self, in_queue, out_queue, debug_queue):
        super(Whisper, self).__init__(self, in_queue, out_queue, debug_queue)

        #tiny
        #base
        #small
        #medium
        self.type = "base" #TODO
        self.model = whisper.load_model(type)
        self.Recognizer = sr.Recognizer()
        self.data_queue = queue.Queue()

        #transcription queue
        self.model_thread = threading.Thread(target=self.recognition, args=(self.data_queue, ))
        self.model_thread.start()

    def model_process(self):
        with sr.Microphone() as source:
            while True:

                #the recognizer part
                try:
                    if self.running:
                        audio = self.Recognizer.listen(source, phrase_time_limit=4)
                        audio_data = audio.get_wav_data()

                        numpydata = np.frombuffer(audio_data, np.int16).copy()
                        numpydata = numpydata.flatten().astype(np.float32) / 32768.0

                        self.data_queue.append(numpydata)
                except KeyboardInterrupt:
                    self.model_thread.join()

    def recognition(self, spec_queue):
        while True:
            if not spec_queue.empty():
                numpydata = spec_queue.get()

                numpydata = whisper.pad_or_trim(numpydata)

                result = self.model.transcribe(numpydata, language="en", fp16=True, verbose=False)
                print("decoded text: " + result["text"])
                self.out_queue.append(result["text"])

class CMUSphinx(Base):
    def __init__(self, in_queue, out_queue, debug_queue):
        super(CMUSphinx, self).__init__(in_queue, out_queue, debug_queue)
        self.model_thread = threading.Thread(target=self.recognize)

    def model_process(self):
        for phrase in LiveSpeech():
            self.out_queue.append(str(phrase))
        

class DeepSpeech(Base):
    def __init__(self, queue):
        super(DeepSpeech, self).__init__(queue)


class GoogleSpeechToText(Base):
    def __init__(self, in_queue, out_queue, debug_queue):
        super(GoogleSpeechToText, self).__init__(in_queue, out_queue, debug_queue)

    def model_process(self):
        self.recognizer = sr.Recognizer()

        # Check for available microphones
        if not sr.Microphone.list_microphone_names():
            self.log("No microphone found. Speech recognition unavailable.")
            return

        with sr.Microphone() as source:

            # Adjust for ambient noise
            self.recognizer.adjust_for_ambient_noise(source)

            # Listen to the user's input
            audio_data = self.recognizer.listen(source)

            try:
                # Recognize the speech using Google Speech Recognition
                text = self.recognizer.recognize_google(audio_data)
                self.out_queue.append(str(text))
                self.log(f"Text succesfully processed, result: {str(text)}")
            except sr.UnknownValueError:
                self.log("could not unknown value error")
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