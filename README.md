# LZW

LZW is a dictionary based compression algorithm. It works quite simple --- if current word was seen before then we do nothing. If it was then we write code of the word we have read in the last step and add new word in the dictionary. Decoding process is almost simmetric to encoding.

I use uncompressed trie as a dictionary. It lets us add a new symbol right after we didn't succeed in finding one in a dictionary in O(1) (because we already found a place for it while we were looking for it in a dictionary).

# Vitter algorithm

Vitter compression algorithm is an improvement of a FGK algorithm. It constructs optimal Huffman tree while keeping it balanced as much as possible.
