dnl $Id$
dnl config.m4 for extension chdb

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(chdb, for chdb support,
dnl Make sure that the comment is aligned:
dnl [  --with-chdb             Include chdb support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(chdb, whether to enable chdb support,
dnl Make sure that the comment is aligned:
dnl [  --enable-chdb           Enable chdb support])

if test "$PHP_CHDB" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-chdb -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/chdb.h"  # you most likely want to change this
  dnl if test -r $PHP_CHDB/$SEARCH_FOR; then # path given as parameter
  dnl   CHDB_DIR=$PHP_CHDB
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for chdb files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CHDB_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CHDB_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the chdb distribution])
  dnl fi

  dnl # --with-chdb -> add include path
  dnl PHP_ADD_INCLUDE($CHDB_DIR/include)

  dnl # --with-chdb -> check for lib and symbol presence
  dnl LIBNAME=chdb # you may want to change this
  dnl LIBSYMBOL=chdb # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CHDB_DIR/lib, CHDB_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CHDBLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong chdb lib version or lib not found])
  dnl ],[
  dnl   -L$CHDB_DIR/lib -lm -ldl
  dnl ])
  dnl
  dnl PHP_SUBST(CHDB_SHARED_LIBADD)

  PHP_NEW_EXTENSION(chdb, chdb.c, $ext_shared)
fi
