# libniftyprefs Makefile.am - v0.2 - Daniel Hiepler <daniel@niftylight.de> 

INCLUDE_DIRS = -I../include -I$(top_srcdir)/include


# module-subdirs to include
SUBDIRS =
EXTRA_DIST = 

# target library
lib_LTLIBRARIES = lib@PACKAGE_TARNAME@.la

# source-files
lib@PACKAGE_TARNAME@_la_SOURCES = \
	niftyprefs.c


# cflags 
lib@PACKAGE_TARNAME@_la_CFLAGS = -Wall

# linker flags
lib@PACKAGE_TARNAME@_la_LDFLAGS = \
        -version-info @PACKAGE_API_CURRENT@:@PACKAGE_API_REVISION@:@PACKAGE_API_AGE@ \
        -release @PACKAGE_MAJOR_VERSION@.@PACKAGE_MINOR_VERSION@.@PACKAGE_MICRO_VERSION@ \
        -Wall -no-undefined

# link in modules from subdirectories
lib@PACKAGE_TARNAME@_la_LIBADD = 
