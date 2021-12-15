Name: GreyAverageTool.exe
Date: 14.12.2021
Author: Martin Lohrmann

This command line tool reads the path to a folder as a command line argument, reads all .pgm files from the given folder and creates an
average image from the .pgm files in the same folder.

The program accepts command-line parameters when called:

"Options:\n"
-h,--help  : Show this help message
-w,--width : Specify the expected width of the .pgm file
-l,--lines : Specify the expected lines/height of the .pgm file
-m,--max   : Specify max. value of the .pgm file

The program asks for a path to the .pgm files that shall be averaged. It awaits a path without spaces in it and without trailing backslash.

Only those images that match the following criteria are considered for the average result image:
(according to http://netpbm.sourceforge.net/doc/pgm.html Plain PGM)
- The magic number of the input image is "P2".
- The input images size is according to specified width and height (default: 1856 x 1024)
- There is a value given for Maxval (default: 4096)
- There are additionally <height> lines of <width> grey values seperated by Whitespace in the file.

Values are expected as ASCII decimal numbers.

Maxval is currently not evaluated because in the example files there are several pictures with pixel values higher than the defined 
maximum value.
Therefore pixel values bigger than Maxval will also be processed. Also images with Maxval bigger than the default value or the
command-line specified value will be processed without modification.

Comments (starting with #) are only expected above/in the header of the file or after the block of width x height pixel values.

In case there are pgm files with a header like expected but the number of grey values do not match the header or there is a comment (#) in the 
grey value matrix, the program will still try to perform the calculation but issues a warning.

The grey values of the images that match the above mentioned criteria are summed up pixel by pixel. Subsequent the summed up grey values are
divided by the number of valid images. The calculation cuts of the decimal place.
(Example: summed up grey values of 5 divided by 2 images will result in a value of 2)
