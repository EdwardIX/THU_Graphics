CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(PA1 CXX)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

ADD_SUBDIRECTORY(deps/vecmath)

SET(PA1_SOURCES
        src/bvhtree.cpp
        src/hit.cpp
        src/image.cpp
        src/kdtree.cpp
        src/main.cpp
        src/mesh.cpp
        src/ppm.cpp
        src/scene_parser.cpp)

SET(PA1_INCLUDES
        include/bvhtree.hpp
        include/camera.hpp
        include/group.hpp
        include/hit.hpp
        include/image.hpp
        include/kdtree.hpp
        include/light.hpp
        include/material.hpp
        include/mesh.hpp
        include/object3d.hpp
        include/plane.hpp
        include/ppm.hpp
        include/ray.hpp
        include/scene_parser.hpp
        include/sphere.hpp
        include/transform.hpp
        include/triangle.hpp
        include/utils.hpp
        )

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -fopenmp")

ADD_EXECUTABLE(${PROJECT_NAME} ${PA1_SOURCES} ${PA1_INCLUDES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} vecmath)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE include)
