include_directories(include)

aux_source_directory(src SRV_SRC)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_executable(srv ${SRV_SRC})

target_link_libraries(srv PRIVATE Threads::Threads)