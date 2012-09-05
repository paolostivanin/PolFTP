AC_INIT([FTPUtils], [1.0-alpha6], [admin@polslinux.it])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADERS(stdio.h string.h unistd.h sys/types.h stdint.h signal.h dirent.h netdb.h netinet/in.h sys/socket.h openssl/sha.h ,,AC_MSG_ERROR[
required header file missing])
AC_CHECK_FUNCS(gethostbyname socket htons connect strlen strdup atoi,,AC_MSG_ERROR([
required standard library function missing]))
AC_CONFIG_FILES([
Makefile
src/client/Makefile
src/server/concurrent/Makefile
])
AC_OUTPUT

