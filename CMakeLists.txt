cmake_minimum_required(VERSION 3.21)

project(LevelPackUI VERSION 1.0.0)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(LevelPackUI STATIC ${SOURCES})

target_include_directories(LevelPackUI PUBLIC include)
target_link_libraries(LevelPackUI PUBLIC geode-sdk)
set_target_properties(LevelPackUI PROPERTIES POSITION_INDEPENDENT_CODE ON)

get_target_property(GEODE_INTERFACE_SOURCES geode-sdk INTERFACE_SOURCES)
if (GEODE_INTERFACE_SOURCES)
    set_source_files_properties(${GEODE_INTERFACE_SOURCES} PROPERTIES HEADER_FILE_ONLY ON)
endif()

function(lpui_check_public_headers target include_dir)
    get_filename_component(include_dir "${include_dir}" ABSOLUTE)
    file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${include_dir}/*.hpp" "${include_dir}/*.h")

    set(check_target ${target}HeaderCheck)
    set(check_dir "${CMAKE_CURRENT_BINARY_DIR}/header-check")
    set(sources "")
    foreach(header IN LISTS headers)
        file(RELATIVE_PATH relative "${include_dir}" "${header}")
        string(MAKE_C_IDENTIFIER "${relative}" name)
        set(source "${check_dir}/${name}.cpp")
        file(CONFIGURE OUTPUT "${source}" CONTENT "#include <${relative}>\n")
        list(APPEND sources "${source}")
    endforeach()

    add_library(${check_target} OBJECT ${sources})
    target_include_directories(${check_target} PRIVATE "${include_dir}")

    get_target_property(include_dirs ${target} INCLUDE_DIRECTORIES)
    if (include_dirs)
        target_include_directories(${check_target} PRIVATE ${include_dirs})
    endif()

    get_target_property(definitions ${target} COMPILE_DEFINITIONS)
    if (definitions)
        target_compile_definitions(${check_target} PRIVATE ${definitions})
    endif()

    get_target_property(libs ${target} LINK_LIBRARIES)
    if (libs)
        list(REMOVE_ITEM libs LevelPackUI)
        target_link_libraries(${check_target} PRIVATE ${libs})
    endif()

    add_dependencies(${target} ${check_target})
endfunction()

lpui_check_public_headers(LevelPackUI include)
