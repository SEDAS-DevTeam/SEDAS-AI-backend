from invoke import task


# some definitions
def add_args(command, *args):
    res_command = command
    for arg in args:
        res_command += " " + arg

    return res_command


@task
def build(ctx):
    print("Building main project...")
    ctx.run(add_args("g++", "./test", "./test.cpp"))


@task
def run(ctx):
    print("Running main project...")
