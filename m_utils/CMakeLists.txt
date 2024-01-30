project(My_utils)
add_library(m_utils STATIC "m_utils.c" "m_utils.h")
target_include_directories(m_utils PUBLIC .)
