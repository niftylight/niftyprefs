#############
# libniftyprefs Makefile.am
# v0.4 - Daniel Hiepler <daniel@niftylight.de>



# module-subdirs to include
SUBDIRS =


# directories to include
INCLUDE_DIRS = \
	-I$(top_srcdir)/include -I$(top_srcdir)/src -I$(srcdir) \
	-I$(top_builddir)/include -I$(top_builddir)/src -I$(builddir)


# extra files to include in distribution
EXTRA_DIST = \
	obj.h \
	class.h \
	updater.h \
	prefs.h


# source-files
lib@PACKAGE@_la_SOURCES = \
	obj.c \
	class.c \
	node.c \
	node-prop.c \
	updater.c \
	version.c \
	array.c \
	prefs.c


# compile for debugging ?
if DEBUG
    DEBUG_CFLAGS = -DDEBUG
endif


# custom cflags
WARN_CFLAGS = -Wall -Wextra -Werror -Wno-unused-parameter


# target library
lib_LTLIBRARIES = lib@PACKAGE@.la

# cflags
lib@PACKAGE@_la_CFLAGS = \
		$(INCLUDE_DIRS) \
        $(xml_CFLAGS) \
        $(niftylog_CFLAGS) \
		$(WARN_CFLAGS) \
        $(DEBUG_CFLAGS) \
        -DPACKAGE_GIT_VERSION="\"`$(top_srcdir)/version --git`\""

# linker flags
lib@PACKAGE@_la_LDFLAGS = \
        -version-info @PACKAGE_API_CURRENT@:@PACKAGE_API_REVISION@:@PACKAGE_API_AGE@ \
        -Wall -no-undefined -no-allow-shlib-undefined \
        -export-symbols-regex [_]*\(nft_\|Nft\|NFT_\).* \
        $(xml_LIBS) \
        $(niftylog_LIBS)

# link in modules from subdirectories
lib@PACKAGE@_la_LIBADD = \
		$(SUBDIRS) \
		$(xml_LIBS) \
		$(niftylog_LIBS)
