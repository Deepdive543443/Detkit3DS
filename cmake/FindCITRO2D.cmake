macro(msys_to_cmake_path MsysPath ResultingPath)
    string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro()

if(NOT DEVKITPRO)
    msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
endif()

set(CITRO2D_PATHS $ENV{CTRULIB} libctru ctrulib ${DEVKITPRO}/libctru ${DEVKITPRO}/ctrulib)

find_path(LIBCITRO2D_INCLUDE_DIR citro2d.h
          PATHS ${CITRO2D_PATHS}
          PATH_SUFFIXES include libctru/include )

find_library(LIBCITRO2D_LIBRARY NAMES citro2d libcitro2d.a
          PATHS ${CITRO2D_PATHS}
          PATH_SUFFIXES lib libctru/lib )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCTRU_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CITRO2D  DEFAULT_MSG
                                    LIBCITRO2D_LIBRARY LIBCITRO2D_INCLUDE_DIR)

mark_as_advanced(LIBCITRO2D_INCLUDE_DIR LIBCITRO2D_LIBRARY)
if(CITRO2D_FOUND)
    set(CITRO2D ${LIBCITRO2D_INCLUDE_DIR}/..)
    message(STATUS "setting CITRO2D to ${CITRO2D}")

    add_library(3ds::citro2d STATIC IMPORTED GLOBAL)
    set_target_properties(3ds::citro2d PROPERTIES
        IMPORTED_LOCATION "${LIBCITRO2D_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBCITRO2D_INCLUDE_DIR}"
    )
endif()
