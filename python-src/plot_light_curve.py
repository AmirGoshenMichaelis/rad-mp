#! /usr/bin/python3
import os
import sys
import getopt
import numpy as np
from matplotlib import pyplot as plt


def read_light_curve(file_name):
    data = []
    with open(file_name, 'rt') as f:
        for line in f:
            row = map(float, line.split())
            data.append(list(row))
    data = np.array(data)
    increse_time = np.argsort(data[:,0])
    time = data[increse_time, 0]
    flux = data[increse_time, 1]
    return time, flux



def process(file_name, interactive = False):
    time, flux = read_light_curve(file_name)
    plt.plot(time, -np.log10(flux))
    plt.gca().invert_yaxis()
    plt.xlabel('t [Simulation time units]')
    plt.ylabel(r'$-\log \, F \; [\mathrm{erg}\, \mathrm{cm}^{-2}\, \mathrm{s}^{-1}$')
    plt.title('Light curve - Flux vs. Time')
    os.path.basename(file_name)
    fig_file_name = os.path.join(os.path.dirname(file_name),'light_curve.png')
    plt.savefig(fig_file_name)
    if interactive:
        plt.show()



def print_usage():
    print("Usage:  {} --file light curve file --not_interactive ".format(sys.argv[0]))
    print("")
    print("Usage:  {} -f light curve file".format(sys.argv[0]))
    print("")
    print("Usage: {} --help or -h print this help".format(sys.argv[0]))



def main():
    file_name   = 'light_curve.out'
    interactive = True

    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'f:h', ['file=', 'help', 'not_interactive'])
    except getopt.GetoptError as err:
        print(err)
        print_usage()
        sys.exit(2)
    for opt, val in optlist:
        if opt in ("-f", "--file"):
            file_name = val
        elif opt == "--not_interactive":
            interactive = False
        elif opt in ("-h", "--help"):
            print_usage()
            return
        else:
            assert False, "unhandled option"
    process(file_name, interactive)



if __name__ == "__main__":
    main()
