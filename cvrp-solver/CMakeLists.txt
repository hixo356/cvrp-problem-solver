
add_executable(cvrp-solver)

target_include_directories(cvrp-solver PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(cvrp-solver
 PRIVATE
  main.cpp)

