cmake_minimum_required(VERSION 3.10)

project(OpenGLtest)

add_subdirectory(glfw-3.4)

set(ecs 
ECS/component.h
ECS/componentManager.h
ECS/componentManager.cpp
ECS/entity.h
ECS/entityManager.h
ECS/entityManager.cpp
ECS/IComponent.h
ECS/system.cpp
ECS/system.h
)

add_executable(BaseECS ${ecs})

target_include_directories(BaseECS PRIVATE ${CMAKE_SOURCE_DIR}/ECS )

add_executable(myApp main.cpp include/glad.c render.h ${ecs})
 
target_include_directories(myApp PRIVATE ${CMAKE_SOURCE_DIR} )

target_include_directories(myApp PRIVATE eigen-5.0.0)

target_include_directories(myApp PRIVATE glfw-3.4/include)

target_include_directories(myApp PRIVATE include)

target_include_directories(myApp PRIVATE glm-1.0.2)

target_link_libraries(myApp  glfw
				
)