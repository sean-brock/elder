# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library.

# Optionally glob, but only for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${elder_SOURCE_DIR}/include/engine/*.h")
# set(HEADER_LIST "${elder_SOURCE_DIR}/include/engine/game_state.h")

# Make an automatic library - will be static or dynamic based on user setting
add_library(engine_library game_state.cpp generational_index.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(engine_library PUBLIC ../include)

# This depends on (header only) boost
#target_link_libraries(engine_library PRIVATE Boost::boost)

# All users of this library will need at least C++11
target_compile_features(engine_library PUBLIC cxx_std_20)
if(MSVC)
  target_compile_options(engine_library PRIVATE /W4 /WX)
else()
  target_compile_options(engine_library PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
