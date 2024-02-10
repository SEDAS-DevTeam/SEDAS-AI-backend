import spacy
import redis
import time

NATO_ALPHA = {
    "alpha": "A",
    "beta": "B",
    "charlie": "C",
    "delta": "D",
    "echo": "E",
    "foxtrot": "F",
    "golf": "G",
    "hotel": "H",
    "india": "I",
    "juliet": "J",
    "kilo": "K",
    "lima": "L",
    "mike": "M",
    "november": "N",
    "oscar": "O",
    "papa": "P",
    "quebec": "Q",
    "romeo": "R",
    "sierra": "S",
    "tango": "T",
    "uniform": "U",
    "victor": "V",
    "whiskey": "W",
    "x-ray": "X",
    "yankee": "Y",
    "zulu": "Z"
}

class simplePOS:
    #really simple POS tagging algorithm that works only for: "fly heading" commands, USE ONLY FOR DEVELOPEMENT
    #requires installing spacy pretrained english model using this command: python3 -m spacy download en_core_web_sm

    def __init__(self, db_instance):
        self.db_instance = db_instance
        self.nlp = spacy.load("en_core_web_sm")
    
    def log(self, message):
        self.db_instance.set("debug-text-model", "TEXT-MODEL " + message)

    def shorten_name(self, text):
        arr = text.split()
        out = ""
        
        temp_out = ""
        for i, token in enumerate(arr):
            if token in NATO_ALPHA.keys():
                if i == len(arr) - 1:
                    factor = -1
                else:
                    factor = +1
                
                if arr[i + factor] in NATO_ALPHA.keys():
                    #there is contiunity
                    temp_out += NATO_ALPHA[token]
                else:
                    #no contiunity
                    temp_out += NATO_ALPHA[token]
                    out += f"{temp_out} "
                    temp_out = ""
            else:
                out += f"{arr[i]} "

        #checking for plane names and appending numeric descriptors based on case-matching
        arr_out = out.split()
        out = ""
        skip = False
        for i, token in enumerate(arr_out):
            if skip:
                skip = False
                continue

            if token.isupper() and arr_out[i + 1].isnumeric():
                out += f"{token + arr_out[i + 1]} "
                skip = True
            else:
                out += f"{token} "

        return out

    def process(self):
        #test
        time.sleep(5)

        last_value = ""
        while True:
            start = self.db_instance.get("start")
            if start == "false":
                continue

            #interrupt through redis
            interrupt = self.db_instance.get("terminate")
            if interrupt == "true":
                self.log("interrupt")
                break
            
            text = self.db_instance.get("proc-voice")
            if text != last_value:
                self.log("incoming text to process")

                #onchange
                text = text.lower()

                #process plane name
                text = self.shorten_name(text) #convert to NATO alphabet

                doc = self.nlp(text)

                value = 0
                name = ""
                command = ""

                for i, token in enumerate(doc):
                    #searching for fly-heading command
                    if token.text == "fly" and doc[i + 1].text == "heading":
                        command = "change-heading"

                for i, token in enumerate(doc):
                    if command == "change-heading":
                        #searching for heading
                        if token.pos_ == "NUM" and doc[i - 1].text == "heading":
                            #sometimes speech recognition is buggy and prints out too much chars for heading (for ex.: 0900)
                            value = int(token.text[:3])
                            continue
                        
                        #searching for plane id
                        if any(char.isdigit() for char in token.text):
                            name = token.text

                if len(name) != 0 and len(command) != 0:
                    self.log("text fully processed")
                    self.db_instance.set("proc-voice-out", f"{name} {command} {value}")

                last_value = text

TEXT_MODEL_DICT = {
    "simplePOS": simplePOS
}

if __name__ == "__main__":
    r_instance = redis.Redis(host='localhost', port=6379, decode_responses=True)
    r_instance.set("terminate", "false")

    pos = simplePOS(r_instance)
    pos.process()