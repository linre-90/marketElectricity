project(WEB)
add_library(web STATIC "web.c" "web.h")
target_link_libraries(gui PRIVATE CURL::libcurl )
target_include_directories(gui PUBLIC .)
