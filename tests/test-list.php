<?php

echo "--------------------------------------------------------------\n";
echo " TAR only compression list\n";
echo "--------------------------------------------------------------\n";

$resource = tar_open('./test.tar');
var_dump( tar_list($resource) );
tar_close( $resource );

echo "--------------------------------------------------------------\n";
echo " TAR + GZ compression list\n";
echo "--------------------------------------------------------------\n";

$resource = tar_open('./test.tar.gz', TAR_GZ );
var_dump( tar_list($resource) );
tar_close( $resource );

echo "--------------------------------------------------------------\n";
echo " TAR + BZIP2 compression list\n";
echo "--------------------------------------------------------------\n";

$resource = tar_open('./test.tar.bz2', TAR_BZIP2 );
var_dump( tar_list($resource) );
tar_close( $resource );

echo "finished test\n";