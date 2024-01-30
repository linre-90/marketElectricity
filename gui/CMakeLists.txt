project(GUI)
add_library(gui STATIC "ui.c" "ui.h" "styles.h")
target_link_libraries(gui PRIVATE viewmodel m_utils raylib_static web )
target_include_directories(gui PUBLIC .)
