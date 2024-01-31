cmake_minimum_required (VERSION 3.8)

project ("marketElectricity")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Add source to this project's executable.
add_executable (marketElectricity "main.c" "app.rc")

# external directory contains fetchcontent dependencies
add_subdirectory(external)
add_subdirectory(m_utils)
add_subdirectory(gui)
add_subdirectory(web)
add_subdirectory(m_cache)
add_subdirectory(viewmodel)

# Copu stuff from src to bin folder this could be better...
file(COPY gui/resources DESTINATION ${CMAKE_BINARY_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET marketElectricity PROPERTY CXX_STANDARD 20)
  set_property(TARGET marketElectricity PROPERTY C_STANDARD 17)
endif()

# Link libraries
target_link_libraries(marketElectricity PRIVATE viewmodel m_utils gui web m_cache)
