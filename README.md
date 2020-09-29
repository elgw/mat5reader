# A MATLAB Level 5 MAT-file reader

This repo contains a small library to read the contents of MATLAB Level 5 MAT-files, and little program that uses this library to browse MAT-files.

You should probably not use this since MATLAB already provides a [C/C++ interface](https://se.mathworks.com/help/matlab/matlab-c-api-to-read-mat-file-data.html). 

Other options (not a complete list):
 * C: [tbeu/matio](https://github.com/tbeu/matio), [jkriege2/TinyMAT](https://github.com/jkriege2/TinyMAT) (write only)
 * Python: [`scipy.io.loadmat`](https://docs.scipy.org/doc/scipy/reference/generated/scipy.io.loadmat.html).
 * [GNU Octave](https://www.gnu.org/software/octave/). 
  * .NET: [matFileHandler](https://github.com/mahalex/MatFileHandler). 
  * R: [R.matlab](http://search.r-project.org/library/R.matlab/html/readMat.html).
  * Java: [HebiRobotics/MFL](https://github.com/HebiRobotics/MFL)

`matreader` can be used as a library while `matbrowser` is a small utility to show how to use the `matreader` library. It does not have full coverage of all data types and is not extensively tested. On the good side it only uses `libz` and is only a few `kB` when compiled.

This implementation is based on [http://www.mathworks.de/help/pdf_doc/matlab/matfile_format.pdf](http://www.mathworks.de/help/pdf_doc/matlab/matfile_format.pdf)

## Usage:
See `matbrowser.c` for examples on how to use the API.

Building on Ubuntu 20.10:
```
$ make -B
gcc -c -g -Wall src/matreader.c 
gcc -g -Wall src/matbrowser.c matreader.o -lm `pkg-config zlib --libs` -lreadline -o matbrowser
```

A sample session with matbrowser:
```
$ ./matbrowser 002.NM 
Reading 002.NM ... 

HEADER: MATLAB 5.0 MAT-file, Platform: GLNXA64, Created on: Tue Jun 30 10:53:07 2020                                        
SUBSYS DATA OFFSET: 0
VERSION: 256
ENDIAN INDICATOR: IM 7377(byte swapping needed)

matbrowser()
Select a variable by name or number
use '..' to go down a level
use Ctrl+d to quit
At root (2 variables)
0: M ([1x1] mxSTRUCT_CLASS)
1: N ([1x48] mxCELL_CLASS)
? M
M ([1x1] mxSTRUCT_CLASS)
0: dapifile ([1x56] mxCHAR_CLASS)
1: nTrueDots ([1x6] mxDOUBLE_CLASS)
2: channels ([1x6] mxCELL_CLASS)
3: nkmers ([1x1] mxDOUBLE_CLASS)
4: voxelSize ([1x3] mxDOUBLE_CLASS)
5: imageSize ([1x3] mxDOUBLE_CLASS)
6: mask ([1024x1024] mxDOUBLE_CLASS)
...
? mask
mask ([1024x1024] mxDOUBLE_CLASS)
[0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 0.000000 ...
? ..
M ([1x1] mxSTRUCT_CLASS)
0: dapifile ([1x56] mxCHAR_CLASS)
1: nTrueDots ([1x6] mxDOUBLE_CLASS)
2: channels ([1x6] mxCELL_CLASS)
3: nkmers ([1x1] mxDOUBLE_CLASS)
...
? channels
channels ([1x6] mxCELL_CLASS)
0:  ([1x3] mxCHAR_CLASS)
1:  ([1x4] mxCHAR_CLASS)
2:  ([1x4] mxCHAR_CLASS)
3:  ([1x4] mxCHAR_CLASS)
4:  ([1x5] mxCHAR_CLASS)
5:  ([1x3] mxCHAR_CLASS)
? 0
 ([1x3] mxCHAR_CLASS)
Cy5
```

## TODO
 - [ ] Write the code for conversion of a few more array types from intermal mi format to external mx format.

## Might do
 - [ ] Guard against corrupt or malign files (first of all: make sure that the size of the data elements are reasonable).

## Will not fix
 - Will not extend this from reading mat files on little-endian machines, that were generated on little endian-machines.
 - `mxOBJECT_CLASS`
 - `mxSPARSE_CLASS`
 - Make it industrial grade (check for all possible errors etc)
