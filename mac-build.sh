#!bin/bash
mkdir qt-build && cd qt-build
curl -o qt4.tar.gz http://ftp.jaist.ac.jp/pub/qtproject/archive/qt/4.8/4.8.6/qt-everywhere-opensource-src-4.8.6.tar.gz
tar -zxf qt4.tar.gz
ls
cd qt* 
echo "yes" | ./configure
make -j 16 --silent
sudo make install