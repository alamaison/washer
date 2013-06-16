# Find comet library
# Once done this will define
#  COMET_FOUND - System has Comet
#  COMET_INCLUDE_DIRS - The Comet include directories

find_path(COMET_INCLUDE_DIR comet/comet.h
          HINTS ${COMET_ROOT}/include /comet/include)

set(COMET_INCLUDE_DIRS ${COMET_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set COMET_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Comet DEFAULT_MSG
                                  COMET_INCLUDE_DIR)

mark_as_advanced(COMET_INCLUDE_DIR COMET_LIBRARY)