cmake_minimum_required(VERSION 3.30)
project(game_engine C)

set(CMAKE_C_STANDARD 23)

include(FetchContent)

function(include_dependency libName gitURL gitTag)
    # setup the declare
    FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG        ${gitTag}
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
    )

    FetchContent_MakeAvailable(${libName})
endfunction()

find_package(SDL3 QUIET)
if (NOT SDL3_FOUND)
    message(STATUS "Getting SDL3 from Github")
    include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.2.6)
else()
    message(STATUS "Using local SDL3")
endif()

add_executable(${PROJECT_NAME} main.c
        engine/array_list.h
        engine/types.h
        engine/util.h
        engine/array_list/array_list.c
        engine/io.h
        engine/io/io.c
        engine/global.h
        engine/global.c
        engine/render.h
        engine/render/render.c
        engine/render/render_internal.h
        engine/render/render_init.c
        engine/render/render_util.c
        engine/time.h
        engine/time/time.c
        engine/config.h
        engine/input.h
        engine/config/config.c
        engine/input/input.c
        engine/physics.h
        engine/physics/physics.c
        engine/physics/physics_internal.h
        engine/entity/entity.c
        engine/entity.h
        engine/animation.h
        engine/animation/animation.c
)

add_subdirectory(dependencies/glad)

target_include_directories(${PROJECT_NAME}
        PUBLIC ${SDL3_INCLUDE_DIRS} dependencies
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE SDL3::SDL3 glad
)

target_compile_definitions(${PROJECT_NAME}
        PRIVATE SDL_MAIN_USE_CALLBACKS
)

add_custom_target(
        CopyAssets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
)

add_dependencies(${PROJECT_NAME} CopyAssets)

if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:game_engine>"
            VERBATIM
    )
endif()
