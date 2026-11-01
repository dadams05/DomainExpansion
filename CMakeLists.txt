cmake_minimum_required(VERSION 3.21)
project(DomainExpansion LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)




if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif()

# By enabling function-section splitting and linker garbage collection,
# any function inside SDL3 or ImGui that your specific app never calls 
# is completely thrown away at link time
add_compile_options(-ffunction-sections -fdata-sections)
add_link_options(-Wl,--gc-sections)

# allows the compiler to optimize across all files (your code + ImGui + SDL3) 
# simultaneously during the final link phase, allowing it to discard dead code 
# and inline aggressively across library boundaries
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)



################################################
#
# Download dependencies
#
################################################

include(FetchContent)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

# set(SDL_GPU OFF CACHE BOOL "" FORCE)
# set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
# set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
# set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
# set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
# set(SDL_POWER OFF CACHE BOOL "" FORCE)
# set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
# set(SDL_DIALOG OFF CACHE BOOL "" FORCE)

# set(SDL_DUMMYCAMERA OFF CACHE BOOL "" FORCE)
# set(SDL_OFFSCREEN OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL.git"
    GIT_TAG "release-3.4.18"
)

FetchContent_Declare(
    ImGui
    GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
    GIT_TAG "v1.93.0"
)

FetchContent_MakeAvailable(SDL3)
FetchContent_MakeAvailable(ImGui)

################################################
#
# Build a dynamic ImGui library
#
################################################

add_library(ImGui 
    STATIC
        "${imgui_SOURCE_DIR}/imgui_demo.cpp"
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
)
target_include_directories(ImGui
    PUBLIC
        "${imgui_SOURCE_DIR}"
        "${imgui_SOURCE_DIR}/backends"
)
target_link_libraries(ImGui
    PUBLIC
        SDL3::SDL3
)

################################################
#
# Set up the main executable
#
################################################

add_executable(DomainExpansion
    "src/main.cpp"
)

target_include_directories(DomainExpansion
    PRIVATE
        "include"
)

target_link_libraries(DomainExpansion
    PRIVATE
        ImGui
)

################################################
#
# If building on Windows
#
################################################

# if (WIN32)
#     # include rc file to set exe icon
#     target_sources(c8ke PRIVATE "c8ke.rc")

#     # console in debug, GUI in release
#     set_target_properties(c8ke PROPERTIES
#         WIN32_EXECUTABLE $<$<CONFIG:Release>:ON>
#     )

#     # post-build commands
#     add_custom_command(TARGET c8ke POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         "$<TARGET_RUNTIME_DLLS:c8ke>"
#         "$<TARGET_FILE_DIR:c8ke>"
#         COMMAND_EXPAND_LISTS
#     )
# endif()
