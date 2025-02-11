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

### Cloning

``` shell
git clone --recursive https://github.com/SEDAS-DevTeam/SEDAS-AI-backend.git
cd SEDAS-AI-backend
```

### Installation

1) Whole library is built around the `invoke` library. There are some requirements to meet in the `requirements.txt`, I recommend **venv** for this.
2) To get started, clone this repository, then `cd src`. To get **TTS** resources, run `invoke fetch-resources`. For the **ASR** part, you unfortunately have to copy resulting binaries from [ATC-whisper](https://github.com/SEDAS-DevTeam/ATC-whisper) repository.

#### For testing (outputs `test` binary)

``` shell
invoke build --DTESTING=ON
invoke run test
```

#### For Integration (outputs `main` binary)

The integration part runs standalone and communicates using socket communication, by default it uses port 65 432 (**TODO**), to run the main part:

``` shell
invoke build --DTESTING=OFF
invoke run main
```

To enable communication (for the integration testing purpose), run command below in separate terminal:

``` shell
invoke test-main
```

This will setup commander/writer that connects to the socket server and sends user-prompted commands.

**Some accepted commands:**

- `start-mic` - starts mic recording
- `stop-mic` - stops mic recording
- `register [callsign (string)] [noise-intensity (float)]` - registers pseudopilot to communicate with user (write without brackets)
- `unregister [callsign (string)]` - unregisters pseudopilot (without brackets)
- `quit` - terminates main file

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

*NOTE:* For the future, I am still deciding if this repository isn't going to be merged with the **SEDAS - manager** parent repo. This subproject is either going to be added as a submodule or merged.

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
- [ ] Find a way how to package large asr models
- [x] return segmented command and values back to main

## TODO for next versions

- [ ] Get rid of ffmpeg shell calling to check correct audio type
- [ ] Rework into multithreading for faster processing
- [x] Don't suppress colors when building
- [ ] Fix libncurses terminal window hell :(
- [ ] Change TextProcessing algorithm into something more stable