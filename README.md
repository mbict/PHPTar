PHPTar
======
PHP Tar wrapper extension for php

This is a simple Tar extension for reading and extracting tar Archives.
It does not support adding files to the tar Yet!

This is part of a personal project and i was amazed PHP didnt have any support for reading tar file.
You will need libtar, zlib and bzip2 and php development library installed to compile the extension.

Compile and install
======
Compiling is simple (only linux/OSX).

phpize
./configure
make
make install

Example/tests
======
Check the tests dir for example how to extract a file and iterate through the file.
There is also a function for getting the content list of  the tar