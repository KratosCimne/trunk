set(CMAKE_INCLUDE_CURRENT_DIR ON)

message("**** configuring KratosTrilinosApplication ****")
 
include_directories( ${CMAKE_SOURCE_DIR}/kratos )
include_directories( ${CMAKE_SOURCE_DIR}/applications/ )

#dependecies on other application -- TRILINOS specific
include_directories( ${CMAKE_SOURCE_DIR}/applications/incompressible_fluid_application )
include_directories( ${CMAKE_SOURCE_DIR}/applications/FluidDynamicsApplication )
include_directories( ${CMAKE_SOURCE_DIR}/applications/ALEapplication )
include_directories( ${CMAKE_SOURCE_DIR}/applications/AdjointFluidApplication )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/custom_external_libraries )
 

#find trilinos solver package -- this is our own find package!!
find_package(TRILINOS REQUIRED)
include_directories(${TRILINOS_INCLUDE_DIR})
link_directories(${TRILINOS_LIBRARIES})

if(TRILINOS_USE_AMGCL MATCHES ON)
    add_definitions(-DAMGCL_INCLUDED)
    add_definitions(-DAMGCL_PROFILING)
#     add_definitions(-lrt)

    find_package(Pastix QUIET)
     
#     find_package(BLAS   QUIET)

#     if (PASTIX_INCLUDES AND SCOTCH_INCLUDES AND BLAS_FOUND)
    if (PASTIX_INCLUDES)
        set(HAVE_PASTIX ON)
        find_package(Scotch REQUIRED)
        #include_directories(${PASTIX_INCLUDES} ${SCOTCH_INCLUDES})
        include_directories(${PASTIX_INCLUDES})
        add_definitions(-DAMGCL_HAVE_PASTIX)
    endif()

    ##including the amgcl, which is header only
    set(AMGCL_ROOT $ENV{AMGCL_ROOT} CACHE STRING "AMGCL root")
    message(STATUS "Found AMGCL at ${AMGCL_ROOT}")
    include_directories(${AMGCL_ROOT})
    
    ##including extra boost libraries 
    
endif(TRILINOS_USE_AMGCL MATCHES ON)


##add definitions needed

## generate variables with the sources
set( KRATOS_TRILINOS_APPLICATION_SOURCES    
	${CMAKE_CURRENT_SOURCE_DIR}/trilinos_application.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/trilinos_python_application.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_trilinos_space_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_trilinos_convergence_criterias_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_trilinos_schemes_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_trilinos_linear_solvers_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_trilinos_processes_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_trilinos_strategies_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_custom_io_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_custom_utilities_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_trilinos_communicator_to_python.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_first.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_zoltan_processes_to_python.cpp
        ${CMAKE_SOURCE_DIR}/applications/ALEapplication/custom_elements/laplacian_meshmoving_element.cpp
        ${CMAKE_SOURCE_DIR}/applications/ALEapplication/custom_elements/structural_meshmoving_element.cpp
        ${CMAKE_SOURCE_DIR}/applications/ALEapplication/custom_elements/structural_meshmoving_element_nonlinear.cpp
        ${CMAKE_SOURCE_DIR}/applications/AdjointFluidApplication/custom_elements/vms_adjoint_element.cpp
        ${CMAKE_SOURCE_DIR}/applications/FluidDynamicsApplication/fluid_dynamics_application_variables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/custom_utilities/mpi_normal_calculation_utilities.cpp;
)

# add_executable(testmap ${CMAKE_CURRENT_SOURCE_DIR}/testmap.cpp )
# target_link_libraries(testmap ${TRILINOS_LIBRARIES} ${MPI_LIBRARIES} ${BLAS_LIBRARIES})

# ###############################################################
add_library(KratosTrilinosApplication SHARED ${KRATOS_TRILINOS_APPLICATION_SOURCES})

if (HAVE_PASTIX)
        target_link_libraries(KratosTrilinosApplication KratosCore ${TRILINOS_LIBRARIES} ${MPI_LIBRARIES} ${PASTIX_LIBRARIES} ${SCOTCH_LIBRARIES} ${BLAS_LIBRARIES} )
        install(FILES ${PASTIX_LIBRARIES} DESTINATION libs )

        message("linking PASTIX in the AMGCL lib")
#         target_link_libraries(KratosTrilinosApplication KratosCore ${TRILINOS_LIBRARIES} ${MPI_LIBRARIES} ${PASTIX_LIBRARIES} ${SCOTCH_LIBRARIES} ${BLAS_LIBRARIES})
else()
        target_link_libraries(KratosTrilinosApplication KratosCore ${TRILINOS_LIBRARIES} ${MPI_LIBRARIES} )
endif()

set_target_properties(KratosTrilinosApplication PROPERTIES PREFIX "")

# changing the .dll suffix to .pyd 
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set_target_properties(KratosTrilinosApplication PROPERTIES SUFFIX .pyd)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# changing the .dylib suffix to .so 
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set_target_properties(KratosTrilinosApplication PROPERTIES SUFFIX .so)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

install(TARGETS KratosTrilinosApplication DESTINATION libs )

if(${INSTALL_PYTHON_FILES} MATCHES ON)
  get_filename_component (CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python_scripts DESTINATION applications/${CURRENT_DIR_NAME}  FILES_MATCHING PATTERN "*.py"  PATTERN ".svn" EXCLUDE)

  message("trilinos libs to be installed" ${TRILINOS_LIBRARIES})
  install(FILES ${TRILINOS_LIBRARIES} DESTINATION libs )
endif(${INSTALL_PYTHON_FILES} MATCHES ON)

# Add to the KratosMultiphisics Python module
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/TrilinosApplication.py" DESTINATION KratosMultiphysics )
