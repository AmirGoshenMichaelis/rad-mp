#! /usr/bin/python3

import os
import sys
import getopt
import yt
import numpy as np
#from yt.analysis_modules.radmc3d_export.api import RadMC3DWriter, RadMC3DSource
# Development of the radmc3d_export module has been moved to the yt_astro_analysis package.
# obsolete/deprecated replase with:
from yt.extensions.astro_analysis.radmc3d_export.api import RadMC3DWriter, RadMC3DSource

# DEBUG:
# args = ["--dir", "/home/amirm/Public/flash_thick_disk/object-result/res-012/radmc-wd/", "--flash_chk", "disk_hdf5_chk_0178"]



############################################################
def write_source_files_with_path(path, sources, wavelengths):
    '''
    This function creates the stars.inp and wavelength_micron.inp
    files that RadMC3D uses for its dust continuum calculations.
    Parameters
    ----------
    sources: a list of RadMC3DSource objects
        A list that contains all the sources you would like yt
        to create
    wavelengths: np.array of float values
        An array listing the wavelength points you would like to
        use the radiative transfer calculation
    '''
    nstars = len(sources)
    nlam = len(wavelengths)
    filename = os.path.join(path,'stars.inp')
    fhandle = open(filename, 'w')
    # write header
    fhandle.write('2 \n')  # a format flag that should always be 2
    fhandle.write('%d    %d \n' % (nstars, nlam))
    # write source information
    for source in sources:
        fhandle.write(str(source.radius) + ' ')
        fhandle.write(str(source.mass) + ' ')
        fhandle.write('%f %f %f' %(source.position[0], \
                                   source.position[1], \
                                   source.position[2]))
        fhandle.write('\n')
    # write wavelength information
    for wavelength in wavelengths:
        fhandle.write('%f \n' % wavelength)
    # finally write blackbody temperature for each source
    for source in sources:
        # the negative sign is a flag used internally
        # by RadMC3D to indicate that this is a blackbody
        # source
        fhandle.write('%f \n' % -source.temperature)
    # done with stars.inp
    fhandle.close()
    # now do the wavelength_micron.inp file
    filename = os.path.join(path,'wavelength_micron.inp')
    fhandle = open(filename, 'w')
    fhandle.write('%d \n' % nlam)
    for wavelength in wavelengths:
        fhandle.write('%f \n' % wavelength)
    # done with both
    fhandle.close()
############################################################



def create_radmc_inp(working_directory, flash_checkpoint_file, refinement_level):
    ds = yt.load(flash_checkpoint_file)
    with open(os.path.join(working_directory,'current_code_time.out'), 'wt') as f:
        f.write("{}\n".format(ds.current_time))
    writer = RadMC3DWriter(ds, max_level=refinement_level)
    writer.grid_filename = os.path.join(working_directory,"amr_grid.inp")
    writer.write_amr_grid()

    m_H = yt.utilities.physical_constants.mass_hydrogen_cgs
    M_star = 8.0
    R_star = 4.4485
    radius_cm = 6.96e10*R_star
    mass_g   = 1.989e33*M_star
    position_cm = [0.0, 0.0, 0.0]
    temperature_K = 21000.0

    star = RadMC3DSource(radius_cm, mass_g, position_cm, temperature_K)
    sources_list = [star]
    wavelengths_micron = np.linspace(100.0e-3, 2000e-3, 2000)
    write_source_files_with_path(working_directory, sources_list, wavelengths_micron)

    # The gradient operator requires periodic boundaries.  This dataset has
    # open boundary conditions.  We need to hack it for now (this will be fixed
    # in future version of yt)
    ds.periodicity = (True, True, True)

    def _envelope_Density(field, data):
        # outside_region = (data["x"]*data["y"]*data["z"])>=radius_cm**2
        # data["density"][outside_region] = 2.0e-16
        return data["density"]
    ds.add_field(("gas", "envelope_density"), function=_envelope_Density, units="g/cm**3", sampling_type='cell')

    def _envelope_Density_Number(field, data):
        # outside_region = (data["x"]*data["y"]*data["z"])>=radius_cm**2
        # data["density"][outside_region] = 2.0e-16
        return data["density"]/m_H
    ds.add_field(("gas", "envelope_density_number"), function=_envelope_Density_Number, units="1/cm**3", sampling_type='cell')

    writer.write_dust_file(("gas", "envelope_density"), os.path.join(working_directory,"dust_density.inp"))
    writer.write_dust_file(("gas", "envelope_density_number"), os.path.join(working_directory,"electron_numdens.inp"))
    writer.write_dust_file(("gas", "envelope_density_number"), os.path.join(working_directory,"ion_numdens.inp"))

    # def _envelope_Temperature_ELE(field, data):
    #     # outside_region = (data["x"]*data["y"]*data["z"])>=radius_cm**2
    #     # data["tele"][outside_region] = 1000.0
    #     return data["tele"]
    # ds.add_field(("gas", "envelope_tele"), function=_envelope_Temperature_ELE, units="K", sampling_type='cell')

    # def _envelope_Temperature_ION(field, data):
    #     # outside_region = (data["x"]*data["y"]*data["z"])>=radius_cm**2
    #     # data["tion"][outside_region] = 1000.0
    #     return data["tion"]
    # ds.add_field(("gas", "envelope_tion"), function=_envelope_Temperature_ION, units="K", sampling_type='cell')

    # writer.write_dust_file(("gas", "envelope_tele"), os.path.join(working_directory,"dust_temperature.dat"))
    # writer.write_dust_file(("gas", "envelope_tion"), os.path.join(working_directory,"gas_temperature.inp"))

    # print('Run \n\tradmc3d sed incl 20 phi 80')
    # os.system('radmc3d sed incl 20 phi 80')
    return float(ds.current_time)
############################################################



def print_usage():
    print("Usage:  {} --dir working directory --flash_chk flash check point file --level refinement grid level to process".format(sys.argv[0]))
    print("")
    print("Usage:  {} -d working directory -f flash check point file -l refinement grid level to process".format(sys.argv[0]))
    print("")
    print("Usage: {} --help or -h print this help".format(sys.argv[0]))
############################################################



def main():
    working_directory     = '.'
    flash_checkpoint_file = ''
    refinement_level      = 2

    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'd:f:l:h', ['dir=', 'flash_chk=', 'level=', 'help'])
    except getopt.GetoptError as err:
        print(err)
        print_usage()
        sys.exit(2)
    for opt, val in optlist:
        if opt in ("-d", "--dir"):
            working_directory = val
        elif opt in ("-f", "--flash_chk"):
            flash_checkpoint_file = val
        elif opt in ("-l" ,"--level"):
            refinement_level = int(val)
        elif opt in ("-h", "--help"):
            print_usage()
            return
        else:
            assert False, "unhandled option"
    create_radmc_inp(working_directory, flash_checkpoint_file, refinement_level)
############################################################



if __name__ == "__main__":
    main()
############################################################
