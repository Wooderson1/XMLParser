# XMLParser

### This program parses XML source code(srcML).

* This program was started as a single file provided by one of my professors.<br>
It was a single file that was poorly written(his words).

## My Work
* I have fixed the functionality of a previous infinite while() loop that caused the program<br>
to break under certain circumstances. 

* My primary work was seperating concerns of XML and srcML into seperete files, resulting in a<br>
driver file that is only concerned with srcML or XML. Now it is easy to write a quick<br>
program to parse srcML or XML without having to be concerned with the details of parsing.<br>

* Inversion of Control and Lambda functions were a couple of the primary improvements<br> 
to this program.

## What the program does
* Calculates various counts on a source-code project, including files, functions,<br>
comments, etc.

* Input is a srcML form of the project source code(demo.xml.zip).

Notes:
* The integrated XML parser handles start tags, end tags, empty elements, attributes,<br>
characters, namespaces, (XML) comments, and CDATA.

## How to run it
I am running cmake --version 3.22.0

```console
mkdir build
```

```console
cd build
```

```console
cmake ..
```

1. To demo XML parsing:
```console 
make runxmlstats
```
2. To demo srcML parsing: 
```console 
make runsrcmlstats
```

