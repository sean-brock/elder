add_executable(print print.cpp)
target_compile_features(print PRIVATE cxx_std_20)

target_link_libraries(print PRIVATE engine_library fmt::fmt)
