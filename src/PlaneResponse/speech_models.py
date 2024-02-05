from gtts import gTTS
from gtts.tokenizer import Tokenizer, pre_processors, tokenizer_cases

from io import BytesIO
from pydub import AudioSegment
from pydub.playback import play

class GoogleTextToSpeech:
    def __init__(self, db_instance):
        self.db_instance = db_instance

    def log(self, message):
        self.db_instance.set("debug-speech-model", "SPEECH-MODEL " + message)

    def process(self):
        last_value = ""
        while True:
            #interrupt through redis
            interrupt = self.db_instance.get("terminate")
            if interrupt == "true":
                self.log("interrupt")
                break

            text = self.db_instance.get("gen-speech")
            if text != last_value:
                self.log("generating speech data")

                self.bytes_obj = BytesIO()

                self.gtts = gTTS(text=text, lang="en")
                self.gtts.write_to_fp(self.bytes_obj)

                last_value = text

                self.log("playing speech data")
                self.play_audio()

    def play_audio(self):
        self.bytes_obj.seek(0)
        song = AudioSegment.from_file(self.bytes_obj, sample_width=2, frame_rate=44100, channels=1)
        play(song)

SPEECH_MODEL_DICT = {
    "GoogleTTS": GoogleTextToSpeech
}

if __name__ == "__main__":
    speech_synth = GoogleTextToSpeech()