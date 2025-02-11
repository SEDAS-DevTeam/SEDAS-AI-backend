from invoke import task
from pathlib import Path
from os.path import join, exists

import json
import os
import time
import socket
import requests
import shutil


# some definitions
def add_args(command, *args):
    res_command = command
    for arg in args:
        res_command += " " + arg

    return res_command


def load_config(path):
    with open(path) as stream:
        return json.load(stream)


def reconstruct_url(res_url, model_name: str):
    split = model_name.split("-")
    res_url += f"{split[0]}/{split[1]}/{split[2]}/{model_name}"

    onnx_url = res_url + ".onnx?download=true"
    json_url = res_url + ".onnx.json?download=true"

    return onnx_url, json_url


def reconstruct_path(model_name):
    model_onnx = join(models_path, model_name + ".onnx")
    model_json = join(models_path, model_name + ".onnx.json")

    return model_onnx, model_json


def fetch_resource(url, path):
    response = requests.get(url)
    if response.status_code == 200:
        response.raise_for_status() # Raise an error for bad status codes
        with open(path, "wb") as file:
            file.write(response.content)


@task
def build(ctx, DTESTING="ON", REMOVEBUILD="ON"):
    print("Building main project...")

    os.chdir(abs_path)
    print(f"Currently in {os.getcwd()} directory")
    print(DTESTING)

    if REMOVEBUILD == "ON":
        try: shutil.rmtree(join(abs_path, "build"))
        except FileNotFoundError: pass

    ctx.run(f"cmake -B build -D TESTING={DTESTING}", pty=True)
    ctx.run("cmake --build build", pty=True)

    if DTESTING == "ON":
        src_bin_path = join(abs_path, "build/test")
        out_bin_path = join(abs_path, "project_build/test")
    else:
        src_bin_path = join(abs_path, "build/main")
        out_bin_path = join(abs_path, "project_build/main")

    src_tts_dir = join(abs_path, "src/PlaneResponse/models/tts")
    out_tts_dir = join(abs_path, "project_build/tts")

    src_asr_dir = join(abs_path, "src/PlaneResponse/models/asr")
    out_asr_dir = join(abs_path, "project_build/asr")

    src_config_dir = join(abs_path, "src/PlaneResponse/config")
    out_config_dir = join(abs_path, "project_build/config")

    print("Moving files to project_build")
    project_build_path = join(abs_path, "project_build")
    if not exists(project_build_path):
        os.mkdir(project_build_path)
        os.mkdir(join(project_build_path, "temp"))

        # move tts files
        shutil.copytree(src_tts_dir, out_tts_dir)

        # move asr files
        shutil.copytree(src_asr_dir, out_asr_dir)

        # move configs
        shutil.copytree(src_config_dir, out_config_dir)

    # move main executable
    shutil.copyfile(src_bin_path, out_bin_path)

    # give it execute permissions
    os.chmod(out_bin_path, 0o755)


@task
def run(ctx, exec):
    print("Running main project...")

    # TODO: rework for project build
    asr_path = join(abs_path, "project_build/asr")
    tts_path = join(abs_path, "project_build/tts")
    config_path = join(abs_path, "project_build/config")
    temp = join(abs_path, "project_build/temp")

    exec_directory = abs_path + f"/project_build/{exec} {asr_path} {tts_path} {config_path} {temp}"

    ctx.run(exec_directory, pty=True)


@task
def test_main(ctx):
    HOST = "localhost"
    PORT = 65432

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.settimeout(4.0)
        print("Connected as writer")

        try:
            while True:
                print("accepted messages are: start-mic, stop-mic, register, unregister, quit")
                message = input("Send message: ")
                s.sendall(message.encode())
                print("Sent: " + message)

                try:
                    response = s.recv(1024).decode()
                    print("Response: " + response)
                except socket.timeout:
                    pass

                if message == "quit":
                    print("terminated")
                    break
        except KeyboardInterrupt:
            print("Terminated")


@task
def clean(ctx):
    # remove build and project_build directories

    project_build_path = join(abs_path, "project_build")
    build_path = join(abs_path, "build")

    shutil.rmtree(project_build_path)
    shutil.rmtree(build_path)

    print("Cleaned project output files: /build, /project_build")


@task
def fetch_resources(ctx):
    print("Fetching model resources...")

    # Removing the existing ones
    for model in os.listdir(models_path):
        if ".gitkeep" in model:
            continue

        os.remove(join(models_path, model))

    for model_name in config_synth["models"]:
        onnx_url, json_url = reconstruct_url(models_url, model_name)
        onnx_path, json_path = reconstruct_path(model_name)

        print(f"Fetching {model_name}...")
        fetch_resource(onnx_url, onnx_path)
        fetch_resource(json_url, json_path)


# runtime
abs_path = str(Path(__file__).parents[1])

models_url = "https://huggingface.co/rhasspy/piper-voices/resolve/v1.0.0/en/"
models_path = join(abs_path, "src/PlaneResponse/models/tts/voices/")

config_synth = load_config(join(abs_path, "src/PlaneResponse/config/config_synth.json"))
