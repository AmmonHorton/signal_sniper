set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "dsp-lib")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set(CPACK_RPM_PACKAGE_SUMMARY "DSP Library for Signal Processing")
set(CPACK_RPM_PACKAGE_DESCRIPTION "A fast, efficient DSP library with Eigen and FFTW3 support.")

include(CPack)

