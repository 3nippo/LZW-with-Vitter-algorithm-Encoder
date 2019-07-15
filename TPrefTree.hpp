#pragma once

#include <map>
#include <vector>
#include <string>

struct SearchRes {
	int Code;
	std::size_t NodeNum;
};

class TPrefTree {
	private:
		class TPNode {
			friend TPrefTree;
			private:
				std::vector<std::size_t> NextPoints;
				int Code;
			public:
				TPNode();
				TPNode(const TPNode &right);
				TPNode(TPNode &&right);
				TPNode& operator=(const TPNode &right);
				TPNode& operator=(TPNode &&right);
				std::size_t find(unsigned char c);
				std::size_t add(unsigned char c, int point);
		};
		std::vector<TPNode> Points;
		int NextCode;
	public:
		TPrefTree();
		void Clear();
		void Add(std::string &str);
		void Add(std::size_t nodeNum, char c);
		SearchRes Get(std::string &str);
		SearchRes Get(std::size_t nodeNum, char c);
		SearchRes Get(std::size_t nodeNum);
		int Size();
};
