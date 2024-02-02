# include(FetchContent)
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
#
# # For Windows: Prevent overriding the parent project's compiler/linker settings
# # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(googletest)

enable_testing()

find_package(gtest REQUIRED)
# target_link_libraries(game raylib asio::asio)

add_executable(ringbuffer_test ringbuffer_test.cc)
add_executable(collision_test collision_test.cc)
add_executable(sparseset_test sparse_set.cc)

target_link_libraries(ringbuffer_test GTest::gtest_main game)
target_link_libraries(sparseset_test GTest::gtest_main game)
target_link_libraries(collision_test GTest::gtest_main raylib game)

include(GoogleTest)
