from gtts import gTTS
from gtts.tokenizer import Tokenizer, pre_processors, tokenizer_cases

from io import BytesIO
from pydub import AudioSegment
from pydub.playback import play

class Google:
    def __init__(self, text):
        self.gtts = gTTS(text=text, lang="en")
        self.bytes_obj = BytesIO()

    def process(self):
        self.gtts.write_to_fp(self.bytes_obj)

    def play_audio(self):
        self.bytes_obj.seek(0)
        song = AudioSegment.from_file(self.bytes_obj, sample_width=2, frame_rate=44100, channels=1)
        play(song)

if __name__ == "__main__":
    speech_synth = Google(text="Fly heading zero niner zero")
    speech_synth.process()
    speech_synth.play_audio()