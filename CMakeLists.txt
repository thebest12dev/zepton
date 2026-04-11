# CMakeList.txt : CMake project for zepton0, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("zepton")
find_package(Tesseract CONFIG REQUIRED)
find_package(OpenCV REQUIRED)
# Add soun rce to this project's executable.
file(GLOB_RECURSE ZEPTON_SOURCES "src/*.cpp" "src/*.h")
add_executable(zepton ${ZEPTON_SOURCES})
target_link_libraries(zepton opencv_imgproc opencv_core Tesseract::libtesseract)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET zepton PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
