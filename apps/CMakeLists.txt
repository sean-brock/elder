add_executable(elder elder.cpp)
target_compile_features(elder PRIVATE cxx_std_20)

target_link_libraries(elder PRIVATE engine_library fmt::fmt)
