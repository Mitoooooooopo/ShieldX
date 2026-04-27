cmake_minimum_required(VERSION 3.10)
project(ShieldCompat)

set(CMAKE_CXX_STANDARD 17)

find_library(SDL2 SDL2)
find_library(log-lib log)
find_library(android-lib android)
find_library(dl-lib dl)

add_library(tsec_wrapper SHARED
    src/compat/tsec_wrapper.cpp
    src/compat/nvsi_stub.cpp
    src/compat/property_spoof.cpp
    src/compat/gles_nv_stubs.cpp  
)

target_link_libraries(tsec_wrapper
    ${log-lib}
    ${dl-lib} 
    ${android-lib} 
    ${egl-lib} 
    
)

