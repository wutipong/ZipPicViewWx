cmake_minimum_required (VERSION 2.6)
project (ZipPicView CXX)
set(CMAKE_CXX_STANDARD 11)

add_executable(ZipPicView MainFrame.cpp ZipPicViewApp.cpp)

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
