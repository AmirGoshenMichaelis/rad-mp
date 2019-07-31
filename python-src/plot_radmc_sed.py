#! /usr/bin/python3

import os
import sys
import getopt
import numpy as np
from matplotlib import pyplot as plt
from scipy import integrate

# freq  = 1.0e4*cc/lam # lam in microns [1.0e-6] and light speed in cm [1.0e-2] all in all a factor of 1.0e4

def readSpectrum(fname):
    """Reads the spectrum / SED


    Parameters
    -----------
    fname : str, optional
            Name of the file to be read

    Returns
    -------

        Returns an ndarray with [Nwavelength, 2] dimensions 
        [Nwavelength,0] is the wavelength / velocity and
        [Nwavelength,1] is the flux density
        
    """
  
    rfile = open(fname, 'r')
    # Read the format number
    dum = rfile.readline()
    # Read the number of wavelengths 
    nwav = int(rfile.readline())
    # Read a blank line
    dum = rfile.readline()
    
    res = np.zeros([nwav, 2], dtype=np.float64)
    for iwav in range(nwav):
        dum = rfile.readline().split()
        res[iwav,0] = float(dum[0])
        res[iwav,1] = float(dum[1])
    rfile.close()

    return res



def process_data(data_working_diractory = '.'):
    cc   = 2.9979245800000e10 # Light speed [cm/s]
    fig  = plt.figure()
    s    = readSpectrum(os.path.join(data_working_diractory,'spectrum.out'))
    lam  = s[:,0] * 1.0e3
    fnu  = s[:,1]
    nu   = 1.0e4*cc/s[:,0]
    flam = 1.0e8*fnu*cc/s[:,0]**2
    plt.plot(lam, np.log10(flam))
    # plt.gca().invert_yaxis()
    plt.xlabel('$\lambda\; [\mathrm{nm}$]')
    plt.ylabel(r'$\log\, F_\lambda \; [\mathrm{erg}\,\mathrm{cm}^{-2}\,\mathrm{s}^{-1}\,\mathrm{cm}^{-1}]$')
    plt.savefig(os.path.join(data_working_diractory, 'spectrum.png'))
    # plt.show()
    # flux = np.sum(flam)
    # flux = np.trapz(flam, lam*1.0e-4)
    flux = integrate.simps(flam, s[:,0]*1.0e-4)
    with open(os.path.join(data_working_diractory, 'flux.out'), 'wt') as f:
        f.write('{} Simpson rule integration ( F_lambda erg cm^-2 s^-1 cm^-1 * d lambda [cm])\n'.format(flux))
    
    return flux


def print_usage():
    print("Usage:  {} --dir data working directory ".format(sys.argv[0]))
    print("")
    print("Usage:  {} -d data working directory ".format(sys.argv[0]))
    print("")
    print("Usage: {} --help or -h print this help".format(sys.argv[0]))



def main():
    working_directory = '.'

    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'd:h', ['dir=', 'help'])
    except getopt.GetoptError as err:
        print(err)
        print_usage()
        sys.exit(2)
    for opt, val in optlist:
        if opt in ("-d", "--dir"):
            working_directory = val
        elif opt in ("-h", "--help"):
            print_usage()
            return
        else:
            assert False, "unhandled option"
    process_data(working_directory)



if __name__ == "__main__":
    main()
