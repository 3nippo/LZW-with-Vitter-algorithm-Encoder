#pragma once

#include <iostream>
#include <string>

// Send bits from string
// Receive 1 bit
// Receive n bits
// Send n bits

class TBitIO {
    private:
        std::istream *In;
        std::ostream *Out;
        unsigned char Buffer;
        int Used;
        bool __EOF;
    public:
        TBitIO() {}
        TBitIO(std::istream *ptr);
        TBitIO(std::ostream *ptr);
        void seekp(std::streamoff pos, std::ios_base::seekdir way);
        std::streampos tellp();
        void read(char* ptr, std::size_t n);
        void write(char* ptr, std::size_t n);
        void reopen(std::istream *ptr);
        void reopen(std::ostream *ptr);
        bool good();
        bool Good();
        TBitIO& operator<<(std::string &str);
        void Send(int from, unsigned char n);
        void Send(unsigned char from, unsigned char n);
        void Send(std::size_t from, unsigned char n);
        unsigned char Recv(char n);
        int Recv(unsigned char n);
        std::size_t Recv(int n);
        void Close();
};
