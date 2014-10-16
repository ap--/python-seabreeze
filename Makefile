
COMPL=-I./SeaBreeze/include -lseabreeze \
      -I/usr/include/python2.7 -lpython2.7 \
      -lusb -shared -fPIC 

all:
	cython seabreeze/seabreezecapi.pyx -o seabreeze/seabreezecapi.c
	gcc seabreeze/seabreezecapi.c $(COMPL) -o seabreeze/seabreezecapi.so
