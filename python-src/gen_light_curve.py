#! /usr/bin/python3

import os
import sys
import getopt
import glob



def process(radmc_db_dir, prefix, verbose = False):

    data = []
    db = glob.glob(os.path.join(radmc_db_dir, prefix)+'*')
    for item in db:
        fn_flux = os.path.join(item,'flux.out')
        fn_time = os.path.join(item,'current_code_time.out')
        flux = 0 # or float('nan')
        time = 0 # or float('nan')
        if os.path.exists(fn_flux) and os.path.exists(fn_time):
            with open(fn_flux,'rt') as f:
                flux = float( f.readline().split()[0] )
            with open(fn_time,'rt') as f:
                time = float( f.readline().split()[0] )
            data.append( (time,flux) )
        if verbose:
            print('trying flux file: {}'.format(fn_flux))
            print('trying time file: {}'.format(fn_time))
            print('with result flux: {: .5e} time {:.3f}'.format(flux, time))

    with open(os.path.join(radmc_db_dir, 'light_curve.out'), 'wt') as f:
        for time, flux in data:
            f.write('{} {}\n'.format(time, flux))

    return data



def print_usage():
    print("Usage:  {} --chk_dir radmc-3d processing main directory --chk_db_prefix file prefix --verbose".format(sys.argv[0]))
    print("")
    print("Usage:  {} -d radmc-3d processing main directory -p file prefix -v".format(sys.argv[0]))
    print("")
    print("Usage: {} --help or -h print this help".format(sys.argv[0]))



def main():
    working_directory  = '.'
    file_prefix        = ''
    verbose            = False

    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'd:p:hv', ['chk_dir=', 'chk_db_prefix=', 'help', 'verbose'])
    except getopt.GetoptError as err:
        print(err)
        print_usage()
        sys.exit(2)
    for opt, val in optlist:
        if opt in ("-d", "--chk_dir"):
            working_directory = val
        elif opt in ("-p", "--chk_db_prefix"):
            file_prefix = val
        elif opt in ("-v", "--verbose"):
            # Verbosely list files processed.
            verbose = True
        elif opt in ("-h", "--help"):
            print_usage()
            return
        else:
            assert False, "unhandled option"
    process(working_directory, file_prefix, verbose)



if __name__ == "__main__":
    main()


