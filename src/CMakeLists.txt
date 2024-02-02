set(SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/main.cc
)

add_library(game)
target_sources(game
  PUBLIC
  FILE_SET CXX_MODULES FILES
  p2.cc
  bullets.cc
  component.cc
  entity.cc
  system.cc
  player.cc
  event.cc
  net.cc
  types.cc
  common/vector_math.cc
  common/sparse_set.cc
  common/collision.cc
  common/ring_buffer.cc
  )

find_package(asio REQUIRED)
target_link_libraries(game raylib asio::asio)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
