from gtts import gTTS
from gtts.tokenizer import Tokenizer, pre_processors, tokenizer_cases

from io import BytesIO
from pydub import AudioSegment
from pydub.playback import play

class Google(gTTS):
    def __init__(self, text, tld="com", lang="en", slow=False, lang_check=True, pre_processor_funcs=..., tokenizer_func=Tokenizer([tokenizer_cases.tone_marks, tokenizer_cases.period_comma, tokenizer_cases.colon, tokenizer_cases.other_punctuation]).run):
        super().__init__(text, tld, lang, slow, lang_check, pre_processor_funcs, tokenizer_func)

    def process(self):
        self.bytes_obj = BytesIO()
        self.write_to_fp(self.bytes_obj)

    def play_audio(self):
        audio = AudioSegment.from_file(self.bytes_obj, format="mp3")
        play(audio)

if __name__ == "__main__":
    speech_synth = Google(text="Fly heading 090", lang="en")
    speech_synth.process()
    speech_synth.play_audio()