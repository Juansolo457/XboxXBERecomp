# The runtime's portable parts, compiled as a game links them with the window, GPU and audio left out, on both platforms.
set(RT ${XBE_SDK_DIR}/runtime)

# image_symbols.h for sources that bind names, every name absent
set(SYMBOLS_DIR ${CMAKE_CURRENT_BINARY_DIR}/symbols)
add_custom_command(
    OUTPUT ${SYMBOLS_DIR}/image_symbols.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SYMBOLS_DIR}
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gen_symbols.py ${SYMBOLS_DIR}/image_symbols.h ${RT}/xdk/dsound.cpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen_symbols.py ${RT}/xdk/dsound.cpp
    VERBATIM)
add_custom_target(runtime_test_symbols DEPENDS ${SYMBOLS_DIR}/image_symbols.h)

add_library(runtime_under_test OBJECT
    ${RT}/core/log.cpp
    ${RT}/cpu/dispatch.cpp
    ${RT}/gpu/shaders.cpp
    ${RT}/gpu/vertex.cpp
    ${RT}/kernel/disc.cpp
    ${RT}/kernel/files.cpp
    ${RT}/kernel/memory.cpp
    ${RT}/kernel/objects.cpp
    ${RT}/kernel/ordinals.cpp
    ${RT}/kernel/threads.cpp
    ${RT}/xdk/dsp_image.cpp)
target_include_directories(runtime_under_test PUBLIC ${RT})
if(NOT WIN32)
    target_include_directories(runtime_under_test PUBLIC ${XBE_SDK_DIR}/harness/compat)  # as harness/build.sh
endif()
target_compile_options(runtime_under_test PUBLIC -fno-strict-aliasing)
target_compile_definitions(runtime_under_test PUBLIC XBE_RAW_VERTICES=0 UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)

add_executable(runtime_tests support.cpp test_objects.cpp test_memory.cpp test_files.cpp test_dsound.cpp test_vertex.cpp)
add_dependencies(runtime_tests runtime_test_symbols)
target_link_libraries(runtime_tests PRIVATE runtime_under_test doctest_header)
set_source_files_properties(test_dsound.cpp PROPERTIES INCLUDE_DIRECTORIES ${SYMBOLS_DIR})
if(WIN32)
    target_link_libraries(runtime_tests PRIVATE ws2_32)
    target_link_options(runtime_tests PRIVATE -fuse-ld=lld)
else()
    target_link_libraries(runtime_tests PRIVATE pthread)
endif()
add_test(NAME runtime_tests COMMAND runtime_tests)

# the shader translator: a fixed set of inputs through the source alone
add_executable(shader_corpus shader_corpus.cpp ${RT}/gpu/shaders.cpp)
target_include_directories(shader_corpus PRIVATE ${RT})
if(WIN32)
    target_link_options(shader_corpus PRIVATE -fuse-ld=lld)
endif()
set(SHADER_OUT ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_test(NAME shader_corpus_write COMMAND shader_corpus ${SHADER_OUT})
add_test(NAME shader_corpus_golden
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/check_shaders.py golden ${SHADER_OUT} ${CMAKE_CURRENT_SOURCE_DIR}/golden)
add_test(NAME shader_corpus_compile
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/check_shaders.py compile ${SHADER_OUT})
set_tests_properties(shader_corpus_write PROPERTIES FIXTURES_SETUP shader_corpus)
set_tests_properties(shader_corpus_golden shader_corpus_compile PROPERTIES FIXTURES_REQUIRED shader_corpus)
set_tests_properties(shader_corpus_compile PROPERTIES SKIP_RETURN_CODE 77)
