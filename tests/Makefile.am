
INCLUDE_DIRS = -I$(top_srcdir)/include
tests_CFLAGS_PRIV = -Wall $(INCLUDE_DIRS) $(xml_CFLAGS) $(niftylog_CFLAGS)
tests_LDFLAGS_PRIV =
tests_LDADD_PRIV = ../src/libniftyprefs.la $(xml_LIBS) $(niftylog_LIBS)



check_PROGRAMS = obj-to-prefs prefs-to-obj
TESTS = $(check_PROGRAMS)




obj_to_prefs_SOURCES = 01_obj-to-prefs.c
obj_to_prefs_CFLAGS = $(tests_CFLAGS_PRIV)
obj_to_prefs_LDFLAGS = $(tests_LDFLAGS_PRIV)
obj_to_prefs_LDADD = $(tests_LDADD_PRIV)

prefs_to_obj_SOURCES = 02_prefs-to-obj.c
prefs_to_obj_CFLAGS = $(tests_CFLAGS_PRIV)
prefs_to_obj_LDFLAGS = $(tests_LDFLAGS_PRIV)
prefs_to_obj_LDADD = $(tests_LDADD_PRIV)
