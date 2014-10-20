
COMPL=-I./seabreeze-3.0.0/include \
      -I/usr/include/python2.7 -lpython2.7 \
      -L/usr/local/lib -lseabreeze \
      -lusb -shared -fPIC -Wl,-rpath=/usr/local/lib

all:
	cython seabreeze/sbcapi.pyx -o seabreeze/sbcapi.c
	gcc seabreeze/sbcapi.c $(COMPL) -o seabreeze/sbcapi.so

