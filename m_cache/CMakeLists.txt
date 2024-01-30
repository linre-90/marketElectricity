project(M_CACHE)
add_library(m_cache STATIC "m_cache.c" "m_cache.h")
target_link_libraries(m_cache PRIVATE viewmodel web)
target_include_directories(m_cache PUBLIC .)
