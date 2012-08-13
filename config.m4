PHP_ARG_WITH(tar, for tar support,
[  --with-tar             Include tar support])

if test "$PHP_TAR" != "no"; then
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/libtar.h"
  if test -r $PHP_TAR/; then
    TAR_DIR=$PHP_TAR
  else # search default path list
    AC_MSG_CHECKING([for tar files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        TAR_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$TAR_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the tar distribution])
   fi

  PHP_ADD_INCLUDE($TAR_DIR/include)

  LIBNAME=tar
  LIBSYMBOL=tar_open

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TAR_DIR/lib, TAR_SHARED_LIBADD)
    AC_DEFINE(HAVE_TARLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong tar lib version or lib not found])
  ],[
     -L$TAR_DIR/lib -lm -ldl
  ])

  PHP_SUBST(TAR_SHARED_LIBADD)

  PHP_NEW_EXTENSION(tar, tar.c, $ext_shared)
fi