CFLAGS = -c -O3 -std=c++17 -pedantic -Wall -Werror -Wno-sign-compare -Wno-long-long -lm -lstdc++fs

all: main.o TBitIO.o TGZIP.o THuff.o TLZW.o TPrefTree.o 
    g++-8 main.o TBitIO.o TGZIP.o THuff.o TLZW.o TPrefTree.o -o KP

main.o: main.cpp
    g++-8 $(CFLAGS) main.cpp
    
TBitIO.o: TBitIO.cpp
    g++-8 $(CFLAGS) TBitIO.cpp
    
TGZIP.o: TGZIP.cpp
    g++-8 $(CFLAGS) TGZIP.cpp
    
THuff.o: THuff.cpp
    g++-8 $(CFLAGS) THuff.cpp
    
TLZW.o: TLZW.cpp
    g++-8 $(CFLAGS) TLZW.cpp
    
TPrefTree.o: TPrefTree.cpp
    g++-8 $(CFLAGS) TPrefTree.cpp


