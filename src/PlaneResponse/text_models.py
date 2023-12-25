import spacy

class simplePOS:
    #really simple POS tagging algorithm that works only for: "fly heading" commands, USE ONLY FOR DEVELOPEMENT
    #requires installing spacy pretrained english model using this command: python3 -m spacy download en_core_web_sm

    def __init__(self, db_instance):
        self.db_instance = db_instance
        self.nlp = spacy.load("en_core_web_sm")

    def heading_translation(self, head_text):
        #sometimes speech recognition is buggy and prints out too much chars for heading (for ex.: 0900)
        head = head_text[:3]
        if head[0] == "0": return int(head[1:])
        else: return int(head)

    def process(self):
        last_value = ""
        while True:
            text = self.db_instance.get("proc-voice")
            if text != last_value:
                #onchange

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
                            value = self.heading_translation(token.text)
                            continue
                        
                        #searching for plane id
                        if any(char.isdigit() for char in token.text):
                            name = token.text

                if len(name) != 0 and len(command) != 0:
                    self.db_instance.set("proc-voice-out", f"{name} {command} {value}")


if __name__ == "__main__":
    pos = simplePOS()
    name, heading = pos.process("Uhmmm, OKD2277 fly heading 090")
    print(name, heading)