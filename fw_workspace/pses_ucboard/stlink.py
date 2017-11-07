
import os
import subprocess
import shlex
import argparse


stlinklocs = (R"C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\st-link utility\st-link_cli.exe",)

parser = argparse.ArgumentParser(description='Invokes STM32 ST-LINK Utility.',
                                 formatter_class=argparse.RawTextHelpFormatter)

parser.add_argument('--hexfile',
                    help='name of binary file (*.hex or *.bin) to be flashed\n'
                         'into controller (full path, w/ extension)\n'
                         'Controller is reseted afterwards as long as\n'
                         'option --noreset is not used.')

parser.add_argument('--reset', dest='reset', action='store_true',
                    help='reset controller\n'
                         'intended to use without --hexfile to reset controller\n'
                         'without flashing')

parser.add_argument('--noreset', dest='noreset', action='store_true',
                    help='no controller reset after flashing\n'
                         '(this options takes precedence over --reset)')

parser.add_argument('--waitaftererror', dest='waitaftererror', action='store_true',
                    help='wait for user input if an error occured\n')

args = parser.parse_args()

hexfile = args.hexfile
resetuc = args.reset
noresetuc = args.noreset
waitaftererror = args.waitaftererror

if hexfile and not noresetuc:
    resetuc = True

if noresetuc:
    resetuc = False

res = 0

stlinkfound = False

for stlink in stlinklocs:
    if os.path.exists(stlink):
        stlinkfound = True;
        break;

if not stlinkfound:
    print("STM32 ST-Link Utility not found!")
    res = 1

if res == 0 and hexfile:
    if os.path.exists(hexfile):
        stlinkcmd = '"{0:s}" -P "{1:s}" 0x08000000 -V "after_programming"'.format(stlink, hexfile)

        args = shlex.split(stlinkcmd)
        p = subprocess.Popen(args)
        res = p.wait()
    else:
        print('Binary file "{0:s}" not found!'.format(hexfile))
        res = 1

if res == 0 and resetuc:
    print("Reset controller.")
    stlinkcmd = '"{0:s}" -Rst'.format(stlink)
    args = shlex.split(stlinkcmd)
    p = subprocess.Popen(args, stdout=subprocess.DEVNULL)
    res = p.wait()

    if not res == 0:
        print('Error while reseting uC!')

if res != 0 and waitaftererror:
    print("")
    input("Press any key to continue ...")
