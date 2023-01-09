
FILE(GLOB tests_sources *_Test.cpp)
FILE(GLOB benchmarks_sources *_Benchmark.cpp)

enable_testing(true)

add_executable(tests ${tests_sources})
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/MessageHandler ${STATUSENGINE_INCLUDES})
target_link_libraries(tests PRIVATE GTest::gmock_main statusengine naemon ${STATUSENGINE_LIBS})
target_compile_options(tests PRIVATE -fsanitize=address)
target_link_options(tests PRIVATE -fsanitize=address)

add_executable(benchmarks ${benchmarks_sources})
target_include_directories(benchmarks PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/MessageHandler ${STATUSENGINE_INCLUDES})
target_link_libraries(benchmarks PRIVATE GTest::gmock_main statusengine naemon ${STATUSENGINE_LIBS})
target_compile_options(benchmarks PRIVATE -O3)

add_executable(benchmarks-debug ${benchmarks_sources})
target_include_directories(benchmarks-debug PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/src/MessageHandler ${STATUSENGINE_INCLUDES})
target_link_libraries(benchmarks-debug PRIVATE GTest::gmock_main statusengine naemon ${STATUSENGINE_LIBS})


include(GoogleTest)
gtest_discover_tests(tests)
gtest_discover_tests(benchmarks)
