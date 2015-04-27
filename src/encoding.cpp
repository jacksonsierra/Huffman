/*
 *  File: encoding.cpp
 *  - - - - - - - - - - - - - - -
 *  This file implements the Huffman encoding algorithm,
 *  which reads in a file or text and compresses it
 *  into binary bits. Functionality is also provided
 *  to decompress a file from binary bits to its
 *  traditional output.
 */

#include "encoding.h"   // Header file
#include "map.h"        // Data type for character counts and encodings
#include "bitstream.h"  // I/O for binary bits
#include "pqueue.h"     // To build encoding tree
#include "strlib.h"     // String to integer conversions

/*
 *  Method: buildFrequencyTable
 *  Parameters: istream input by reference
 *  - - - - - - - - - - - - - - - - - -
 *  Returns a map with keys corresponding
 *  to the distinct characters, represented as integers,
 *  in the given input parameter referenced and values
 *  as the associated counts of each key in the input.
 *  A unique 'End of File' character is also included
 *  to identify an input's culmination.
 */
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    int ch;
    while((ch = input.get()) != EOF) {
        freqTable[ch]++;
    }
    freqTable[PSEUDO_EOF]++;
    return freqTable;
}

/*
 *  Method: buildEncodingTree
 *  Parameters: PriorityQueue of HuffmanNodes by reference
 *  - - - - - - - - - - - - - - - - - -
 *  Recursive function that returns the root of an
 *  encoding tree given a priority queue of the temporary tree's
 *  existing roots. Its base case is when the PriorityQueue's size
 *  is 1, indicating a properly formed binary tree. Its process 
 *  removes the two highest priority nodes and combines them into
 *  a branch that is reinserted into the tree, recursively doing
 *  this until there is a single root. This function is called
 *  by a wrapper function that formulates and passes the queue used.
 */
HuffmanNode* buildEncodingTree(PriorityQueue<HuffmanNode* >& queue) {
    if(queue.size() == 1) {
        return queue.dequeue();
    } else {
        HuffmanNode* node = new HuffmanNode;
        HuffmanNode* zero = queue.dequeue();
        HuffmanNode* one = queue.dequeue();
        node->character;
        node->count = zero->count + one->count;
        node->zero = zero;
        node->one = one;
        queue.enqueue(node, node->count);
        return buildEncodingTree(queue);
    }
}

/*
 *  Method: buildEncodingTree
 *  Parameters: Map of keys and values of integers by constant reference
 *  - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Wrapper function for recursive function that builds the binary
 *  encoding tree. This function builds a PriorityQueue given
 *  a frequency table of the characters in the input, passed by
 *  reference through a map, and then passes that queue to the 
 *  recursive function.
 */
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    PriorityQueue<HuffmanNode* > queue;
    for(int ch : freqTable) {
        HuffmanNode* node = new HuffmanNode;
        node->character = ch;
        node->count = freqTable[ch];
        node->zero = NULL;
        node->one = NULL;
        queue.enqueue(node, node->count);
    }
    return buildEncodingTree(queue);
}

/*
 *  Method: buildEncodingMap
 *  Parameters: HuffmanNode representing branch of binary tree
 *              Map of input's characters and associated encodings by reference
 *              String of binary digits denoting encoding path for character
 *  - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Recursive function that traverses down the encoding tree
 *  looking for leaves of the tree so to gather the respective
 *  leaf's encoding for insertion into a map. Its base case
 *  is whether a node doesn't exist, and within that whether
 *  the current node is a leaf, as indicated by whether it contains
 *  a character value.
 */
void buildEncodingMap(HuffmanNode* node, Map<int, string>& encodingMap, string path) {
    if(node != NULL) {
        if(node->character != NOT_A_CHAR) {
            encodingMap[node->character] = path;
        } else {
            buildEncodingMap(node->zero, encodingMap, path + "0");
            buildEncodingMap(node->one, encodingMap, path + "1");
        }
    }
}

/*
 *  Method: buildEncodingMap
 *  Parameters: HuffmanNode representing the root of a binary tree
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  Wrapper function that initializes a map that will be used
 *  to list all associated encoding values for the characters of 
 *  the given input, via the referenced encoding binary tree.
 *  The function returns the referenced map it passes at conclusion.
 */
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    string path = "";
    buildEncodingMap(encodingTree, encodingMap, path);
    return encodingMap;
}

