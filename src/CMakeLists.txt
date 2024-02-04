set(SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/main.cc
)

add_library(game)
target_sources(game
  PUBLIC
  FILE_SET CXX_MODULES FILES
  common/vector_math.cc
  common/sparse_set.cc
  common/collision.cc
  common/ring_buffer.cc
  common/types.cc
  ecs/component.cc
  ecs/entity.cc
  ecs/system.cc
  ecs/ecs.cc
  p2.cc
  bullets.cc
  player.cc
  event.cc
  net.cc
  )

find_package(asio REQUIRED)
target_link_libraries(game raylib asio::asio)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
