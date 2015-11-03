cmake_minimum_required(VERSION 2.6)
project(ZipPicView CXX)
set(CMAKE_CXX_STANDARD 11)

set(SRC ZipPicView MainFrame.cpp ZipPicViewApp.cpp ImageViewPanel.cpp Entry.h ZipEntry.cpp ZipEntry.h FileEntry.cpp FileEntry.h)

if (WINDOWS OR MSYS)
    enable_language(RC)
    #CMake does not play well with GCC's windres.
    set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> -i <SOURCE>")
    set(SRC ${SRC} resource.rc)
endif (WINDOWS OR MSYS)

add_executable(${SRC} )

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZIP REQUIRED libzip)

include(${wxWidgets_USE_FILE})
include_directories(${LIBZIP_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${LIBZIP_LIBRARIES})
