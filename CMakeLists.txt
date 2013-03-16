project(gif)

SET(CMAKE_CXX_FLAGS "-std=c++11")

add_executable(gif
    main.cc
    gif.cc
    io.cc)
