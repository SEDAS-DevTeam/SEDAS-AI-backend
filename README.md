## About

This is modular port for **SEDAS-manager** that contains all the code responsible for running three main tasks when it comes to simulating *Pseudopilots* - **ASR** (Automatic speech recognition), **PostProcessing** and **Speech synthesis**.

All these processes are connected with threads and queues, the whole architecture is therefore non-blocking and allows request stacking.

## Architecture

Configurations are written in `json` files. It is because the parent project uses this extensions for all its configuration.

There are 3 segments of this code:

1) **Speech recognition** - made with the use of `whisper.cpp` project. Using `BUT-FIT` Whisper checkpoint for more accurate ATC speech detection
2) **Text processing** - algorithm, that processes recognized ATC commands into commands that are ported into **SEDAS - manager** (*NOTE:* Detection is currently hardcoded, working partly just as **PoC** [Proof of concept], definitely going to change in future versions)
3) **Speech Synthesis** - done with the use of `piper TTS` project. Currently using only **english** models

*NOTE:* For the future, I am still deciding if this repository isn't going to be merged with the **SEDAS - manager** parent repo. This subproject is either going to be added as a submodule or merged.

## TODO

- [x] Better, detailed readme
- [ ] finish ATC speech recognition
- [ ] finish processing
- [x] finish speech synthesis
- [ ] incorporate into SEDAS-manager
- [ ] Add json submodule
- [ ] Change TextProcessing algorithm into something more stable