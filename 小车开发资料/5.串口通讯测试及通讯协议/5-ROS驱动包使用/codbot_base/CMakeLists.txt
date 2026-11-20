cmake_minimum_required(VERSION 3.0.2)
project(codbot_base)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  nav_msgs
  roscpp
  rospy
  tf
)
find_package(Eigen3 REQUIRED)
catkin_package()

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

include_directories(${EIGEN3_INCLUDE_DIRS})

add_library(${PROJECT_NAME}
        src/codbot_base.cpp
        )

add_executable(${PROJECT_NAME}_node src/main.cpp )

 target_link_libraries(${PROJECT_NAME}_node
         ${PROJECT_NAME}
         ${catkin_LIBRARIES}
         )

