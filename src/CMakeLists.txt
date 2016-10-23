# 增加PCL库的依赖
FIND_PACKAGE( PCL REQUIRED COMPONENTS common io visualization filters)
# 添加头文件和库文件
ADD_DEFINITIONS( ${PCL_DEFINITIONS} )
INCLUDE_DIRECTORIES( ${PCL_INCLUDE_DIRS}  )
LINK_LIBRARIES( ${PCL_LIBRARY_DIRS} )


# 增加一个可执行的二进制
add_executable (interactive_icp interactive_icp.cpp)
target_link_libraries (interactive_icp
              ${PCL_LIBRARIES})

add_executable (icp iterative_closest_point.cpp)
target_link_libraries (icp
              ${PCL_LIBRARIES})



