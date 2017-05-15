# Upwards-Planarity
Upwards Planar Embedding with the OGDF C++ Library

# License
Refer back to the licenses for:
* [OGDF - GNU v2.0/3.0 with special exceptions](https://github.com/ogdf/ogdf/blob/ee4a91cc823e1bc16825a23e0c543aa77e749512/LICENSE.txt)
* [JSONcpp - MIT](https://github.com/open-source-parsers/jsoncpp/blob/master/LICENSE)

# Installation
These directions are for using this code directly, not as libraries.
## Installation for Unix systems
Make an empty installation directory {installdir} named however you like. Then, clone this repo into that directory.
### Install library dependencies: OGDF and JSONCpp
For this part of the installation, simply follow the directions for using CMAKE with [OGDF]() and [JSONcpp]() in separate directories inside {installdir}. We'll refer these as {OGDF} and {JSONcpp}.
### Compile Upwards-Planarity
Use g++ like the following to compile:
```shell
g++ -std=c++11 -L/{installdir}/ogdf -L/{installdir}/jsoncpp/build/debug/src/lib_json -I/{installdir}/ogdf/include -I/{installdir}/jsoncpp/include -o upge NodeListClass.cpp UPGrapher.cpp upge.cpp -lOGDF -lCOIN -ljsoncpp -pthread
```
### Set up the command in bash
Add the resulting executable "upge" to your path, then add the following to a file also called "upge" into /usr/bin:
```shell
#!bin/bash
for i in $@;
do
  params=" $params $i"
done
./upge $params$
```
You should now be able to use the upwards planar grapher from bash.

# Usage
The syntax for the program is:

  upge {input .off} {output .gml} {output .svg}

It takes an .off file graph as input and outputs a .gml representation of the graph (unembedded) and an upwards-planarly-embedded .svg representation.
