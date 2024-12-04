std::string fetch_url = "https://huggingface.co/rhasspy/piper-voices/resolve/v1.0.0/en/";
std::string synth_model_paths = main_path + "PlaneResponse/model/speech_synth/model_source/";

std::string reconstruct_url(std::string model_name, std::string file_type){
    std::string spec_fetch_url = fetch_url;
    std::vector<std::string> parsed = split_string(model_name, '-');
    spec_fetch_url += parsed[0] + "/" + parsed[1] + "/" + parsed[2] + "/" + model_name + file_type + "?download=true";
    return spec_fetch_url;
}

str_matrix check_models(json config_data){

    std::vector<std::string> paths;
    str_matrix to_install;
    for (const auto& model_path : fs::directory_iterator(synth_model_paths)){
        paths.push_back(model_path.path());
    }

    std::string ex_path = paths[0];
    int prepend_size = ex_path.find("/model_source/") + std::string("/model_source/").size();
    std::string prepend_path = ex_path.erase(prepend_size, ex_path.size() - prepend_size);

    json model_array = config_data["models"];
    for (const auto& model : model_array){
        std::string model_name = model.get<std::string>();
        std::string full_path_onnx = prepend_path + model_name + ".onnx";
        std::string full_path_json = prepend_path + model_name + ".onnx.json";

        bool onnx_found = search_vector(paths, full_path_onnx);
        bool json_found = search_vector(paths, full_path_json);

        if (!onnx_found){
            std::cout << "onnx file for: " + model_name + " is missing, refetching..." << std::endl;
            to_install.push_back({reconstruct_url(model_name, ".onnx"), full_path_onnx});
        }

        if (!json_found){
            std::cout << "json file for: " + model_name + " is missing, refetching..." << std::endl;
            to_install.push_back({reconstruct_url(model_name, ".onnx.json"), full_path_json});
        }
    }

    return to_install;
}

void refetch_missing(str_matrix install_list){
    for (auto& install_record : install_list){
        download_file_from_url(install_record[0], install_record[1]);
    }
}