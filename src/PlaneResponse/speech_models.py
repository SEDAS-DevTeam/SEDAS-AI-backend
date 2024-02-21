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
    #just a sample class to pass db_instance correctly using inheritance
    def __init__(self, db_instance): pass

class PyTTSx3(Base):
    def __init__(self, db_instance):
        super(PyTTSx3, self).__init__(db_instance)

        self.db_instance = db_instance
        self.last_value = ""

        self.NOISE_FACT = [40, 35, 30]
        self.ACCENT_DICT = ["en-scottish", "en-westindies", "english-north", "english_rp", "english_wmids", "english-us", "english"]
        self.RATE_MOD = [40, 60, 80]
    
    def log(self, message):
        self.db_instance.set("debug-speech-model", "SPEECH-MODEL " + message)
    
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
    def __init__(self, db_instance):
        super(GoogleTextToSpeech, self).__init__(db_instance)

        self.db_instance = db_instance
        self.last_value = ""

        self.NOISE_FACT = [40, 35, 30]
        self.ACCENT_DICT = ["com.au", "co.uk", "us", "ca", "co.in", "ie", "co.za"]
        self.SLOW_SPEECH_FACT = [True, False]

    def log(self, message):
        self.db_instance.set("debug-speech-model", "SPEECH-MODEL " + message)

    def process(self):
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

class PyTTS_gTTS_ensemble(PyTTSx3, GoogleTextToSpeech):
    def __init__(self, db_instance):
        super(PyTTS_gTTS_ensemble, self).__init__(db_instance)

SPEECH_MODEL_DICT = {
    "GoogleTTS": GoogleTextToSpeech,
    "PyTTSx3": PyTTSx3,
    "PyTTS gTTS ensemble": PyTTS_gTTS_ensemble 
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