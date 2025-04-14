## About

This is modular port for **SEDAS-manager** that contains all the code responsible for running three main tasks when it comes to simulating *Pseudopilots* - **ASR** (Automatic speech recognition), **PostProcessing** and **Speech synthesis**.

All these processes are connected with threads and queues, the whole architecture is therefore non-blocking and allows request stacking.

## Dependencies

for the C++ part:

- `curl`
- `libncurses`
- `portaudio`
- `libsndfile`

for the Python part:

- `requests`
- `invoke`

## Installation/Usage

Installation steps are located in the [SEDAS documentation](https://sedas-docs.readthedocs.io/en/latest/user-manual.html#app-installation)

## Architecture

Configurations are written in `json` files. It is because the parent project uses this extensions for all its configuration.

There are 3 segments of this code:

1) **Speech recognition** - made with the use of `whisper.cpp` project. Using `BUT-FIT` Whisper checkpoint for more accurate ATC speech detection
2) **Text processing** - algorithm, that processes recognized ATC commands into commands that are ported into **SEDAS - manager** (*NOTE:* Detection is currently hardcoded, working partly just as **PoC** [Proof of concept], definitely going to change in future versions)
3) **Speech Synthesis** - done with the use of `piper TTS` project. Currently using only **english** models

To be more specific, here you can see a small flow diagram:

`record.h` (**1**) &rarr; `voice_recog.h` (**2**) &rarr; `process.h` (**3**) &rarr; `classify.h` (**4**)

**1)** mic recording from keypress <br>
**2)** voice recognition using Whisper binary <br>
**3)** simple text processing (callsign and value extraction) <br>
**4)** more robust classification of corresponding text, to determine command

## TODO

- [x] Better, detailed readme
- [x] finish ATC speech recognition
- [x] finish processing
- [x] finish speech synthesis
- [x] incorporate into SEDAS-manager (finish integrate.cpp + set rules)
- [x] Add json submodule
- [x] Add how-to-run in README
- [x] Modify the whole fetching aparatus
- [x] Add keybinds
- [x] Add wav recording using PortAudio
- [x] Maybe rewrite to CMake?
- [x] Fix debugging
- [x] Fix wav recording + add whisper processing part
- [x] Restructure code, get sentence from whisper stdout
- [x] Better parsing for Whisper out
- [ ] Change socket server to socket client and vice versa (also corresponds with **SEDAS-manager issue**)
- [x] Find a way how to package large asr models
- [x] return segmented command and values back to main
- [x] Add "Say again" response
- [x] Add or cases
- [ ] Fix the feet/flight-level responses (feet currently do not work) + add FL and feet limits
- [x] Better "Say again" response
- [ ] Rework into multithreading for faster processing
- [x] Getting model file doesn't exist in Piper subprocess
- [ ] Sometimes, bluetooth devices do not work well (Portaudio errors)

## TODO for next versions

- [ ] Get rid of ffmpeg shell calling to check correct audio type
- [x] Don't suppress colors when building
- [ ] Fix libncurses terminal window hell :(
- [ ] Change TextProcessing algorithm into something more stable
- [ ] Add time benchmarks + try to optimize
