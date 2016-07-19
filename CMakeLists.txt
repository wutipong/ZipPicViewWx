cmake_minimum_required(VERSION 2.6)
project(ZipPicView CXX)
set(CMAKE_CXX_STANDARD 11)

set(SRC MainFrame.cpp ZipPicViewApp.cpp ImageViewPanel.cpp Entry.h ZipEntry.cpp ZipEntry.h
	FileEntry.cpp FileEntry.h ThumbnailLoadThread.cpp ThumbnailLoadThread.h)

if (MSYS)
    enable_language(RC)
    set(SRC ${SRC} resource.rc)
	INCLUDE(InstallRequiredSystemLibraries)
	get_filename_component( Mingw_Path ${CMAKE_CXX_COMPILER} PATH )

    set( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${Mingw_Path}/wxbase30u_gcc_custom.dll
		${Mingw_Path}/wxmsw30u_core_gcc_custom.dll ${Mingw_Path}/libjpeg-8.dll
		${Mingw_Path}/liblzma-5.dll ${Mingw_Path}/libpng16-16.dll
		${Mingw_Path}/libtiff-5.dll ${Mingw_Path}/libzip-4.dll ${Mingw_Path}/zlib1.dll
		${Mingw_Path}/libwinpthread-1.dll)

	if(CMAKE_SIZEOF_VOID_P EQUAL 8) #win64
		set ( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
			${Mingw_Path}/libgcc_s_seh-1.dll ${Mingw_Path}/libstdc++-6.dll)
	else() #win32
		set ( CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS}
			${Mingw_Path}/libgcc_s_dw2-1.dll ${Mingw_Path}/libstdc++-6.dll)
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
endif (MSYS)

add_executable(${PROJECT_NAME} ${SRC} )

set(wxWidgets_CONFIGURATION mswu)
find_package(wxWidgets COMPONENTS core base REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBZIP REQUIRED libzip)

include(${wxWidgets_USE_FILE})
include_directories(${LIBZIP_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} ${LIBZIP_LIBRARIES})

if (WINDOWS OR MSYS)
	install( PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION . )
endif (WINDOWS OR MSYS)

install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION . )

SET(CPACK_PACKAGE_VERSION_MAJOR "0" CACHE STRING "Major Version")
SET(CPACK_PACKAGE_VERSION_MINOR "0" CACHE STRING "Minor Version")
SET(CPACK_PACKAGE_VERSION_PATCH "0" CACHE STRING "Patch Version")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "In-ZIP file imager viewer")
SET(CPACK_PACKAGE_VENDOR "Wutipong Wongsakuldej aka mr_tawan")
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/res/appicon.ico")
set(CPACK_PACKAGE_EXECUTABLES ${PROJECT_NAME} ${PROJECT_NAME})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_WIX_UPGRADE_GUID "C36E8433-FF32-4CBA-B574-96060B209EA6")


include(CPack)
