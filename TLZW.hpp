#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <filesystem>
#include "TBitIO.hpp"
#include <exception>
#include "TPrefTree.hpp"

class TLZW {
    public:
        typedef int TCodeType;
        TLZW();
		void PrepForWork(const std::string &sName, const std::string &rName, char keys);
        void Encode(TBitIO &Result);
        void Decode(TBitIO &Source);
		void Clear();
        unsigned long long UncompressedSize();
		~TLZW();
    private:

        class EncodeTable : public TPrefTree {
            private:
                unsigned long long GivenSize;
            public:
                void SetSize(const unsigned long long &givenSize);
                void AddWord(std::string &str);
				void AddWord(std::string &str, std::size_t nodeNum);
        };

        class DecodeTable : public std::vector<std::string> {
            private:
                unsigned long long GivenSize;
            public:
                void SetSize(const unsigned long long &givenSize);
                void AddCode(std::string &str);
                void Clear();
				std::size_t Size();
        };

		void SendBits(TCodeType bufToSend, TBitIO &Result);
		TCodeType ReceiveBits(TBitIO &Source);
		void EncCheckCodeLength();
		bool IncCodeLength();

        int ToSR;
		unsigned long long Sup;

        static const unsigned long long Fast;
        static const unsigned long long Best;
        char Keys;
        EncodeTable CodeTable;
        DecodeTable WordTable;
		std::ostream *Result;
        std::istream *Source;
		std::fstream Fin;
        std::fstream Fout;

        unsigned long long Uncompressed;
};
