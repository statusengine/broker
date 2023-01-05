
FILE(GLOB tests_sources *.cpp)

enable_testing(true)

add_executable(tests ${tests_sources})
target_include_directories(tests PRIVATE ../src ../src/MessageHandler ${STATUSENGINE_INCLUDES})
target_link_libraries(tests PRIVATE GTest::gmock_main ${STATUSENGINE_LIBS})
target_compile_options(tests PRIVATE -fsanitize=address)
target_link_options(tests PRIVATE -fsanitize=address)

include(GoogleTest)
gtest_discover_tests(tests)
