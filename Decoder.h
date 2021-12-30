#pragma once

#include "BitReader.h"
#include "BitWriter.h"
#include "HuffmanTree.h"

#include <cstdint>
#include <iostream>
#include <string>

class Decoder
{
public:
    static void DecodeArchive(std::istream& input_stream);

    static std::vector<huffman_tree::huffman_code> GetSymbolsCode(BitReader& bit_reader, std::istream& input_stream);

    static HuffmanTree::Node* BuildDecodeTree(const std::vector<huffman_tree::huffman_code>& symbols_code);

protected:
    static std::string GetOutputFileName(BitReader& bit_reader, std::istream& input_stream,
                                         HuffmanTree::Node* decode_tree_root);

    static bool DecodeFileContent(BitReader& bit_reader, BitWriter& bit_writer, std::istream& input_stream,
                                  std::ostream& output_stream, HuffmanTree::Node* decode_tree_root);
};