/*
 *  Method: encodeData
 *  Parameters: istream input by reference
 *              Map of input's characters and associated encodings by constant reference
 *              bitstream output by reference
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  Outputs a given input's text into its binary representation provided with an
 *  encoding tree, which in this function is represented by a map for ease of use.
 *  Due to the data types chosen upstream, this function reads in each character
 *  of the input, gets that character's associated encoded value and then uses a
 *  an output bitstream to print its representatino.
 */
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    int ch;
    string encoding = "";
    while((ch = input.get()) != EOF) {
        encoding += encodingMap[ch];
    }
    encoding += encodingMap[PSEUDO_EOF];
    for(int i = 0; i < encoding.length(); i++) {
        string digit = encoding.substr(i, 1);
        output.writeBit(stringToInteger(digit));
    }
}

/*
 *  Method: decodeData
 *  Parameters: HuffmanNode for which to evaluate
 *              ostream output by reference
 *              String of remaining bits to decode
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  Recursive function that, given a specific node in an
 *  encoding tree, determines whether it is a leaf of the
 *  tree and prints out its corresponding decoded value if so.
 *  If the affirmative base case fails, the function moves down
 *  the string of bits needed to decode, which is tantamount
 *  to moving down the encoding tree to find the next leaf.
 */
void decodeData(HuffmanNode* node, ostream& output, string& bitsToDecode) {
    if(node != NULL) {
        if(node->character == PSEUDO_EOF) {
            bitsToDecode = "";
        } else if(node->character != NOT_A_CHAR) {
            output.put(node->character);
        } else {
            if(bitsToDecode.length() == 0) return;
            string digit = bitsToDecode.substr(0, 1);
            int nextBit = stringToInteger(digit);
            bitsToDecode = (bitsToDecode.length() > 1 ? bitsToDecode.substr(1) : "");
            HuffmanNode* nextNode = (nextBit == 0 ? node->zero : node->one);
            decodeData(nextNode, output, bitsToDecode);
        }
    }
}

/*
 *  Method: decodeData
 *  Parameters: ibitstream input by reference
 *              HuffmanNode for which to evaluate
 *              ostream output by reference
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  Wrapper function that creates a list of bits to decode
 *  from the ibitstream and then continually runs
 *  its recursive counterpart until all bits have been
 *  decoded and the input's original characters have been printed.
 */
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    int bit;
    string bitsToDecode = "";
    while((bit = input.readBit()) != EOF) {
        bitsToDecode += integerToString(bit);   // Was having problems keeping track of
    }                                           // position in bitstream with .readBit()
    while(!bitsToDecode.empty()) {              // Slow decompression on large files likely result
        decodeData(encodingTree, output, bitsToDecode);
    }
}

/*
 *  Method: compress
 *  Parameters: istream input by reference
 *              obitstream output by reference
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  Compresses a file from its original form to a binary representation.
 *  Function basically wraps up all the previous helper
 *  functions into a single operation. Given an input,
 *  it:
 *    - tabulates the number of times a character occurs in
 *      the file using a helper function
 *    - creates an encoding tree based on the character frequencies
 *    - maps all characters to their binary representations, 
 *      freeing the memory allocated from the encoding tree no longer needed
 *    - encodes the input file into bits using what it did previously
 *  It also outputs the frequency table created as a header so to be used
 *  by the decompress function without explictly passing the needed encoding tree.
 */
void compress(istream& input, obitstream& output) {
    Map<int, int> freqTable = buildFrequencyTable(input);
    output << freqTable;
    HuffmanNode* node = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(node);
    freeTree(node);
    input.clear();
    input.seekg(0, ios::beg);
    encodeData(input, encodingMap, output);
}

/*
 *  Method: decompress
 *  Parameters: ibitstream input by reference
 *              ostream output by reference
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  Takes a binary representation of a file and reverts it
 *  to its original form. It essentially does the opposite
 *  of the compress function by leveraging the character
 *  frequency table outputted to the header by compress and:
 *    - recreating the associated encoding tree
 *    - decoding the data by recursively checking a file's
 *      characters for their binary representations
 *    - outputting the original file, freeing any intermediate
 *      memory as it does
 */
void decompress(ibitstream& input, ostream& output) {
    Map<int, int> freqTable;
    input >> freqTable;
    HuffmanNode* node = buildEncodingTree(freqTable);
    decodeData(input, node, output);
    freeTree(node);
}

/*
 *  Method: freeTree
 *  Parameters: HuffmanNode to be deleted
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - 
 *  Recursively frees all memory associated with the passed pointer variable.
 */
void freeTree(HuffmanNode* node) {
    if(node != NULL) {
        freeTree(node->zero);
        freeTree(node->one);
        delete node;
    }
}
