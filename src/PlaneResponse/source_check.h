void check_models(json config_data){
    std::string fetch_url = "https://huggingface.co/rhasspy/piper-voices/resolve/v1.0.0/";
    std::string synth_model_paths = main_path + "PlaneResponse/model/speech_synth/model_source/";

    std::vector<std::string> paths;
    for (const auto& model_path : fs::directory_iterator(synth_model_paths)){
        paths.push_back(model_path.path());
    }

    int prepend_size = paths[0].find("/model_source/") + std::string("/model_source/").size();
    std::string prepend_path = paths[0].erase(prepend_size, paths[0].size() - prepend_size);

    json model_array = config_data["models"];
    for (const auto& model : model_array){
        std::string full_path_onnx = prepend_path + model.get<std::string>() + ".onnx";
        std::string full_path_json = prepend_path + model.get<std::string>() + ".onnx.json";

        bool onnx_found = search_vector(paths, full_path_onnx);
        bool json_found = search_vector(paths, full_path_json);

        if (!onnx_found){
            std::cout << "onnx file for: " + model.get<std::string>() + " is missing, refetching..." << std::endl;
        }

        if (!json_found){
            std::cout << "json file for: " + model.get<std::string>() + " is missing, refetching..." << std::endl;
        }
    }
}

void refetch_missing(){

}