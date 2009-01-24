dnl $Id$
dnl config.m4 for extension ax_gameserver_query

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(ax_gameserver_query, for ax_gameserver_query support,
dnl Make sure that the comment is aligned:
dnl [  --with-ax_gameserver_query             Include ax_gameserver_query support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(ax_gameserver_query, whether to enable ax_gameserver_query support,
[  --enable-ax_gameserver_query           Enable ax_gameserver_query support])

if test "$PHP_AX_GAMESERVER_QUERY" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-ax_gameserver_query -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/ax_gameserver_query.h"  # you most likely want to change this
  dnl if test -r $PHP_AX_GAMESERVER_QUERY/$SEARCH_FOR; then # path given as parameter
  dnl   AX_GAMESERVER_QUERY_DIR=$PHP_AX_GAMESERVER_QUERY
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for ax_gameserver_query files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       AX_GAMESERVER_QUERY_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$AX_GAMESERVER_QUERY_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the ax_gameserver_query distribution])
  dnl fi

  dnl # --with-ax_gameserver_query -> add include path
  dnl PHP_ADD_INCLUDE($AX_GAMESERVER_QUERY_DIR/include)

  dnl # --with-ax_gameserver_query -> check for lib and symbol presence
  dnl LIBNAME=ax_gameserver_query # you may want to change this
  dnl LIBSYMBOL=ax_gameserver_query # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $AX_GAMESERVER_QUERY_DIR/lib, AX_GAMESERVER_QUERY_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_AX_GAMESERVER_QUERYLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong ax_gameserver_query lib version or lib not found])
  dnl ],[
  dnl   -L$AX_GAMESERVER_QUERY_DIR/lib -lm -ldl
  dnl ])
  dnl
  dnl PHP_SUBST(AX_GAMESERVER_QUERY_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ax_gameserver_query, php_ax_gameserver_query.c ax_gameserver_query.c, $ext_shared)
fi
