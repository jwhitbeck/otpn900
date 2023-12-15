# main program
bin_PROGRAMS = maeotp

maeotp_CPPFLAGS = $(HILDON_CFLAGS) $(LIBOSSO_CFLAGS) $(BASEENCODE_CFLAGS) -Wall
maeotp_LDADD = $(HILDON_LIBS) $(LIBOSSO_LIBS) $(BASEENCODE_LIBS)
maeotp_SOURCES = main.c tokenselector.c tokenview.c \
	pindialog.c ctdialog.c etdialog.c \
	util.c tokendb.c otp.c strconv.c

noinst_HEADERS = tokenselector.h tokenview.h \
	pindialog.h ctdialog.h etdialog.h \
	util.h tokendb.h otp.h strconv.h hildonize.h \
	config.h


# TESTS
check_PROGRAMS = test_hmac test_hotp test_motp test_errors

test_hmac_CPPFLAGS = $(HILDON_CFLAGS)
test_hmac_LDADD = $(HILDON_LIBS)
test_hmac_SOURCES = test_hmac.c otp.c strconv.c

test_hotp_CPPFLAGS = $(HILDON_CFLAGS)
test_hotp_LDADD = $(HILDON_LIBS)
test_hotp_SOURCES = test_hotp.c otp.c strconv.c

test_motp_CPPFLAGS = $(HILDON_CFLAGS)
test_motp_LDADD = $(HILDON_LIBS)
test_motp_SOURCES = test_motp.c otp.c strconv.c

test_errors_CPPFLAGS = $(HILDON_CFLAGS)
test_errors_LDADD = $(HILDON_LIBS)
test_errors_SOURCES = test_errors.c otp.c strconv.c


TESTS = $(check_PROGRAMS)
