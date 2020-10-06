include_directories(include)

aux_source_directory(src SRV_SRC)

add_executable(srv ${SRV_SRC})