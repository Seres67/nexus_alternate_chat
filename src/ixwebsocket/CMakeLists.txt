project(ixwebsocket)

file(GLOB src ./**.cpp)

add_library(ixwebsocket STATIC ${src})
set_property(TARGET ixwebsocket PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")