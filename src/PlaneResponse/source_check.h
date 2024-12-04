void check_models(json config_data){

    std::string synth_model_paths = main_path + "PlaneResponse/model/speech_synth/model_source/";

    std::vector<std::string> paths;
    for (const auto& model_path : fs::directory_iterator(synth_model_paths)){
        paths.push_back(model_path.path());
        std::cout << model_path.path() << std::endl;
    }
}

void refetch_missing(){

}