from __future__ import print_function, absolute_import, division  # makes KratosMultiphysics backward compatible with python 2.6 and 2.7
from KratosMultiphysics import *                                  # importing the Kratos Library
from KratosMultiphysics.ManagerApplication import *
CheckForPreviousImport()                                          # check that KratosMultiphysics was imported in the main script
import shutil
from glob import glob
from math import pi, sin, cos, tan, atan, fabs

from os import system

def initialize_time_parameters(benchmark_number):

    number_of_coeffs_of_restitution = 1

    if benchmark_number==1:

        final_time                      = 6000
        dt                              = 30 # Complies Rayleigh's condition
        output_time_step                = 30
        number_of_points_in_the_graphic = 6

    elif benchmark_number==2:

        final_time                      = 2000
        dt                              = 20 #3e-7 # Complies Rayleigh's condition????????????????
        output_time_step                = 20
        number_of_points_in_the_graphic = 6


    else: #benchmark_number==28:        # Shear + radius expansion

        final_time                      = 100
        dt                              = 10
        output_time_step                = 10
        number_of_points_in_the_graphic = 1

    return final_time, dt, output_time_step, number_of_points_in_the_graphic, number_of_coeffs_of_restitution


class Benchmark1:

    def __init__(self):
        #self.initial_normal_vel = 10.0

    #def set_initial_data(self, modelpart, iteration, number_of_points_in_the_graphic, coeff_of_restitution_iteration=0):

        #for node in modelpart.Nodes:
            #if node.Id == 1:
                #node.SetSolutionStepValue(VELOCITY_X, -self.initial_normal_vel)
            #else:
                #node.SetSolutionStepValue(VELOCITY_X,  self.initial_normal_vel)

    #def generate_graph_points(self, modelpart, time, output_time_step, dt):
        #pass

    #def get_final_data(self, modelpart):
        #pass

    #def ApplyNodalRotation(self, time, dt, modelpart):
        #pass

    def print_results(self, number_of_points_in_the_graphic, dt=0):

        normal_contact_force_outfile_name = 'variables_for_node_1.txt'
        gnuplot_script_name = 'benchmark1_dt_' + str(dt) + 's.gp'
        self.gnuplot_outfile = open(gnuplot_script_name, 'w')
        self.gnuplot_outfile.write("set grid; plot '" + normal_contact_force_outfile_name + "' every 20 u 1:8 w lp lt -1 lw 1.5 ps 1 pt 4")
        self.gnuplot_outfile.close()
        #print_gnuplot_files_on_screen(gnuplot_script_name)

        error1, error2, error3 = self.compute_errors(normal_contact_force_outfile_name)

        error_filename = 'errors.txt'
        error_file = open(error_filename, 'a')
        error_file.write("DEM Benchmark 1:")

        if (error1 < 1.0 and error2 < 1.0 and error3 < 1.0):
            error_file.write(" OK!........ Test 1 SUCCESSFUL\n")
        else:
            error_file.write(" KO!........ Test 1 FAILED\n")
        error_file.close()

    def compute_errors(self, normal_contact_force_outfile_name):

        Chung_data = []; DEM_data = []

        with open('paper_data/benchmark1_graph1.dat') as inf:
            for line in inf:
                Chung_data.append(float(line))

        with open(normal_contact_force_outfile_name) as inf:
            for line in inf:
                parts = line.split()
                if parts[0] == '#Time':
                    break
            for line in inf:
                parts = line.split()
                DEM_data.append(float(parts[7]))

        error = fabs(max(DEM_data) - float(Chung_data[0]))/float(Chung_data[0])

        print("Error in restitution numbers =", 100*error,"%")

        error1 = 100*error

        error2 = error3 = 0

        return error1, error2, error3

class Benchmark2:

    def __init__(self):
        #self.initial_normal_vel = -0.2

    #def set_initial_data(self, modelpart, iteration, number_of_points_in_the_graphic, coeff_of_restitution_iteration=0):

        #for node in modelpart.Nodes:
            #node.SetSolutionStepValue(VELOCITY_Z, self.initial_normal_vel)


    #def generate_graph_points(self, modelpart, time, output_time_step, dt):
        #pass

    #def get_final_data(self, modelpart):
        #pass

    #def ApplyNodalRotation(self, time, dt, modelpart):
        #pass

    def print_results(self, number_of_points_in_the_graphic, dt=0):

        normal_contact_force_outfile_name = 'variables_for_node_1.txt'
        gnuplot_script_name = 'benchmark2_dt_' + str(dt) + 's.gp'
        self.gnuplot_outfile = open(gnuplot_script_name, 'w')
        self.gnuplot_outfile.write("set grid; plot '" + normal_contact_force_outfile_name + "' every 10 u 1:10 w lp lt 3 lw 1.5 ps 1 pt 6")
        self.gnuplot_outfile.close()
        #print_gnuplot_files_on_screen(gnuplot_script_name)

        error1, error2, error3 = self.compute_errors(normal_contact_force_outfile_name)

        error_filename = 'errors.txt'
        error_file = open(error_filename, 'a')
        error_file.write("DEM Benchmark 2:")

        if (error1 < 1.0 and error2 < 1.0 and error3 < 1.0):
            error_file.write(" OK!........ Test 2 SUCCESSFUL\n")
        else:
            error_file.write(" KO!........ Test 2 FAILED\n")
        error_file.close()

    def compute_errors(self, normal_contact_force_outfile_name):

        Chung_data = []; DEM_data = []

        with open('paper_data/benchmark2_graph1.dat') as inf:
            for line in inf:
                Chung_data.append(float(line))

        with open(normal_contact_force_outfile_name) as inf:
            for line in inf:
                parts = line.split()
                if parts[0] == '#Time':
                    break
            for line in inf:
                parts = line.split()
                DEM_data.append(float(parts[9]))

        error = fabs(max(DEM_data) - float(Chung_data[0]))/float(Chung_data[0])

        print("Error in restitution numbers =", 100*error,"%")

        error1 = 100*error

        error2 = error3 = 0

        return error1, error2, error3



def delete_archives():

    #.......................Removing extra files
    files_to_delete_list = glob('*.time')
    #files_to_delete_list.extend(glob('*.dat'))
    files_to_delete_list.extend(glob('*.gp'))

    for to_erase_file in files_to_delete_list:
        os.remove(to_erase_file)

    #............Getting rid of unuseful folders
    folders_to_delete_list      = glob('*Data')
    folders_to_delete_list.extend(glob('*ists'))
    folders_to_delete_list.extend(glob('*ults'))
    folders_to_delete_list.extend(glob('*he__'))
    folders_to_delete_list.extend(glob('*aphs'))

    for to_erase_folder in folders_to_delete_list:
        shutil.rmtree(to_erase_folder)


def print_gnuplot_files_on_screen(gnuplot_script_name):
    system('gnuplot -persist ' + gnuplot_script_name)


def create_pdf_document(pdf_script_name):
    system('gnuplot -persist ' + pdf_script_name)
