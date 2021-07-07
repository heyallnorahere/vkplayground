import scripts
from scripts.util import strip_array
import subprocess
import sys
print("Syncing submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])
print("Finished syncing submodules.")
if len(sys.argv) < 2:
    print("No command specified!")
    exit(1)
args = strip_array(sys.argv, 2)
commands = {
    "config": scripts.config
}
try:
    command = commands[sys.argv[1]]
    print("Running command: {}".format(sys.argv[1]))
    exit(command(args))
except KeyError:
    print("The specified command was not found! Tried to call: \"{}\"".format(sys.argv[1]))
    exit(1)