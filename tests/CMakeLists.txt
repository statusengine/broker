Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

FILE(GLOB tests_sources *.cpp)

enable_testing(true)

add_executable(tests ${tests_sources})
target_include_directories(tests PRIVATE ../src ../src/MessageHandler ${STATUSENGINE_INCLUDES})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain ${STATUSENGINE_LIBS})
target_compile_options(tests PRIVATE -fsanitize=address)
target_link_options(tests PRIVATE -fsanitize=address)

include(Catch)
catch_discover_tests(tests TEST_PREFIX tests/)
