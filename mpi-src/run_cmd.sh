#! /usr/bin/bash
module load mpi
mpirun --use-hwthread-cpus  -np 3 main --chk_dir ~/Public/flash_thick_disk/object-result/res-004/ --chk_prefix disk_hdf5_chk --py_radmc_dir ../python-src/ --incl 0.0 -- phi 0.0 --python_yt /usr/bin/python3 --output_main_dir rad-out

