
FILE(GLOB tests_minimal_sources *.cpp)

enable_testing(true)

add_executable(tests-minimal ${tests_minimal_sources})
target_include_directories(tests-minimal PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/MessageHandler ${STATUSENGINE_INCLUDES})
target_link_libraries(tests-minimal PRIVATE GTest::gmock_main statusengine naemon ${STATUSENGINE_LIBS})
target_compile_options(tests-minimal PRIVATE -fsanitize=address)
target_link_options(tests-minimal PRIVATE -fsanitize=address)

include(GoogleTest)
gtest_discover_tests(tests-minimal)
