from gtts import gTTS
from gtts.tokenizer import Tokenizer, pre_processors, tokenizer_cases

from io import BytesIO
from pydub import AudioSegment
from pydub.playback import play

class Google:
    def __init__(self, db_instance):
        self.bytes_obj = BytesIO()
        self.db_instance = db_instance

    def process(self):
        last_value = ""
        while True:
            text = self.db_instance.get("gen-speech")

            if text != last_value:
                self.gtts = gTTS(text=text, lang="en")
                self.gtts.write_to_fp(self.bytes_obj)

                last_value = text

                self.play_audio()

    def play_audio(self):
        self.bytes_obj.seek(0)
        song = AudioSegment.from_file(self.bytes_obj, sample_width=2, frame_rate=44100, channels=1)
        play(song)

if __name__ == "__main__":
    speech_synth = Google()