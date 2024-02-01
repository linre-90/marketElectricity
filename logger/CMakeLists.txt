project(LOGGER)
add_library(logger STATIC "logger.c" "logger.h")
target_include_directories(logger PUBLIC .)
