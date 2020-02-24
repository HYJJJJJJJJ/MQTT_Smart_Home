
if (CMAKE_VERSION VERSION_LESS 3.1.0)
    message(FATAL_ERROR "Qt 5 Qmqtt module requires at least CMake version 3.1.0")
endif()

get_filename_component(_qt5Qmqtt_install_prefix "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

# For backwards compatibility only. Use Qt5Qmqtt_VERSION instead.
set(Qt5Qmqtt_VERSION_STRING 1.0.0)

set(Qt5Qmqtt_LIBRARIES Qt5::Qmqtt)

macro(_qt5_Qmqtt_check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "The imported target \"Qt5::Qmqtt\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
endmacro()


macro(_populate_Qmqtt_target_properties Configuration LIB_LOCATION IMPLIB_LOCATION)
    set_property(TARGET Qt5::Qmqtt APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt5Qmqtt_install_prefix}/bin/${LIB_LOCATION}")
    _qt5_Qmqtt_check_file_exists(${imported_location})
    set(_deps
        ${_Qt5Qmqtt_LIB_DEPENDENCIES}
    )
    set_target_properties(Qt5::Qmqtt PROPERTIES
        "INTERFACE_LINK_LIBRARIES" "${_deps}"
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        # For backward compatibility with CMake < 2.8.12
        "IMPORTED_LINK_INTERFACE_LIBRARIES_${Configuration}" "${_deps}"
    )

    set(imported_implib "${_qt5Qmqtt_install_prefix}/lib/${IMPLIB_LOCATION}")
    _qt5_Qmqtt_check_file_exists(${imported_implib})
    if(NOT "${IMPLIB_LOCATION}" STREQUAL "")
        set_target_properties(Qt5::Qmqtt PROPERTIES
        "IMPORTED_IMPLIB_${Configuration}" ${imported_implib}
        )
    endif()
endmacro()

if (NOT TARGET Qt5::Qmqtt)

    set(_Qt5Qmqtt_OWN_INCLUDE_DIRS "${_qt5Qmqtt_install_prefix}/include/" "${_qt5Qmqtt_install_prefix}/include/QtQmqtt")
    set(Qt5Qmqtt_PRIVATE_INCLUDE_DIRS "")
    include("${CMAKE_CURRENT_LIST_DIR}/ExtraSourceIncludes.cmake" OPTIONAL)

    foreach(_dir ${_Qt5Qmqtt_OWN_INCLUDE_DIRS})
        _qt5_Qmqtt_check_file_exists(${_dir})
    endforeach()

    # Only check existence of private includes if the Private component is
    # specified.
    list(FIND Qt5Qmqtt_FIND_COMPONENTS Private _check_private)
    if (NOT _check_private STREQUAL -1)
        foreach(_dir ${Qt5Qmqtt_PRIVATE_INCLUDE_DIRS})
            _qt5_Qmqtt_check_file_exists(${_dir})
        endforeach()
    endif()

    set(Qt5Qmqtt_INCLUDE_DIRS ${_Qt5Qmqtt_OWN_INCLUDE_DIRS})

    set(Qt5Qmqtt_DEFINITIONS -DQT_QMQTT_LIB)
    set(Qt5Qmqtt_COMPILE_DEFINITIONS QT_QMQTT_LIB)
    set(_Qt5Qmqtt_MODULE_DEPENDENCIES "Network;Core")


    set(Qt5Qmqtt_OWN_PRIVATE_INCLUDE_DIRS ${Qt5Qmqtt_PRIVATE_INCLUDE_DIRS})

    set(_Qt5Qmqtt_FIND_DEPENDENCIES_REQUIRED)
    if (Qt5Qmqtt_FIND_REQUIRED)
        set(_Qt5Qmqtt_FIND_DEPENDENCIES_REQUIRED REQUIRED)
    endif()
    set(_Qt5Qmqtt_FIND_DEPENDENCIES_QUIET)
    if (Qt5Qmqtt_FIND_QUIETLY)
        set(_Qt5Qmqtt_DEPENDENCIES_FIND_QUIET QUIET)
    endif()
    set(_Qt5Qmqtt_FIND_VERSION_EXACT)
    if (Qt5Qmqtt_FIND_VERSION_EXACT)
        set(_Qt5Qmqtt_FIND_VERSION_EXACT EXACT)
    endif()

    set(Qt5Qmqtt_EXECUTABLE_COMPILE_FLAGS "")

    foreach(_module_dep ${_Qt5Qmqtt_MODULE_DEPENDENCIES})
        if (NOT Qt5${_module_dep}_FOUND)
            find_package(Qt5${_module_dep}
                1.0.0 ${_Qt5Qmqtt_FIND_VERSION_EXACT}
                ${_Qt5Qmqtt_DEPENDENCIES_FIND_QUIET}
                ${_Qt5Qmqtt_FIND_DEPENDENCIES_REQUIRED}
                PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
            )
        endif()

        if (NOT Qt5${_module_dep}_FOUND)
            set(Qt5Qmqtt_FOUND False)
            return()
        endif()

        list(APPEND Qt5Qmqtt_INCLUDE_DIRS "${Qt5${_module_dep}_INCLUDE_DIRS}")
        list(APPEND Qt5Qmqtt_PRIVATE_INCLUDE_DIRS "${Qt5${_module_dep}_PRIVATE_INCLUDE_DIRS}")
        list(APPEND Qt5Qmqtt_DEFINITIONS ${Qt5${_module_dep}_DEFINITIONS})
        list(APPEND Qt5Qmqtt_COMPILE_DEFINITIONS ${Qt5${_module_dep}_COMPILE_DEFINITIONS})
        list(APPEND Qt5Qmqtt_EXECUTABLE_COMPILE_FLAGS ${Qt5${_module_dep}_EXECUTABLE_COMPILE_FLAGS})
    endforeach()
    list(REMOVE_DUPLICATES Qt5Qmqtt_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5Qmqtt_PRIVATE_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5Qmqtt_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5Qmqtt_COMPILE_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5Qmqtt_EXECUTABLE_COMPILE_FLAGS)

    set(_Qt5Qmqtt_LIB_DEPENDENCIES "Qt5::Network;Qt5::Core")


    add_library(Qt5::Qmqtt SHARED IMPORTED)

    set_property(TARGET Qt5::Qmqtt PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${_Qt5Qmqtt_OWN_INCLUDE_DIRS})
    set_property(TARGET Qt5::Qmqtt PROPERTY
      INTERFACE_COMPILE_DEFINITIONS QT_QMQTT_LIB)

    set_property(TARGET Qt5::Qmqtt PROPERTY INTERFACE_QT_ENABLED_FEATURES )
    set_property(TARGET Qt5::Qmqtt PROPERTY INTERFACE_QT_DISABLED_FEATURES )

    set(_Qt5Qmqtt_PRIVATE_DIRS_EXIST TRUE)
    foreach (_Qt5Qmqtt_PRIVATE_DIR ${Qt5Qmqtt_OWN_PRIVATE_INCLUDE_DIRS})
        if (NOT EXISTS ${_Qt5Qmqtt_PRIVATE_DIR})
            set(_Qt5Qmqtt_PRIVATE_DIRS_EXIST FALSE)
        endif()
    endforeach()

    if (_Qt5Qmqtt_PRIVATE_DIRS_EXIST)
        add_library(Qt5::QmqttPrivate INTERFACE IMPORTED)
        set_property(TARGET Qt5::QmqttPrivate PROPERTY
            INTERFACE_INCLUDE_DIRECTORIES ${Qt5Qmqtt_OWN_PRIVATE_INCLUDE_DIRS}
        )
        set(_Qt5Qmqtt_PRIVATEDEPS)
        foreach(dep ${_Qt5Qmqtt_LIB_DEPENDENCIES})
            if (TARGET ${dep}Private)
                list(APPEND _Qt5Qmqtt_PRIVATEDEPS ${dep}Private)
            endif()
        endforeach()
        set_property(TARGET Qt5::QmqttPrivate PROPERTY
            INTERFACE_LINK_LIBRARIES Qt5::Qmqtt ${_Qt5Qmqtt_PRIVATEDEPS}
        )
    endif()

    _populate_Qmqtt_target_properties(RELEASE "Qt5Qmqtt.dll" "libQt5Qmqtt.a" )



    _populate_Qmqtt_target_properties(DEBUG "Qt5Qmqttd.dll" "libQt5Qmqttd.a" )



    file(GLOB pluginTargets "${CMAKE_CURRENT_LIST_DIR}/Qt5Qmqtt_*Plugin.cmake")

    macro(_populate_Qmqtt_plugin_properties Plugin Configuration PLUGIN_LOCATION)
        set_property(TARGET Qt5::${Plugin} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

        set(imported_location "${_qt5Qmqtt_install_prefix}/plugins/${PLUGIN_LOCATION}")
        _qt5_Qmqtt_check_file_exists(${imported_location})
        set_target_properties(Qt5::${Plugin} PROPERTIES
            "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        )
    endmacro()

    if (pluginTargets)
        foreach(pluginTarget ${pluginTargets})
            include(${pluginTarget})
        endforeach()
    endif()




_qt5_Qmqtt_check_file_exists("${CMAKE_CURRENT_LIST_DIR}/Qt5QmqttConfigVersion.cmake")

endif()
