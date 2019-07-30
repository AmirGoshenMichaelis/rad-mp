#! /usr/bin/python3

import os
import sys
import getopt
import glob
from multiprocessing import Pool
import numpy as np
import setup_radmc_sed
import plot_radmc_sed
import gen_light_curve

# DEBUG 
# args = ["--chk_dir", "/home/amirm/Public/flash_thick_disk/object-result/test-temp", "--chk_db_prefix", "disk_hdf5_chk_"]



def preprocess_element(chk_file, radmc_db_dir, refinement_level, src_dir):
    radmc_db_dir__chk_file = os.path.join(radmc_db_dir, os.path.split(chk_file)[1])
    if not os.path.exists(radmc_db_dir__chk_file):
        os.mkdir(radmc_db_dir__chk_file)
    
    current_time = setup_radmc_sed.create_radmc_inp(radmc_db_dir__chk_file, chk_file, refinement_level)
    for file_to_copy in ['dustkappa_silicate.inp', 'dustopac.inp', 'radmc3d.inp']:
        os.system('cp '+os.path.join(src_dir, file_to_copy)+' '+radmc_db_dir__chk_file )

    return radmc_db_dir__chk_file



def process_element(run_radmc_cmd):
    os.system(run_radmc_cmd)



def postprocess_element(radmc_db_dir__chk_file):
    plot_radmc_sed.process_data(radmc_db_dir__chk_file)



def alternate_processing(run_radmc_cmd, radmc_db_dir__chk_file):
    os.system(run_radmc_cmd)
    plot_radmc_sed.process_data(radmc_db_dir__chk_file)


def process_db(num_proc, src_dir, db_dir, prefix, incl=20, phi=80):
    refinement_level = 2

    db = glob.glob(os.path.join(db_dir,prefix)+'*')
    radmc_db_dir = os.path.join(db_dir,'radmc3d_processing_db-incl-{:.2f}-phi-{:.2f}'.format(incl,phi))
    if not os.path.exists(radmc_db_dir):
        os.mkdir(radmc_db_dir)

    preprocessing_args = []
    for element in db:
        preprocessing_args.append( (element, radmc_db_dir, refinement_level, src_dir) )

    with Pool(num_proc) as p:
        preprocessing_result = p.starmap(preprocess_element, preprocessing_args)


    processing_args = []
    postprocessing_args = []
    alternate_processing_arg = []
    for radmc_db_dir__chk_file in preprocessing_result:
        run_radmc_cmd = 'cd {}; radmc3d sed incl {} phi {};'.format(radmc_db_dir__chk_file, incl, phi)
        processing_args.append(run_radmc_cmd)
        postprocessing_args.append(radmc_db_dir__chk_file)
        alternate_processing_arg.append( (run_radmc_cmd,radmc_db_dir__chk_file) )


    #########################
    # with Pool(num_proc) as p:
    #     p.map(process_element, processing_args)
    # with Pool(num_proc) as p:
    #     p.map(postprocess_element, postprocessing_args)
    #########################
    
    #########################
    with Pool(num_proc) as p:
        p.starmap(alternate_processing, alternate_processing_arg)
    #########################
    
    gen_light_curve.process(radmc_db_dir, prefix)



def print_usage():
    print("Usage:  {}".format(sys.argv[0]) + \
          " --chk_dir flash check point working directory --chk_db_prefix flash check point file prefix" + \
          " --np number of parallel processes --incl inclination angle --phi phi ")
    print("")
    print("Usage:  {} -d flash check point working directory -p flash check point file prefix -n number of parallel processes".format(sys.argv[0]))
    print("")
    print("Usage: {} --help or -h print this help".format(sys.argv[0]))



def main():
    working_directory       = '.'
    flash_checkpoint_prefix = ''
    num_proc = None

    incl = 20
    phi = 80

    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'd:p:n:h', ['chk_dir=', 'chk_db_prefix=', 'np=', 'incl=', 'phi=', 'help'])
    except getopt.GetoptError as err:
        print(err)
        print_usage()
        sys.exit(2)
    for opt, val in optlist:
        if opt in ("-d", "--chk_dir"):
            working_directory = val
        elif opt in ("-p", "--chk_db_prefix"):
            flash_checkpoint_prefix = val
        elif opt in ("-n", "--np"):
            num_proc = int(val)
        elif opt=="--incl":
            incl = float(val)
        elif opt=="--phi":
            phi = float(val)
        elif opt in ("-h", "--help"):
            print_usage()
            return
        else:
            assert False, "unhandled option"
    process_db(num_proc, os.getcwd(), working_directory, flash_checkpoint_prefix, incl, phi)



if __name__ == "__main__":
    main()
