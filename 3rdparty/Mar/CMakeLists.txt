set(lib mar_loader)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options("-w") # legacy code with many warnings
endif()

file(GLOB files *.h *.hpp *.cpp)
add_library(${lib} ${files})

target_include_directories(${lib}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_HOME_DIRECTORY}/lib # for message callback
)

target_link_libraries(${lib} Qt5::Core)
