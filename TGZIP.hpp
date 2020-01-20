#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include "THuff.hpp"
#include "TLZW.hpp"
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cmath>
#include "TBitIO.hpp"

class TGZIP {
    struct TGZData {
        std::string Name;
        std::size_t Compressed;
        std::size_t Uncompressed;
    };
    private:
        THuff Huff;
        TLZW LZW;

        char Key;
        std::vector<TGZData> Results;
        void PrintInfo();
        void Encode();
        void Decode();
        void DumpFromIn();
        void DumpToOut();
    public:
        TGZIP(std::string &name, std::string &resName, char key);
        ~TGZIP();
};
