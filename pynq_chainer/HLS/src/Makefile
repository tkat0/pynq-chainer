CC = arm-linux-gnueabihf-gcc
CPP = arm-linux-gnueabihf-g++

OBJSH = devreg.o cf_stub.o mmult_accel.o portinfo.o 
LIBS = -l sds_lib
INC = -I include/ -I include_hls/

all:	
	$(CC) -fPIC $(INC) -c -g3 -gstabs devreg.c
	$(CC) -fPIC $(INC) -c -g3 -gstabs cf_stub.c
	$(CC) -fPIC $(INC) -c -g3 -gstabs mmult_accel.cpp
	$(CC) -fPIC $(INC) -c -g3 -gstabs portinfo.c
	$(CPP) -g3 -gstabs -shared -fPIC -rdynamic $(OBJSH) -Wl,--start-group \
            $(LIBS) -Wl,--end-group -o libaccel.so
	rm *.o

clean:
	rm -f *.so *.o
