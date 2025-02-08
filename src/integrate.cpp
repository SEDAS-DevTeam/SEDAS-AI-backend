/*
    Integration file for SEDAS-manager
*/

#include "./include/utils.hpp"
#include "./PlaneResponse/voice_recog.hpp"
#include "./PlaneResponse/process.hpp"
#include "./PlaneResponse/classify.hpp"
#include "./PlaneResponse/speech_synth.hpp"

#include "./include/record.hpp"
#include "./include/source_check.hpp"
#include "./include/keypress.hpp"

int main(){
    Recorder recorder;
    recorder.initialize();
    Recognizer recognizer;

    Processor processor;
    Classifier classifier;
    //classifier.set_rules(); // TODO

    Synthesizer synthesizer;
    synthesizer.setup_model_registry();

    /*
    keypress_detector.mainloop(recorder,
                      recognizer,
                      processor,
                      classifier,
                      synthesizer);    
    */
}