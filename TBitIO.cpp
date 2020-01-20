#include "TBitIO.hpp"
#include <bitset>

TBitIO::TBitIO(std::istream *ptr) {
    __EOF = false;
    Used = 8;
    In = ptr;
    Out = nullptr;
    return;
}

TBitIO::TBitIO(std::ostream *ptr) {
    __EOF = false;
    Used = 0;
    Out = ptr;
    In = nullptr;
    Buffer = 0;
    return;
}

void TBitIO::reopen(std::istream *ptr) {
    __EOF = false;
    Used = 8;
    In = ptr;
    Out = nullptr;
    return;
}

void TBitIO::reopen(std::ostream *ptr) {
    __EOF = false;
    Used = 0;
    Out = ptr;
    In = nullptr;
    Buffer = 0;
    return;
}

TBitIO& TBitIO::operator<<(std::string &str) {
    for (char c : str)
        if (c == '0')
            Send(0, 1);
        else
            Send(1, 1);
    return *this;
}

void TBitIO::Send(unsigned char from, unsigned char n) {
    unsigned char mask = 1 << (n - 1);
    while (n) {
        Buffer = (Buffer << 1);
        if (from & mask)
            Buffer |= 1;
        ++Used;
        if (Used == 8) {
            Out->write(reinterpret_cast<char*>(&Buffer), 1);
            Used = 0;
            Buffer = 0;
        }
        mask = mask >> 1;
        --n;
    }
    return;
}

void TBitIO::Send(int from, unsigned char n) {
    unsigned int mask = 1 << (n - 1);
    while (n) {
        Buffer = (Buffer << 1);
        if (from & mask)
            Buffer |= 1;
        ++Used;
        if (Used == 8) {
            Out->write(reinterpret_cast<char*>(&Buffer), 1);
            Used = 0;
            Buffer = 0;
        }
        mask = mask >> 1;
        --n;
    }
    return;
}

void TBitIO::Send(std::size_t fromm, unsigned char n) {
    std::bitset<64> mask;
    std::bitset<64> from(fromm);
    mask.set(63);
    while (n) {
        Buffer = (Buffer << 1);
        if ((from & mask).to_ullong())
            Buffer |= 1;
        ++Used;
        if (Used == 8) {
            Out->write(reinterpret_cast<char*>(&Buffer), 1);
            Used = 0;
            Buffer = 0;
        }
        mask = mask >> 1;
        --n;
    }
    return;
}

std::size_t TBitIO::Recv(int n) {
    std::bitset<64> ans(0);
    static unsigned char mask = 128;
    while (n) {
        if (Used == 8) {
            In->read(reinterpret_cast<char*>(&Buffer), 1);
            Used = 0;
            mask = 128;
        }
        else
            mask = mask >> 1;
        ans = (ans << 1);
        if (Buffer & mask)
            ans.set(0);
        ++Used;
        --n;
    }
    return ans.to_ullong();
}

int TBitIO::Recv(unsigned char n) {
    int ans = 0;
    static unsigned char mask = 128;
    while (n) {
        if (Used == 8) {
            In->read(reinterpret_cast<char*>(&Buffer), 1);
            Used = 0;
            mask = 128;
        }
        else
            mask = mask >> 1;
        ans = (ans << 1);
        if (Buffer & mask)
            ans |= 1;
        ++Used;
        --n;
    }
    return ans;
}

unsigned char TBitIO::Recv(char n) {
    unsigned char ans = 0;
    static unsigned char mask = 128;
    while (n) {
        if (Used == 8) {
            In->read(reinterpret_cast<char*>(&Buffer), 1);
            Used = 0;
            mask = 128;
        }
        else
            mask = mask >> 1;
        ans = (ans << 1);
        if (Buffer & mask)
            ans |= 1;
        ++Used;
        --n;
    }
    return ans;
}

void TBitIO::read(char* ptr, std::size_t n) {
    In->read(ptr, n);
    return;
}

void TBitIO::write(char* ptr, std::size_t n) {
    Out->write(ptr, n);
    return;
}

bool TBitIO::good() {
    if (In != nullptr)
        return In->good();
    return Out->good();
}

bool TBitIO::Good() {
    if (__EOF) {
        return true;
    }
    return good();
}

void TBitIO::seekp(std::streamoff pos, std::ios_base::seekdir way) {
    Out->seekp(pos, way);
}

std::streampos TBitIO::tellp() {
    return Out->tellp();
}

void TBitIO::Close() {
    if (Out != nullptr && Used != 0) {
        Buffer = (Buffer << (8 - Used));
        Out->write(reinterpret_cast<char*>(&Buffer), 1);
        Used = 0;
        Buffer = 0;
    }
    return;
}
