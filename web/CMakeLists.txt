project(WEB)
add_library(web STATIC "web.c" "web.h")
target_link_libraries(web PRIVATE CURL::libcurl logger)
target_include_directories(web PUBLIC .)
