#pragma once

#include "BitWriter.h"
#include "HuffmanTree.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

class Encoder
{
public:
    static void EncodeArchive(const std::vector<std::string>& input_stream_names, std::ostream& out);

    static std::unordered_map<uint32_t, uint32_t> GetSymbolsFrequency(const std::string& stream_name,
                                                                      std::istream& input_stream);

    static std::unordered_map<uint32_t, uint32_t> GetSymbolsCode(
            const std::unordered_map<uint32_t, uint32_t>& symbols_frequency);

    static std::vector<huffman_tree::huffman_code> CanonizeSymbolsCode(
            const std::unordered_map<uint32_t, uint32_t>& symbols_code);

protected:
    static void EncodeStream(BitWriter& bit_writer, const std::string& stream_name, std::istream& input_stream,
                             std::ostream& output_stream, bool is_last_stream,
                             const std::vector<huffman_tree::huffman_code>& symbols_canonical_code);

    static void EncodeMetaInformation(BitWriter& bit_writer, std::ostream& output_stream,
                                      const std::vector<huffman_tree::huffman_code>& symbols_canonical_code);

    static void EncodeStreamName(BitWriter& bit_writer, const std::string& stream_name, std::ostream& output_stream,
                                 const std::unordered_map<uint32_t, std::vector<bool>>& symbols_code);

    static void EncodeStreamContent(BitWriter& bit_writer, std::istream& input_stream, std::ostream& output_stream,
                                    const std::unordered_map<uint32_t, std::vector<bool>>& symbols_code);
};
