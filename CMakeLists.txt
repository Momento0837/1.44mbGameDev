cmake_minimum_required(VERSION 3.20)

project(OnePointFourFourMBGame LANGUAGES CXX)

add_executable(OnePointFourFourMBGame WIN32
    src/main.cpp
)

target_compile_features(OnePointFourFourMBGame PRIVATE cxx_std_17)
target_compile_definitions(OnePointFourFourMBGame PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

target_link_libraries(OnePointFourFourMBGame PRIVATE Msimg32 Gdiplus)

add_custom_command(TARGET OnePointFourFourMBGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:OnePointFourFourMBGame>/assets"
)

if(MSVC)
    target_compile_options(OnePointFourFourMBGame PRIVATE
        /W4
        /permissive-
        /utf-8
    )
endif()
