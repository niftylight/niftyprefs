# libniftyprefs Makefile.am - v0.2 - Daniel Hiepler <daniel@niftylight.de> 

INCLUDE_DIRS = -I../include -I$(top_srcdir)/include -I.


# module-subdirs to include
SUBDIRS =
EXTRA_DIST = obj.h class.h prefs.h

# source-files
lib@PACKAGE@_la_SOURCES = \
	obj.c \
	class.c \
	node.c \
	version.c \
	array.c \
	prefs.c


# compile for debugging ?
if DEBUG
    OPT_CFLAGS = -O0 -g
else
    OPT_CFLAGS = -O2
endif


# target library
lib_LTLIBRARIES = lib@PACKAGE@.la

# cflags 
lib@PACKAGE@_la_CFLAGS = \
        -Wall -Wextra -Werror -Wno-unused-parameter \
        $(xml_CFLAGS) \
        $(niftylog_CFLAGS) \
        $(OPT_CFLAGS) \
        $(INCLUDE_DIRS) \
        -DPACKAGE_GIT_VERSION="\"`$(top_srcdir)/version.sh --git`\""

# linker flags
lib@PACKAGE@_la_LDFLAGS = \
        -version-info @PACKAGE_API_CURRENT@:@PACKAGE_API_REVISION@:@PACKAGE_API_AGE@ \
        -release @PACKAGE_MAJOR_VERSION@.@PACKAGE_MINOR_VERSION@.@PACKAGE_MICRO_VERSION@ \
        -Wall -no-undefined \
        $(xml_LIBS) \
        $(niftylog_LIBS)

# link in modules from subdirectories
lib@PACKAGE@_la_LIBADD = $(SUBDIRS) $(xml_LIBS) $(niftylog_LIBS)
