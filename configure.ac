# libniftyprefs configure.ac 
# v0.3 - (c) Daniel Hiepler (2010-2012)


# --------------------------------
#    initialization
# --------------------------------
AC_INIT([libniftyprefs], m4_esyscmd([./version.sh --long]), [http://github.com/niftylight/niftylight/issues], [niftyprefs], [http://wiki.niftylight.de/AC_PACKAGE_NAME])
AM_INIT_AUTOMAKE([color-tests dist-bzip2 parallel-tests silent-rules])
AC_CONFIG_MACRO_DIR([m4])


# --------------------------------
#    information on the package
# --------------------------------

# infos
AC_DEFINE(PACKAGE_DESCRIPTION, [lightweight modelless preferences management library], [description])

# versioning (s. version.sh)
AC_SUBST([PACKAGE_MAJOR_VERSION], m4_esyscmd([./version.sh --major]), [major version])
AC_SUBST([PACKAGE_MINOR_VERSION], m4_esyscmd([./version.sh --minor]), [minor version])
AC_SUBST([PACKAGE_MICRO_VERSION], m4_esyscmd([./version.sh --micro]), [micro version])
AC_SUBST([PACKAGE_API_REVISION], m4_esyscmd([./version.sh --api-revision]), [api revision])
AC_SUBST([PACKAGE_API_CURRENT], m4_esyscmd([./version.sh --api-current]), [api current])
AC_SUBST([PACKAGE_API_AGE], m4_esyscmd([./version.sh --api-age]), [api age])
AC_SUBST([PACKAGE_GIT_VERSION], m4_esyscmd([./version.sh --git]), [git version])


# --------------------------------
#    checks for programs
# --------------------------------
LT_INIT
PKG_PROG_PKG_CONFIG([0.14])
if test -z "$PKG_CONFIG"; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/ or check your distribution.])
fi


# --------------------------------
#    checks for libraries
# --------------------------------
PKG_CHECK_MODULES(xml, [libxml-2.0 >= 2.4], [], [AC_MSG_ERROR([You need libxml-2.0 (>= 2.4) + development headers installed])])
AC_SUBST(xml_CFLAGS)
AC_SUBST(xml_LIBS)

PKG_CHECK_MODULES(niftylog, [niftylog >= 0.1], [], [AC_MSG_ERROR([You need libniftylog + development headers installed])])
AC_SUBST(niftylog_CFLAGS)
AC_SUBST(niftylog_LIBS)


# --------------------------------
#    checks for header files
# --------------------------------


# --------------------------------
#    checks for types
# --------------------------------


# --------------------------------
#    checks for structures
# --------------------------------


# --------------------------------
#    checks for compiler characteristics
# --------------------------------


# --------------------------------
#    checks for library functions
# --------------------------------
AC_CHECK_FUNC(strndup, 
        AC_DEFINE([HAVE_STRNDUP],
        [1],
        [defined if strndup is available]))


# --------------------------------
#    checks for system services
# --------------------------------


# --------------------------------
#   compile process
# --------------------------------
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([no])])


# --------------------------------
# Output
# --------------------------------
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(
[
    Makefile \
    src/Makefile \
    include/Makefile \
    include/niftyprefs-version.h
])
AC_OUTPUT


echo "
Configure report:

    Package.....................:  ${PACKAGE_STRING}
    Libtool version.............:  ${PACKAGE_API_CURRENT}:${PACKAGE_API_REVISION}:${PACKAGE_API_AGE}
    Bugreports..................:  ${PACKAGE_BUGREPORT}

    Install prefix..............:  ${prefix}
    C compiler..................:  ${CC}
    C++ compiler................:  ${CPP}
"