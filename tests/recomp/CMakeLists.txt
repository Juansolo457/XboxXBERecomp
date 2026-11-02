# Each case: make_xbe.py writes its image and config, xbe_recomp its C++, clang builds that with the case's checks. One
# test runs the checks, one compares the C++ with the golden files.
set(RECOMP_CASE_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/make_xbe.py ${CMAKE_CURRENT_SOURCE_DIR}/cases.py)

execute_process(COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/make_xbe.py --list
    OUTPUT_VARIABLE RECOMP_CASES OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
string(REPLACE "\r" "" RECOMP_CASES "${RECOMP_CASES}")
string(REPLACE "\n" ";" RECOMP_CASES "${RECOMP_CASES}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${RECOMP_CASE_SCRIPTS})

foreach(name ${RECOMP_CASES})
    set(dir ${CMAKE_CURRENT_BINARY_DIR}/${name})
    set(gen ${dir}/generated)
    set(gen_sources ${gen}/x86_recomp.0.cpp ${gen}/x86_func_mapping.cpp)
    add_custom_command(
        OUTPUT ${dir}/stamp
        BYPRODUCTS ${gen_sources} ${gen}/x86_recomp_shared.h ${gen}/image_symbols.h ${dir}/case_symbols.h ${dir}/default.xbe
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/make_xbe.py ${name} ${dir}
        COMMAND xbe_recomp ${dir}/config/game.toml ${dir}/default.xbe ${gen}
        COMMAND ${CMAKE_COMMAND} -E touch ${dir}/stamp
        DEPENDS ${RECOMP_CASE_SCRIPTS} xbe_recomp
        COMMENT "Recompiling case ${name}"
        VERBATIM)
    add_custom_target(recompile_${name} DEPENDS ${dir}/stamp)
    set_source_files_properties(${gen_sources} PROPERTIES GENERATED TRUE COMPILE_OPTIONS "-w")

    add_executable(recomp_${name} cases/${name}.cpp guest.cpp ${gen_sources})
    add_dependencies(recomp_${name} recompile_${name})
    target_include_directories(recomp_${name} PRIVATE . ${dir} ${gen} ${XBE_SDK_DIR}/runtime)
    target_link_libraries(recomp_${name} PRIVATE doctest_header)
    target_compile_definitions(recomp_${name} PRIVATE XBE_PATH="${dir}/default.xbe")
    target_compile_options(recomp_${name} PRIVATE -fno-strict-aliasing -include ${CMAKE_CURRENT_SOURCE_DIR}/host_shim.h)
    if(WIN32)
        target_link_options(recomp_${name} PRIVATE -fuse-ld=lld)
    endif()
    add_test(NAME recomp_${name} COMMAND recomp_${name})
    add_test(NAME recomp_golden_${name}
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/check_golden.py ${gen} ${CMAKE_CURRENT_SOURCE_DIR}/golden/${name})
endforeach()
