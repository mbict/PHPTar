/*
  +----------------------------------------------------------------------+
  | PHP Tar wrapper                                                      |
  +----------------------------------------------------------------------+
  | Copyright (c) 2010 MB-Ict                                            |
  +----------------------------------------------------------------------+
  | This is a simple Tar extension for reading and extracting tar        |
  | Archives. It does not support adding files to the tar Yet!           |
  |                                                                      |
  | This is part of a personal project and i was amazed PHP didnt have   |
  | any support for reading tar file.                                    |
  |                                                                      |
  | You will need libtar, zlib and bzip2 library installed               |
  | Compiling is simple ( only linux)                                    |
  | phpize                                                               |
  | ./configure                                                          |
  | make                                                                 |
  | make install                                                         |
  |                                                                      |
  | Check the tests dir for example how to extract a file and iterate    |
  | through the file. Also a function for getting the content list of    |
  | the tar                                                              |
  |                                                                      |
  | If you have any comments/bugs/spare money please send them ;)        |
  +----------------------------------------------------------------------+
  | Author: Michael Boke <michael.boke@gmail.com>                        |
  | Website: www.ly2.nl (personal blog dutch)                            |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_TAR_H
#define PHP_TAR_H 1

#define PHP_TAR_EXTNAME	"tar"
#define PHP_TAR_EXTVER	"1.0"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "zend.h"
#include "zend_variables.h"
#include "ext/standard/info.h"

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <zlib.h>
#include <bzlib.h>

#include <libtar.h>

#include "tar.h"

typedef struct _php_tarArchive {
	TAR *handle;
	int bExtracted;
} php_tarArchive;

#define PHP_TARARCHIVE_RES_NAME "Tar Archive"

#define TAR_DEFAULT 1
#define TAR_GZ 2
#define TAR_BZIP2 3

PHP_MINIT_FUNCTION( tar );
PHP_MSHUTDOWN_FUNCTION( tar );
PHP_MINFO_FUNCTION( tar );

PHP_FUNCTION( tar_open );
PHP_FUNCTION( tar_list );
PHP_FUNCTION( tar_read );
PHP_FUNCTION( tar_getname );
PHP_FUNCTION( tar_extract );
PHP_FUNCTION( tar_close );

extern zend_module_entry tar_module_entry;
#define phpext_tar_ptr &tar_module_entry

#endif
