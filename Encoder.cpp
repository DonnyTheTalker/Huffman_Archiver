#include "Encoder.h"
#include "BitWorker.h"
#include "BitWriter.h"
#include "HuffmanTree.h"
#include "ServiceSymbols.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <queue>

void Encoder::EncodeArchive(const std::vector<std::string>& input_stream_names, std::ostream& out_stream)
{
    BitWriter bit_writer;

    for (size_t i = 0; i < input_stream_names.size(); ++i)
    {
        std::ifstream cur_stream(input_stream_names[i], std::ios::binary | std::ios::in);

        if (!cur_stream)
        {
            std::string error_msg = "Error opening " + input_stream_names[i];
            throw std::runtime_error(error_msg);
        }

        auto symbols_frequency = GetSymbolsFrequency(input_stream_names[i], cur_stream);
        auto symbols_code = GetSymbolsCode(symbols_frequency);
        auto symbols_canonical_code = CanonizeSymbolsCode(symbols_code);

        cur_stream.clear();
        cur_stream.seekg(0);

        EncodeStream(bit_writer, input_stream_names[i], cur_stream,
                     out_stream, ((i + 1) == input_stream_names.size()),
                     symbols_canonical_code);

        cur_stream.close();
    }
}

std::unordered_map<uint32_t, uint32_t> Encoder::GetSymbolsFrequency(const std::string& stream_name,
                                                                    std::istream& input_stream)
{
    std::unordered_map<uint32_t, uint32_t> symbols_frequency;

    symbols_frequency[service_symbols::ARCHIVE_END] = 1;
    symbols_frequency[service_symbols::FILENAME_END] = 1;
    symbols_frequency[service_symbols::ONE_MORE_FILE] = 1;

    for (const char& ch : stream_name)
    {
        ++symbols_frequency[static_cast<uint32_t>(static_cast<unsigned char>(ch))];
    }

    unsigned char ch;
    while (input_stream >> std::noskipws >> ch)
    {
        ++symbols_frequency[static_cast<uint32_t>(ch)];
    }

    return symbols_frequency;
}

std::unordered_map<uint32_t, uint32_t> Encoder::GetSymbolsCode(
        const std::unordered_map<uint32_t, uint32_t>& symbols_frequency)
{
    std::unordered_map<uint32_t, uint32_t> symbols_code;

    HuffmanTree::Node* encode_tree_root = HuffmanTree::BuildTree(symbols_frequency);

    std::queue<std::pair<HuffmanTree::Node*, uint32_t>> symbols_len;
    symbols_len.push({encode_tree_root, 0});

    while (!symbols_len.empty())
    {
        auto* node = symbols_len.front().first;
        auto len = symbols_len.front().second;
        symbols_len.pop();

        if (node->IsTerminal())
        {
            symbols_code[node->GetValue()] = len;
        } else
        {
            symbols_len.push({node->GetLeft(), len + 1});
            symbols_len.push({node->GetRight(), len + 1});
        }
    }

    delete encode_tree_root;
    return symbols_code;
}

