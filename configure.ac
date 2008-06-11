dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(modcom_server, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC




AM_PROG_LIBTOOL



PKG_CHECK_MODULES(MODCOM_SERVER, [glib-2.0 gmodule-2.0   ])
AC_SUBST(MODCOM_SERVER_CFLAGS)
AC_SUBST(MODCOM_SERVER_LIBS)


AC_OUTPUT([
Makefile
])
