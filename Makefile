CFLAGS = -c -O3 -std=c++17 -pedantic -Wall -Werror -Wno-sign-compare -Wno-long-long -lm -lstdc++fs

all: main.o TBitIO.o TGZIP.o THuff.o TLZW.o TPrefTree.o 
	g++ main.o TBitIO.o TGZIP.o THuff.o TLZW.o TPrefTree.o -o KP -lstdc++fs

main.o: main.cpp
	g++ $(CFLAGS) main.cpp
	
TBitIO.o: TBitIO.cpp
	g++ $(CFLAGS) TBitIO.cpp
	
TGZIP.o: TGZIP.cpp
	g++ $(CFLAGS) TGZIP.cpp
	
THuff.o: THuff.cpp
	g++ $(CFLAGS) THuff.cpp
	
TLZW.o: TLZW.cpp
	g++ $(CFLAGS) TLZW.cpp
	
TPrefTree.o: TPrefTree.cpp
	g++ $(CFLAGS) TPrefTree.cpp


