cmake_minimum_required (VERSION 2.6)
project (ZipPicView CXX)
set(CMAKE_CXX_STANDARD 11)

add_executable(ZipPicView MainFrame.cpp ZipPicViewApp.cpp ImageViewPanel.cpp)

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules (LIBZIP REQUIRED libzip)

include(${wxWidgets_USE_FILE} )
include_directories(${LIBZIP_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${LIBZIP_LIBRARIES})
