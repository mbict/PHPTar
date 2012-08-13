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

#include "php_tar.h"

int le_tarArchive;

//global module functions
static function_entry tar_functions[] = {
	PHP_FE( tar_open, NULL )
	PHP_FE( tar_list, NULL )
	PHP_FE( tar_read, NULL )
	PHP_FE( tar_getname, NULL )
	PHP_FE( tar_extract, NULL )
	PHP_FE( tar_close, NULL )
	{NULL,NULL,NULL}
};

static void php_tarArchive_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    php_tarArchive *tarArchive = (php_tarArchive*)rsrc->ptr;

    if (tarArchive)
    {
        if ( tarArchive->handle ) 
        {
            tar_close(tarArchive->handle);
        }
        efree( tarArchive );
    }
}

PHP_MINIT_FUNCTION( tar )
{
	le_tarArchive = zend_register_list_destructors_ex(php_tarArchive_dtor, NULL, PHP_TARARCHIVE_RES_NAME, module_number);
	
	REGISTER_LONG_CONSTANT( "TAR_DEFAULT", TAR_DEFAULT, CONST_CS|CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "TAR_GZ", TAR_GZ, CONST_CS|CONST_PERSISTENT );
	REGISTER_LONG_CONSTANT( "TAR_BZIP2", TAR_BZIP2, CONST_CS|CONST_PERSISTENT );
	
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION( tar )
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION( tar )
{
	php_info_print_table_start();
	php_info_print_table_header( 2, "Tar Module", "enabled" );
	//php_info_print_table_row( 2, "Version", PHP_tar_EXTVER );
	php_info_print_table_end();

}

zend_module_entry tar_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	PHP_TAR_EXTNAME,
	tar_functions,
	PHP_MINIT( tar ),
	PHP_MSHUTDOWN( tar ),
	NULL,
	NULL,
	PHP_MINFO( tar ),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_TAR_EXTVER,
#endif
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TAR
ZEND_GET_MODULE( tar )
#endif

int gzopen_frontend(char *pathname, int oflags, int mode)
{
	char *gzoflags;
	gzFile gzf;
	int fd;

	switch (oflags & O_ACCMODE)
	{
	case O_WRONLY:
		gzoflags = "wb";
		break;
	case O_RDONLY:
		gzoflags = "rb";
		break;
	default:
	case O_RDWR:
		errno = EINVAL;
		return -1;
	}

	fd = open(pathname, oflags, mode);
	if (fd == -1)
		return -1;

	if ((oflags & O_CREAT) && fchmod(fd, mode))
		return -1;

	gzf = gzdopen(fd, gzoflags);
	if (!gzf)
	{
		errno = ENOMEM;
		return -1;
	}

	return (int)gzf;
}

tartype_t gztype = { (openfunc_t) gzopen_frontend, (closefunc_t) gzclose,
	(readfunc_t) gzread, (writefunc_t) gzwrite
};

int bzopen_frontend(char *pathname, int oflags, int mode)
{
	char *bzoflags;
	BZFILE *bzf;
	int fd;

	switch (oflags & O_ACCMODE)
	{
	case O_WRONLY:
		bzoflags = "wb";
		break;
	case O_RDONLY:
		bzoflags = "rb";
		break;
	default:
	case O_RDWR:
		errno = EINVAL;
		return -1;
	}

	fd = open(pathname, oflags, mode);
	if (fd == -1)
		return -1;

	if ((oflags & O_CREAT) && fchmod(fd, mode))
		return -1;

	bzf = BZ2_bzdopen(fd, bzoflags);
	if (!bzf)
	{
		errno = ENOMEM;
		return -1;
	}

	return (int)bzf;
}

tartype_t bztype = { (openfunc_t) bzopen_frontend, (closefunc_t) BZ2_bzclose,
	(readfunc_t) BZ2_bzread, (writefunc_t) BZ2_bzwrite
};

PHP_FUNCTION( tar_open )
{
	php_tarArchive *tarArchive;
	TAR *handle;
	char *sFilename;
	int iFilename_len;
	long iType = 1;
	
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l!", &sFilename, &iFilename_len, &iType ) == FAILURE ) 
    {
        RETURN_FALSE;
    }
	
	if ( tar_open( &handle, sFilename, (iType == TAR_GZ ? &gztype : ( iType == TAR_BZIP2 ? &bztype : NULL ) ), O_RDONLY, 0644, 0 ) == 0)
	{
		tarArchive = ecalloc( sizeof(php_tarArchive), 1 );
		tarArchive->bExtracted = 1;
		tarArchive->handle = handle;
		ZEND_REGISTER_RESOURCE(return_value, tarArchive, le_tarArchive);
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION( tar_list )
{
	php_tarArchive *tarArchive;
    zval *zResource;
	int i;
	
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zResource) == FAILURE) 
    {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(tarArchive, php_tarArchive*, &zResource, -1, PHP_TARARCHIVE_RES_NAME, le_tarArchive);
    
    if ( array_init(return_value) == FAILURE ) 
    	RETURN_FALSE;
    	
    while ((i = th_read(tarArchive->handle)) == 0)
	{
		add_next_index_string(return_value, th_get_pathname(tarArchive->handle), 1);

		if (TH_ISREG(tarArchive->handle) && tar_skip_regfile(tarArchive->handle) != 0)
		{
			RETURN_FALSE;
		}
	}
}

PHP_FUNCTION( tar_read )
{
	php_tarArchive *tarArchive;
    zval *zResource;

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zResource ) == FAILURE ) 
    {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(tarArchive, php_tarArchive*, &zResource, -1, PHP_TARARCHIVE_RES_NAME, le_tarArchive);

	if ( tarArchive->bExtracted == 0 )
	{
		if (TH_ISREG(tarArchive->handle) && tar_skip_regfile(tarArchive->handle) != 0)
		{
			//strerror(errno));
			RETURN_FALSE;
		}
	}

	if ( th_read(tarArchive->handle) == 0 )
	{
		tarArchive->bExtracted = 0;
		RETURN_TRUE;
	}
	else
	{
		RETURN_FALSE;
	}
}

PHP_FUNCTION( tar_getname )
{
	php_tarArchive *tarArchive;
    zval *zResource;
    char *filename;

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zResource ) == FAILURE ) 
    {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(tarArchive, php_tarArchive*, &zResource, -1, PHP_TARARCHIVE_RES_NAME, le_tarArchive);

	filename = th_get_pathname( tarArchive->handle );
	
	if ( filename != NULL )
	{
		RETURN_STRING( filename, 1 );
	}
	else
	{
		RETURN_FALSE;
	}
}
 

PHP_FUNCTION( tar_extract )
{
	php_tarArchive *tarArchive;
    zval *zResource;
    char *sDestination;
	int iDestination_len;
	
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zResource, &sDestination, &iDestination_len ) == FAILURE ) 
    {
        RETURN_FALSE;
    }

    ZEND_FETCH_RESOURCE(tarArchive, php_tarArchive*, &zResource, -1, PHP_TARARCHIVE_RES_NAME, le_tarArchive);
    
    tarArchive->bExtracted = 1;
    if ( tar_extract_file( tarArchive->handle, sDestination ) == 0 )
    {
    	RETURN_TRUE;
    }
    else
    {
    	RETURN_FALSE;
    }
}

PHP_FUNCTION( tar_close )
{
    zval *zResource;
	
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zResource ) == FAILURE ) 
    {
        RETURN_FALSE;
    }

    zend_list_delete( Z_LVAL_P( zResource ) );
    
    RETURN_TRUE;
}
