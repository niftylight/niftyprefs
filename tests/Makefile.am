
INCLUDE_DIRS = -I$(top_srcdir)/include
tests_CFLAGS_PRIV = -Wall $(INCLUDE_DIRS) $(xml_CFLAGS) $(niftylog_CFLAGS) -g -O0
tests_LDFLAGS_PRIV =
tests_LDADD_PRIV = ../src/libniftyprefs.la $(xml_LIBS) $(niftylog_LIBS)



check_PROGRAMS = array api obj-to-prefs prefs-to-obj
TESTS = $(check_PROGRAMS)


array_SOURCES = array.c
array_CFLAGS = $(tests_CFLAGS_PRIV)
array_LDFLAGS = $(tests_LDFLAGS_PRIV)
array_LDADD = $(tests_LDADD_PRIV)


api_SOURCES = api.c
api_CFLAGS = $(tests_CFLAGS_PRIV)
api_LDFLAGS = $(tests_LDFLAGS_PRIV)
api_LDADD = $(tests_LDADD_PRIV)

obj_to_prefs_SOURCES = obj-to-prefs.c
obj_to_prefs_CFLAGS = $(tests_CFLAGS_PRIV)
obj_to_prefs_LDFLAGS = $(tests_LDFLAGS_PRIV)
obj_to_prefs_LDADD = $(tests_LDADD_PRIV)

prefs_to_obj_SOURCES = prefs-to-obj.c
prefs_to_obj_CFLAGS = $(tests_CFLAGS_PRIV)
prefs_to_obj_LDFLAGS = $(tests_LDFLAGS_PRIV)
prefs_to_obj_LDADD = $(tests_LDADD_PRIV)
