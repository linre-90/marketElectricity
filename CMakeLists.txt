cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("marketElectricity")

# Add source to this project's executable.
add_executable (marketElectricity "main.c" "ui.h" "ui.c" "styles.h" "m_utils.h" "m_utils.c")

add_subdirectory(external)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET marketElectricity PROPERTY CXX_STANDARD 20)
  set_property(TARGET marketElectricity PROPERTY C_STANDARD 17)
endif()

# Link stuff
#target_link_libraries(marketElectricity PRIVATE cee_events raylib)
target_link_libraries(marketElectricity PRIVATE raylib)