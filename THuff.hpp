#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <list>
#include "TBitIO.hpp"
#include <exception>
#include <memory>

class THuff {
    public:
	typedef int TCodeType;

	THuff();
	void PrepForWork(const std::string &sName, const std::string &rName, char keys);
	void Clear();
	void SetUncompSize(std::size_t size);
	void Encode(TBitIO &Source);
	void Decode(TBitIO &Result);
	std::size_t UncompSize();
	std::size_t CompSize();
	std::string InfoName();
	~THuff();
private:
	struct THData {
		THData *Parent;
		THData *LeftChild;
		THData *RightChild;

		bool Type; // 0 - leaf, 1 - internal
		std::size_t Weight;
		int Index;

		THData(THData *parent, bool type, std::size_t weight, int index);
		bool IsLeft();
		bool IsSiblingToNYT();
		~THData() {}

		static void Slide(int ia, int ib);
	};

	friend THData;

	struct THAData : public std::vector<THData*> {
		THData* IChangeWLeader(int i);
		THData* Slide(int i);
	};

	struct THAAllocator : public std::vector<THData> {
		int Used;
		THData* Get();
	};

	static THAAllocator Emptys;
	static THAData Data;

        TCodeType ART;
        char Keys;

	std::string SName;
        TBitIO Result;
        TBitIO Source;
        std::fstream Fin;
        std::fstream Fout;

	unsigned int Mask;
	unsigned int Buffer;

        static std::vector<int> Labels;
        static std::vector<TCodeType> RevLabels;

	void Upd(TCodeType a);
	void SlideAndInc(THData* &p);
        void EncSym(TCodeType a);
        void AddNode(TCodeType a);
        TCodeType DecSym();
        TCodeType ReceiveSym();
	void SendEOF();

	void Print();
	void PrintHelper(THData *node, unsigned long long space);

	std::size_t Compressed;
        std::size_t Uncompressed;
	static const int CHECK;
	std::string IName;
};
