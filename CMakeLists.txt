project(KDEBASE_APPS)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

#search packages used by KDE
set(KDE_MIN_VERSION "4.3.60")  # for the < 4.2 macro
find_package(KDE4 4.3.60 REQUIRED)
find_package(Strigi REQUIRED)
find_package(ZLIB REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

include(ConfigureChecks.cmake)
configure_file (config-apps.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-apps.h )
include_directories (${CMAKE_CURRENT_BINARY_DIR})

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/lib/konq ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

add_subdirectory( lib )
add_subdirectory( dolphin )
add_subdirectory( kwrite )
add_subdirectory( kdialog )
add_subdirectory( keditbookmarks )
add_subdirectory( konqueror )
add_subdirectory( kfind )
add_subdirectory( kappfinder )
add_subdirectory( plasma )

if ( Q_WS_MAC )
  add_subdirectory( konsole )
  add_subdirectory( kdepasswd )
endif ( Q_WS_MAC )

if ( Q_WS_X11 )
  add_subdirectory( kinfocenter )
  add_subdirectory( nsplugins )
  add_subdirectory( konsole )
  add_subdirectory( kdepasswd )
endif ( Q_WS_X11 )
macro_optional_add_subdirectory( doc )

if(CMAKE_SOURCE_DIR STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")
    macro_display_feature_log()
endif(CMAKE_SOURCE_DIR STREQUAL "${CMAKE_CURRENT_SOURCE_DIR}")