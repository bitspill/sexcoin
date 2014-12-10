cmake_minimum_required(VERSION 2.8.4)
project(Sexcoin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")


# depend on everything just so the editor recognizes the files as being used
# unfortunately CLion only supports cmake at this time
# do not compile using cmake!
file(GLOB_RECURSE ALL_SOURCE_FILES
    "src/*.h"
    "src/*.cpp"
)

add_executable(AppeaseTheEditors ${ALL_SOURCE_FILES})