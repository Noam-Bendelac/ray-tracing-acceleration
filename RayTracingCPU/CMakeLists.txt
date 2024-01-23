# CMakeList.txt : CMake project for RayTracingCPU, include source and define
# project specific logic here.
#






# Add source to this project's executable.
add_executable (RayTracingCPU "RayTracingCPU.cpp" "Renderer.cpp" "Scene.cpp" "Material.cpp" "Entity.cpp")


find_package(fmt CONFIG REQUIRED)
target_link_libraries(RayTracingCPU PRIVATE fmt::fmt)

find_package(unofficial-graphicsmagick CONFIG REQUIRED)
target_link_libraries(RayTracingCPU PRIVATE unofficial::graphicsmagick::graphicsmagick)

find_package(glm CONFIG REQUIRED)
target_link_libraries(RayTracingCPU PRIVATE glm::glm)

# find_package(glm CONFIG REQUIRED)
# target_link_libraries(main PRIVATE glm::glm)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RayTracingCPU PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.
