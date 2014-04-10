#############
# libniftyprefs Makefile.am
# v0.4 - Daniel Hiepler <daniel@niftylight.de>


EXTRA_DIST = \
	tests.env

# directories to include
INCLUDE_DIRS = \
	-I$(top_srcdir)/include \
	-I$(top_builddir)/include \
	-I$(srcdir)

# files to clean on "make distclean"
DISTCLEANFILES = \
	test-prefs-light.xml \
	test-prefs.xml

# custom cflags
WARN_CFLAGS = -Wall -Wextra -Werror -Wno-unused-parameter


TESTCFLAGS = \
	$(INCLUDE_DIRS) \
	$(WARN_CFLAGS) \
	$(xml_CFLAGS) \
	$(niftylog_CFLAGS)

TESTLDFLAGS = \
	-Wall -no-undefined

TESTLDADD = \
	$(top_builddir)/src/libniftyprefs.la \
	$(xml_LIBS) \
	$(niftylog_LIBS)



check_PROGRAMS = \
		array \
		api \
		obj-to-prefs \
		prefs-to-obj \
		update

TESTS = $(check_PROGRAMS)
AM_TESTS_ENVIRONMENT = $(srcdir)/tests.env;

array_SOURCES = array.c
array_CFLAGS = $(TESTCFLAGS)
array_LDFLAGS = $(TESTLDFLAGS)
array_LDADD = $(TESTLDADD)


api_SOURCES = api.c
api_CFLAGS = $(TESTCFLAGS)
api_LDFLAGS = $(TESTLDFLAGS)
api_LDADD = $(TESTLDADD)

obj_to_prefs_SOURCES = obj-to-prefs.c
obj_to_prefs_CFLAGS = $(TESTCFLAGS)
obj_to_prefs_LDFLAGS = $(TESTLDFLAGS)
obj_to_prefs_LDADD = $(TESTLDADD)

prefs_to_obj_SOURCES = prefs-to-obj.c
prefs_to_obj_CFLAGS = $(TESTCFLAGS)
prefs_to_obj_LDFLAGS = $(TESTLDFLAGS)
prefs_to_obj_LDADD = $(TESTLDADD)

update_SOURCES = update.c
update_CFLAGS = $(TESTCFLAGS)
update_LDFLAGS = $(TESTLDFLAGS)
update_LDADD = $(TESTLDADD)
