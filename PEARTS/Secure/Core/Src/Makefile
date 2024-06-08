
#  Build appendVec.cpp
CC=gcc
# CPPFLAGS=-Wall -g -std=c++11

$(OUTDIR):
	mkdir -p $(OUTDIR)
	
all: $(OUTDIR) 
	$(CC) -o test.exe $(CPPFLAGS) test.c 

run: all
	test.exe 

clean:
	rm -f *.exe *.o