std::vector<huffman_tree::huffman_code> Encoder::CanonizeSymbolsCode(
        const std::unordered_map<uint32_t, uint32_t>& symbols_code)
{
    std::vector<huffman_tree::huffman_code> symbols_canonical_code;
    std::vector<std::pair<uint32_t, uint32_t>> symbols_code_len;

    symbols_canonical_code.reserve(symbols_code.size());
    symbols_code_len.reserve(symbols_code.size());

    for (const auto& kvp : symbols_code)
    {
        symbols_code_len.emplace_back(kvp.first, kvp.second);
    }

    std::sort(symbols_code_len.begin(), symbols_code_len.end(), [](const auto& lhs, const auto& rhs)
    {
        return lhs.second < rhs.second || (lhs.second == rhs.second && lhs.first < rhs.first);
    });

    for (size_t i = 0; i < symbols_code_len.size(); ++i)
    {
        if (i == 0)
        {
            symbols_canonical_code.emplace_back(symbols_code_len[i].first,
                                                std::vector<bool>(symbols_code_len[i].second, false));
        } else
        {
            bool next_place = true;
            symbols_canonical_code.emplace_back(symbols_code_len[i].first,
                                                std::vector<bool>(symbols_code_len[i - 1].second, false));

            auto& cur_code = symbols_canonical_code[i].second;
            auto& prev_code = symbols_canonical_code[i - 1].second;

            for (size_t j = cur_code.size(); j > 0; --j)
            {
                int temp = static_cast<int>(next_place) + static_cast<int>(prev_code[j - 1]);
                cur_code[j - 1] = static_cast<bool>(temp % 2);
                next_place = static_cast<bool>(temp / 2);
            }

            for (size_t j = 0; j < symbols_code_len[i].second - symbols_code_len[i - 1].second; ++j)
            {
                cur_code.push_back(false);
            }
        }
    }

    return symbols_canonical_code;
}

void Encoder::EncodeStream(BitWriter& bit_writer, const std::string& stream_name, std::istream& input_stream,
                           std::ostream& output_stream, bool is_last_stream,
                           const std::vector<huffman_tree::huffman_code>& symbols_canonical_code)
{

    std::unordered_map<uint32_t, std::vector<bool>> symbols_code;
    for (const auto& [symbol, code] : symbols_canonical_code)
    {
        symbols_code[symbol] = code;
    }

    EncodeMetaInformation(bit_writer, output_stream, symbols_canonical_code);
    EncodeStreamName(bit_writer, stream_name, output_stream, symbols_code);
    EncodeStreamContent(bit_writer, input_stream, output_stream, symbols_code);

    if (is_last_stream)
    {
        bit_writer.WriteBits(output_stream, symbols_code[service_symbols::ARCHIVE_END]);
        bit_writer.WriteAll(output_stream);
    } else
    {
        bit_writer.WriteBits(output_stream, symbols_code[service_symbols::ONE_MORE_FILE]);
    }
}

void Encoder::EncodeMetaInformation(BitWriter& bit_writer, std::ostream& output_stream,
                                    const std::vector<huffman_tree::huffman_code>& symbols_canonical_code)
{
    bit_writer.WriteNBits(output_stream, symbols_canonical_code.size(), bit_values::NINE_BITS);
    for (const auto& [symbol, code] : symbols_canonical_code)
    {
        bit_writer.WriteNBits(output_stream, symbol, bit_values::NINE_BITS);
    }

    size_t max_symbol_code_size = 0;
    std::unordered_map<size_t, uint32_t> symbols_code_size;
    for (const auto& [symbol, code] : symbols_canonical_code)
    {
        ++symbols_code_size[code.size()];
        max_symbol_code_size = std::max(max_symbol_code_size, code.size());
    }
    for (size_t i = 1; i <= max_symbol_code_size; ++i)
    {
        bit_writer.WriteNBits(output_stream, symbols_code_size[i], bit_values::NINE_BITS);
    }
}

void Encoder::EncodeStreamName(BitWriter& bit_writer, const std::string& stream_name, std::ostream& output_stream,
                               const std::unordered_map<uint32_t, std::vector<bool>>& symbols_code)
{
    for (const char& ch : stream_name)
    {
        bit_writer.WriteBits(output_stream, symbols_code.at(static_cast<uint32_t>(static_cast<unsigned char>(ch))));
    }

    bit_writer.WriteBits(output_stream, symbols_code.at(service_symbols::FILENAME_END));
}

void Encoder::EncodeStreamContent(BitWriter& bit_writer, std::istream& input_stream, std::ostream& output_stream,
                                  const std::unordered_map<uint32_t, std::vector<bool>>& symbols_code)
{
    unsigned char ch;
    while (input_stream >> std::noskipws >> ch)
    {
        bit_writer.WriteBits(output_stream, symbols_code.at(static_cast<uint32_t>(ch)));
    }
}
