from invoke import task
from pathlib import Path
from os.path import join

import json
import os
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


@task
def run(ctx):
    print("Running main project...")

    exec_directory = abs_path + "/build/test"

    ctx.run(exec_directory, pty=True)


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
