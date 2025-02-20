set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "dsp-lib")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_RELEASE "1")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "DSP Library for Signal Processing with Eigen and FFTW3 support.")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libfftw3-dev, libeigen3-dev")

include(CPack)

