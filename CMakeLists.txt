
#--------------------------------------------------------------------------------
# Init
#--------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.12) # cmake version here
project(ecs-experimental VERSION 0.1 LANGUAGES CXX) # Your project name here

set(CMAKE_CXX_STANDARD 20) # C++ version here
set(DISABLE_CONSOLE 0) # Console usage

#set(CMAKE_CXX_STANDARD_REQUIRED ON) # Mayde you doesn't need std
#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) # Link Time Optimization

# Variables Zone
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

#--------------------------------------------------------------------------------
# Src & Include
#--------------------------------------------------------------------------------

# Set variables for short
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Recurse collect every file in include/ and src/ folders
file(GLOB_RECURSE INCLUDES CONFIGURE_DEPENDS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp")
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SOURCE_DIR}/*.c" "${SOURCE_DIR}/*.cpp")

# add include/ folder as root for all header files
include_directories("${INCLUDE_DIR}")

# finally add all founded include/ and src/ files into compiler as a program
add_executable("${CMAKE_PROJECT_NAME}" "${INCLUDES}" "${SOURCES}")

# single files zone
#add_executable("${CMAKE_PROJECT_NAME}" "src/main.cpp")

# single folders zone
#file(GLOB_RECURSE SOURCES CMAKE_CONFIGURE_DEPENDS "src/*.h" "src/*.c" "src/*.hpp" "src/*.cpp")
#add_executable("${CMAKE_PROJECT_NAME}" "${SOURCES}")

#--------------------------------------------------------------------------------
# Libraries
#--------------------------------------------------------------------------------

# Include folder in directory (etc. combine folders)
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/") # lib

# Add Local libraries
#add_subdirectory("lib/glm")

# Add Package libraries (vcpkg)
#find_package(fmt CONFIG REQUIRED)

# Link Local Libraries
#target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw)

# Link Package Libraries (vcpkg)
#target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE fmt::fmt)


#--------------------------------------------------------------------------------
# Utility
#--------------------------------------------------------------------------------

# Disable console
if (DISABLE_CONSOLE)
	if(MSVC) # If using the VS compiler...
		set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	endif()
	# For others idk how to solve this, search through the internet or something
endif()

# Scripts execution

if (WIN32)
	#execute_process(COMMAND "${CMAKE_PROJECT_NAME}/scripts/compile-shaders.bat")
	execute_process(COMMAND "${CMAKE_PROJECT_NAME}/scripts/copy-resources.bat")
endif (WIN32)

if (UNIX)
	#execute_process(COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile-shaders.sh")
	execute_process(COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/scripts/copy-resources.sh")
endif (UNIX)
