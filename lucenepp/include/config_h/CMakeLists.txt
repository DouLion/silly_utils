# ##############################################################################
# set LPPConfig.h vars
# ##############################################################################
set(USE_BOOST_INTEGER "define")

set(USE_CYCLIC_CHECK "define")

# set(LPP_SHARED_DLL "define")
set(LPP_SHARED_DLL "undef")

# ##############################################################################
# generate LPPConfig.h
# ##############################################################################
if(EXISTS "${LucenePP_SOURCE_ROOT}/include/config_h/LPPConfig.h.in")
  configure_file("${LucenePP_SOURCE_ROOT}/include/config_h/LPPConfig.h.in"
                 "${LucenePP_SOURCE_ROOT}/include/lucenepp/LPPConfig.h" @ONLY)
endif()
if(NOT EXISTS ${LucenePP_SOURCE_ROOT}/include/lucenepp/LPPConfig.h)
  message(FATAL_ERROR "Error While Generate LPPConfig.h")
endif()
