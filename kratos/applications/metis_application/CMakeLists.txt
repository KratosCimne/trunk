set(CMAKE_INCLUDE_CURRENT_DIR ON)

message("**** configuring KratosMetisApplication ****")
 
include_directories( ${CMAKE_SOURCE_DIR}/kratos )

#find mkl solver package -- this is our own find package!!
if(${USE_METIS_5} MATCHES ON)
    message("using metis (and not Parmetis) version 5")
    add_definitions(-DKRATOS_USE_METIS_5)
    find_package(METIS REQUIRED)
    include_directories(${METIS_INCLUDE_DIR})
    link_directories(${METIS_LIBRARIES})
    install(FILES ${METIS_LIBRARIES} DESTINATION libs)
else(${USE_METIS_5} MATCHES ON)
    message("using PARMETIS < 4 as default. Set USE_METIS_5=ON to compile with METIS v5 instead- note that current compile option is deprecated")
    find_package(PARMETIS REQUIRED)
    include_directories(${PARMETIS_INCLUDE_DIR})
    link_directories(${PARMETIS_LIBRARIES})
endif(${USE_METIS_5} MATCHES ON)

##add definitions needed
add_definitions(-DMKL_ILP64)

## generate variables with the sources
set( KRATOS_METIS_APPLICATION_SOURCES    
	${CMAKE_CURRENT_SOURCE_DIR}/metis_application.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/add_processes_to_python.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/custom_python/kratos_metis_python_application.cpp 
)


# ###############################################################
add_library(KratosMetisApplication SHARED ${KRATOS_METIS_APPLICATION_SOURCES})
target_link_libraries(KratosMetisApplication KratosCore ${METIS_LIBRARY} ${MPI_LIBRARIES} )
set_target_properties(KratosMetisApplication PROPERTIES PREFIX "")

# changing the .dll suffix to .pyd 
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set_target_properties(KratosMetisApplication PROPERTIES SUFFIX .pyd)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# changing the .dylib suffix to .so 
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set_target_properties(KratosMetisApplication PROPERTIES SUFFIX .so)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

install(TARGETS KratosMetisApplication DESTINATION libs )

if(${INSTALL_PYTHON_FILES} MATCHES ON)
  get_filename_component (CURRENT_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/python_scripts DESTINATION applications/${CURRENT_DIR_NAME}  FILES_MATCHING PATTERN "*.py"  PATTERN ".svn" EXCLUDE)
endif(${INSTALL_PYTHON_FILES} MATCHES ON)

# Add to the KratosMultiphisics Python module
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/MetisApplication.py" DESTINATION KratosMultiphysics )
