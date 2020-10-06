include_directories(include)

aux_source_directory(src CLI_SRC)

add_executable(cli ${CLI_SRC})