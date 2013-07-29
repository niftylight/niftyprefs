#############
# libniftyprefs configure.ac
# v0.5 - (c) Daniel Hiepler (2010-2013)


# --------------------------------
#    initialization
# --------------------------------

# package info
AC_INIT([libniftyprefs], m4_esyscmd([./version --short]), [http://github.com/niftylight/niftyprefs/issues], [niftyprefs], [http://wiki.niftylight.de/AC_PACKAGE_NAME])
AC_COPYRIGHT([Copyright (C) 2006-2013 Daniel Hiepler])

# for cross compiling
AC_CANONICAL_SYSTEM

# autotools initialization
AC_LANG(C)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([color-tests dist-bzip2 parallel-tests silent-rules -Wno-portability -Wall])



# --------------------------------
#    further info on the package
# --------------------------------

# infos
PACKAGE_DESCRIPTION="lightweight library to handle preferences/configuration files"
PACKAGE_DESCRIPTION_LONG="lightweight library to handle preferences for arbitrary (tree) objects using XML"

AC_SUBST([PACKAGE_DESCRIPTION], [${PACKAGE_DESCRIPTION}], [package description])
AC_SUBST([PACKAGE_DESCRIPTION_LONG], [${PACKAGE_DESCRIPTION_LONG}], [long package description])
AC_DEFINE_UNQUOTED(PACKAGE_DESCRIPTION, "${PACKAGE_DESCRIPTION}", [package description])
AC_DEFINE_UNQUOTED(PACKAGE_DESCRIPTION_LONG, "${PACKAGE_DESCRIPTION_LONG}", [long package description])

# versioning (s. version script)
AC_SUBST([PACKAGE_MAJOR_VERSION], m4_esyscmd([./version --major]), [major version])
AC_SUBST([PACKAGE_MINOR_VERSION], m4_esyscmd([./version --minor]), [minor version])
AC_SUBST([PACKAGE_MICRO_VERSION], m4_esyscmd([./version --micro]), [micro version])
AC_SUBST([PACKAGE_API_REVISION], m4_esyscmd([./version --api-revision]), [api revision])
AC_SUBST([PACKAGE_API_CURRENT], m4_esyscmd([./version --api-current]), [api current])
AC_SUBST([PACKAGE_API_AGE], m4_esyscmd([./version --api-age]), [api age])
AC_SUBST([PACKAGE_GIT_VERSION], m4_esyscmd([./version --git]), [git version])


# -----------------------------------------------
#    libtool stuff
# -----------------------------------------------
LT_INIT
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL


# --------------------------------
#    checks for programs
# --------------------------------
dnl change this to 0.28 as soon as it's widely used. Then also delete m4/pkg.m4
PKG_PROG_PKG_CONFIG([0.25])
if test -z "${PKG_CONFIG}"; then
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
AC_HEADER_STDC


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
AC_PROG_CC_C99
AM_PROG_CC_C_O
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])


# --------------------------------
# Arguments for configure-script
# --------------------------------
AC_ARG_ENABLE(
        debug,
		AS_HELP_STRING([--enable-debug], [enable debugging, default: no]),
		[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
		esac],
		[debug=false])
AM_CONDITIONAL(DEBUG, test x$debug = xtrue)




# --------------------------------
# Output
# --------------------------------
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(
[
    Doxyfile \
    Makefile \
    src/Makefile \
    src/version.c \
    tests/Makefile \
    include/Makefile \
    include/niftyprefs-version.h \
    $PACKAGE.pc \
    debian/control.tmpl \
    gentoo/dev-libs/niftyprefs/niftyprefs-9999.ebuild
])
AC_OUTPUT


echo -e "
Configure report:

\tPackage.....................:  ${PACKAGE_STRING}
\tDescription.................:  ${PACKAGE_DESCRIPTION}
\tRevision....................:  ${PACKAGE_GIT_VERSION}
\tLibtool version.............:  ${PACKAGE_API_CURRENT}:${PACKAGE_API_REVISION}:${PACKAGE_API_AGE}
\tURL.........................:  ${PACKAGE_URL}
\tBugreports..................:  ${PACKAGE_BUGREPORT}

\tInstall prefix..............:  ${prefix}
\tC compiler..................:  ${CC}
\tSystem CFLAGS...............:  ${CFLAGS}
\tSystem CXXFLAGS.............:  ${CXXFLAGS}
\tSystem LDFLAGS..............:  ${LDFLAGS}
"
