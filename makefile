buildbplustree: bplustree.cpp bplustree.hpp main.cpp node.cpp
	g++ -w -std=c++11 bplustree.cpp node.cpp main.cpp -o treesearch
