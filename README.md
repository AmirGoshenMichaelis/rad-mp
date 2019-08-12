# rad-mp (radmc3d-multi-process).
__Envelope for running RADMC3D on multi FLASH simulation checkpoints__
+ Running on a node with python multi-processing api.  
  Maximize the utilization of the node.
+ Or running on a cluster using MPI.
Running example:
  mpirun --use-hwthread-cpus  -np 3 main --chk_dir ~/flash/object/ --chk_prefix sod_hdf5_chk --py_radmc_dir ../python-src/ --incl 0.0 --phi 0.0 --python_yt /usr/bin/python3 --output_main_dir ./rad-out

