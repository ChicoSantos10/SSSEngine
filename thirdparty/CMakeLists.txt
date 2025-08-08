# INVESTIGATE: Should we have this stored in cache? Probably should find a different way
# Either through functions/macros or through targets
set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRDPARTY_LIB_DIR ${THIRDPARTY_DIR}/libs)

include(ThirdParty.cmake)
