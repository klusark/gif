project(gif)

SET(CMAKE_CXX_FLAGS "-std=c++11 -g -O3 -Wall")


add_executable(gif
    main.cc
    gif.cc
    io.cc)

target_link_libraries(gif png)
