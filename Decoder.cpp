#include "Decoder.h"
#include "BitReader.h"
#include "BitWorker.h"
#include "BitWriter.h"
#include "ServiceSymbols.h"

#include <exception>
#include <fstream>
#include <vector>

void Decoder::DecodeArchive(std::istream& input_stream)
{
    BitReader bit_reader;

    bool is_archive_over = false;

    while (!is_archive_over)
    {

        auto symbols_code = GetSymbolsCode(bit_reader, input_stream);
        auto* decode_tree_root = BuildDecodeTree(symbols_code);

        BitWriter bit_writer;
        auto output_file_name = GetOutputFileName(bit_reader, input_stream, decode_tree_root);
        std::ofstream output_stream(output_file_name, std::ios::binary | std::ios::out);

        if (!output_stream)
        {
            std::string error_msg = "Error opening " + output_file_name;
            delete decode_tree_root;
            throw std::runtime_error(error_msg);
        }

        is_archive_over = DecodeFileContent(bit_reader, bit_writer,
                                            input_stream, output_stream, decode_tree_root);
        bit_writer.WriteAll(output_stream);

        output_stream.close();
        delete decode_tree_root;
    }
}

std::vector<huffman_tree::huffman_code> Decoder::GetSymbolsCode(BitReader& bit_reader, std::istream& input_stream)
{

    uint32_t symbols_count = bit_reader.ReadNBits(input_stream, bit_values::NINE_BITS);

    std::vector<huffman_tree::huffman_code> symbols_code(symbols_count);
    std::vector<uint32_t> code_len_cnt;

    for (size_t i = 0; i < symbols_count; ++i)
    {
        symbols_code[i].first = bit_reader.ReadNBits(input_stream, bit_values::NINE_BITS);
    }

    uint32_t cur_len = 1;
    while (code_len_cnt.size() < symbols_count)
    {
        uint32_t symbols_cnt = bit_reader.ReadNBits(input_stream, bit_values::NINE_BITS);
        for (size_t i = 0; i < symbols_cnt; ++i)
        {
            code_len_cnt.push_back(cur_len);
        }
        ++cur_len;
    }

    for (size_t i = 0; i < symbols_code.size(); ++i)
    {
        if (i == 0)
        {
            symbols_code[i].second = std::vector<bool>(code_len_cnt[i], false);
        } else
        {
            symbols_code[i].second = std::vector<bool>(code_len_cnt[i - 1], false);

            bool next_place = true;
            auto& cur_code = symbols_code[i].second;
            auto& prev_code = symbols_code[i - 1].second;

            for (size_t j = cur_code.size(); j > 0; --j)
            {
                int temp = static_cast<int>(next_place) + static_cast<int>(prev_code[j - 1]);
                cur_code[j - 1] = static_cast<bool>(temp % 2);
                next_place = static_cast<bool>(temp / 2);
            }
            for (size_t j = 0; j < code_len_cnt[i] - code_len_cnt[i - 1]; ++j)
            {
                cur_code.push_back(false);
            }
        }
    }

    return symbols_code;
}

HuffmanTree::Node* Decoder::BuildDecodeTree(const std::vector<huffman_tree::huffman_code>& symbols_code)
{
    auto* decode_tree_root = new HuffmanTree::Node();

    for (auto& [symbol, code] : symbols_code)
    {
        decode_tree_root->AddValue(symbol, code, 0);
    }
    return decode_tree_root;
}

std::string Decoder::GetOutputFileName(BitReader& bit_reader, std::istream& input_stream,
                                       HuffmanTree::Node* decode_tree_root)
{
    auto* current_node = decode_tree_root;

    bool is_file_name_over = false;
    std::string output_file_name;

    while (!is_file_name_over)
    {
        uint32_t next_bit = bit_reader.ReadNBits(input_stream, bit_values::ONE_BIT);
        current_node = current_node->Move(next_bit);

        if (current_node->IsTerminal())
        {
            uint32_t cur_value = current_node->GetValue();
            current_node = decode_tree_root;

            if (cur_value == service_symbols::FILENAME_END)
            {
                is_file_name_over = true;
            } else
            {
                output_file_name.push_back(cur_value);
            }
        }
    }

    {
        size_t last_directory_end = output_file_name.rfind('/');
        if (last_directory_end != std::string::npos)
        {
            output_file_name = output_file_name.substr(last_directory_end + 1);
        }
    }

    return output_file_name;
}

bool Decoder::DecodeFileContent(BitReader& bit_reader, BitWriter& bit_writer, std::istream& input_stream,
                                std::ostream& output_stream, HuffmanTree::Node* decode_tree_root)
{
    bool is_file_content_over = false;
    bool is_archive_over = false;
    auto* current_node = decode_tree_root;

    while (!is_file_content_over)
    {
        uint32_t next_bit = bit_reader.ReadNBits(input_stream, bit_values::ONE_BIT);
        current_node = current_node->Move(next_bit);

        if (current_node->IsTerminal())
        {
            uint32_t cur_value = current_node->GetValue();
            current_node = decode_tree_root;
            if (cur_value == service_symbols::ONE_MORE_FILE)
            {
                is_file_content_over = true;
            } else if (cur_value == service_symbols::ARCHIVE_END)
            {
                is_file_content_over = true;
                is_archive_over = true;
            } else
            {
                bit_writer.WriteUInt(output_stream, cur_value);
            }
        }
    }

    return is_archive_over;
}
