## About

This is modular port for **SEDAS-manager** that contains all the code responsible for running three main tasks when it comes to simulating *Pseudopilots* - **ASR** (Automatic speech recognition), **PostProcessing** and **Speech synthesis**.

All these processes are connected with threads and queues, the whole architecture is therefore non-blocking and allows request stacking.

## TODO

- [ ] Better detailed readme
- [ ] finish ATC speech recognition
- [ ] finish processing
- [x] finish speech synthesis
- [ ] incorporate into SEDAS-manager