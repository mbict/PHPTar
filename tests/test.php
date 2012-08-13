<?php
/*
$resource = tar_open('/home/projects/php_ext_tar/test.tar');

var_dump( tar_list($resource) );

tar_close( $resource );
*/

echo "--------------------------------------------------------------\n";
echo " TAR only compression\n";
echo "--------------------------------------------------------------\n";

$resource = tar_open('./test.tar');

while( false !== tar_read( $resource ) )
{
	$sFilename = tar_getname($resource);
	echo " ",$sFilename,"\n";
	
	if ( !strcasecmp( 'testfile2.txt',$sFilename ) )
	{
		echo 'extracting testfile2 as tar_testfile2.txt : ',
			( true === tar_extract( $resource, './tmp/tar_testfile2.txt') ? 'Success' : 'Failed' ),
			"\n";
	}
}
tar_close( $resource );

echo "--------------------------------------------------------------\n";
echo " TAR + GZ compression\n";
echo "--------------------------------------------------------------\n";

$resource = tar_open('./test.tar.gz', TAR_GZ );


while( false !== tar_read( $resource ) )
{
	$sFilename = tar_getname($resource);
	echo " ",$sFilename,"\n";
	
	if ( !strcasecmp( 'testfile2.txt',$sFilename ) )
	{
		echo 'extracting testfile2 as tar_gz_testfile2.txt : ',
			( true === tar_extract( $resource, './tmp/tar_gz_testfile2.txt') ? 'Success' : 'Failed' ),
			"\n";
	}
}
tar_close( $resource );

echo "--------------------------------------------------------------\n";
echo " TAR + BZIP2 compression\n";
echo "--------------------------------------------------------------\n";

$resource = tar_open('./test.tar.bz2', TAR_BZIP2 );

while( false !== tar_read( $resource ) )
{
	$sFilename = tar_getname($resource);
	echo " ",$sFilename,"\n";
	
	if ( !strcasecmp( 'testfile2.txt',$sFilename ) )
	{
		echo 'extracting testfile2 as tar_bz2_testfile2.txt : ',
			( true === tar_extract( $resource, './tmp/tar_bz2_testfile2.txt') ? 'Success' : 'Failed' ),
			"\n";
	}
}
tar_close( $resource );

echo "finished test\n";