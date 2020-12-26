cmake_minimum_required(VERSION 3.16)
project(wtf-pipes)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

add_subdirectory(the_framework)

set(SOURCES_COMMON
        AsyncWrapper.cpp
        AsyncWrapper.h
        BasePanel.cpp
        BasePanel.h
)

set(SOURCES_CLIENT
        ClientPanel.cpp
        ClientPanel.h
        ClientPipe.cpp
        ClientPipe.h
        main-client.cpp
)

set(SOURCES_SERVER
        main-server.cpp
        ServerPanel.cpp
        ServerPanel.h
        ServerPipe.cpp
        ServerPipe.h
)

add_executable(client-pipe ${SOURCES_CLIENT} ${SOURCES_COMMON})
target_link_libraries(client-pipe frmwrk)
add_executable(server-pipe ${SOURCES_SERVER} ${SOURCES_COMMON})
target_link_libraries(server-pipe frmwrk)
