project(GUI)
add_library(gui STATIC "ui.c" "ui.h" "styles.h")
target_link_libraries(gui PRIVATE m_utils raylib )
target_include_directories(gui PUBLIC .)
