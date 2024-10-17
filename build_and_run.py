#!/usr/bin/python3

import subprocess
import os
import signal
import time

PURPLE = '\033[0;35m'
BLUE = '\033[0;34m'
NC = '\033[0m'


def run_command(command):
    comm_args = command.split(" ")
    with subprocess.Popen(comm_args, stdout=subprocess.PIPE, bufsize=1, universal_newlines=True) as p:
        try:
            for line in p.stdout:
                print(line, end='')
        except KeyboardInterrupt:
            print("Interrupted! Sending SIGINT to source file")
            p.send_signal(signal.SIGINT)
            p.wait()
            print("Subprocess terminated.")


if __name__ == "__main__":
    print(f"[{PURPLE}Build info{NC}] {BLUE}Building file...{NC}")
    run_command("g++ ./src/test.cpp -o ./build/test")
    print(f"[{PURPLE}Build info{NC}] {BLUE}Running built file...{NC}")
    run_command("./build/test")
