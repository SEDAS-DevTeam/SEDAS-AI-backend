from gtts import gTTS
import pyttsx3

from io import BytesIO
import time
import random
import os

from pydub import AudioSegment
from pydub.playback import play
from pydub.generators import WhiteNoise

class Base(object):
    #just a sample class that every other derives from
    def __init__(self, in_queue, out_queue, debug_queue):
        self.in_queue = in_queue #for incoming communication with core.py
        self.out_queue = out_queue #for out communication with core.py
        self.debug_queue = debug_queue

    def log(self, message):
        self.debug_queue.append(f"SPEECH-MODEL {message}")

    def process(self):
        while True:
            if len(self.in_queue) != 0:
                out = self.in_queue.pop(0)
                if out == "interrupt":
                    self.log("interrupt")
                    break
                else:
                    #output to process
                    split = out.split(":")
                    if "input" in split[0] and split[1]:
                        self.model_process(split[1])

class PyTTSx3(Base):
    def __init__(self, db_instance):
        super(PyTTSx3, self).__init__(db_instance)

        self.NOISE_FACT = [40, 35, 30]
        self.ACCENT_DICT = ["en-scottish", "en-westindies", "english-north", "english_rp", "english_wmids", "english-us", "english"]
        self.RATE_MOD = [40, 60, 80]
    
    def process(self):
        time.sleep(5)

        while True:
            start = self.db_instance.get("start")
            if start == "false":
                continue

            interrupt = self.db_instance.get("terminate")
            if interrupt == "true":
                self.log("interrupt")
                break

            text = self.db_instance.get("gen-speech")
            if text != self.last_value:
                #
                #PyTTSx3 main code
                #

                self.log("generating speech data")

                tts = pyttsx3.init()

                rate = tts.getProperty('rate')
                tts.setProperty('rate', rate - random.choice(self.RATE_MOD))

                tts.setProperty("voice", random.choice(self.ACCENT_DICT))
                tts.save_to_file("Oscar kilo lima four four five five fly heading zero niner zero", "output.mp3")
                tts.runAndWait()

                song = AudioSegment.from_file("output.mp3")

                #delete temporary output.mp3 file
                os.remove("output.mp3")

                noise = WhiteNoise().to_audio_segment(duration=len(song))
                noise = noise - random.choice(self.NOISE_FACT)

                combined = song.overlay(noise)

                self.last_value = text
                self.log("playing speech data")
                play(combined)

class GoogleTextToSpeech(Base):
    def __init__(self, in_queue, out_queue, debug_queue):
        super(GoogleTextToSpeech, self).__init__(in_queue, out_queue, debug_queue)

        self.NOISE_FACT = [40, 35, 30]
        self.ACCENT_DICT = ["com.au", "co.uk", "us", "ca", "co.in", "ie", "co.za"]
        self.SLOW_SPEECH_FACT = [True, False]

    def model_process(self, inp):
        self.log("generating speech data")

        self.bytes_obj = BytesIO()

        self.gtts = gTTS(text=inp, tld=random.choice(self.ACCENT_DICT), slow=random.choice(self.SLOW_SPEECH_FACT))
        self.gtts.write_to_fp(self.bytes_obj)

        self.bytes_obj.seek(0)
        song = AudioSegment.from_file(self.bytes_obj, sample_width=2, frame_rate=44100, channels=1)

        noise = WhiteNoise().to_audio_segment(duration=len(song))
        noise = noise - random.choice(self.NOISE_FACT)

        combined = song.overlay(noise)

        self.log("playing speech data")
        play(combined)

class PyTTS_gTTS_ensemble(Base):
    def __init__(self, db_instance):
        super(PyTTS_gTTS_ensemble, self).__init__(db_instance)

        #PyTTSx3
        self.pytts_NOISE_FACT = [40, 35, 30]
        self.pytts_ACCENT_DICT = ["en-scottish", "en-westindies", "english-north", "english_rp", "english_wmids", "english-us", "english"]
        self.pytts_RATE_MOD = [40, 60, 80]

        #gTTS
        self.gtts_NOISE_FACT = [40, 35, 30]
        self.gtts_ACCENT_DICT = ["com.au", "co.uk", "us", "ca", "co.in", "ie", "co.za"]
        self.gtts_SLOW_SPEECH_FACT = [True, False]

    def process(self):
        selector = "male" #TODO rework

        #test
        time.sleep(5)

        while True:
            start = self.db_instance.get("start")
            if start == "false":
                continue

            #interrupt through redis
            interrupt = self.db_instance.get("terminate")
            if interrupt == "true":
                self.log("interrupt")
                break

            text = self.db_instance.get("gen-speech")
            if text != self.last_value:
                self.log("generating speech data")
                if selector == "male":
                    #use PyTTSx3

                    tts = pyttsx3.init()

                    rate = tts.getProperty('rate')
                    tts.setProperty('rate', rate - random.choice(self.RATE_MOD))

                    tts.setProperty("voice", random.choice(self.ACCENT_DICT))
                    tts.save_to_file("Oscar kilo lima four four five five fly heading zero niner zero", "output.mp3")
                    tts.runAndWait()

                    song = AudioSegment.from_file("output.mp3")

                    #delete temporary output.mp3 file
                    os.remove("output.mp3")

                elif selector == "female":
                    #use gTTS

                    self.bytes_obj = BytesIO()

                    self.gtts = gTTS(text=text, tld=random.choice(self.ACCENT_DICT), slow=random.choice(self.SLOW_SPEECH_FACT))
                    self.gtts.write_to_fp(self.bytes_obj)

                    self.bytes_obj.seek(0)
                    song = AudioSegment.from_file(self.bytes_obj, sample_width=2, frame_rate=44100, channels=1)

                noise = WhiteNoise().to_audio_segment(duration=len(song))
                noise = noise - random.choice(self.NOISE_FACT)

                combined = song.overlay(noise)

                self.last_value = text
                self.log("playing speech data")
                play(combined)

SPEECH_MODEL_DICT = {
    "GoogleTTS": GoogleTextToSpeech,
    "PyTTSx3": PyTTSx3,
    "PyTTSx3 gTTS ensemble": PyTTS_gTTS_ensemble 
}

if __name__ == "__main__":
    pass
    """
    ###FEMALE VOICE
    from pydub.generators import WhiteNoise
    import random

    NOISE_FACT = [40, 35, 30]
    ACCENT_DICT = ["com.au", "co.uk", "us", "ca", "co.in", "ie", "co.za"]
    SLOW_SPEECH_FACT = [True, False]
    
    bytes_obj = BytesIO()

    gtts = gTTS(text="OKL4455 fly heading 090", tld=random.choice(ACCENT_DICT), slow=random.choice(SLOW_SPEECH_FACT))
    gtts.write_to_fp(bytes_obj)

    bytes_obj.seek(0)
    song = AudioSegment.from_file(bytes_obj, sample_width=2, frame_rate=44100, channels=1)

    noise = WhiteNoise().to_audio_segment(duration=len(song))
    noise = noise - random.choice(NOISE_FACT)

    combined = song.overlay(noise)
    play(combined)
    """
    import redis

    r_instance = redis.Redis(host='localhost', decode_responses=True)
    ensemble = PyTTS_gTTS_ensemble(r_instance)