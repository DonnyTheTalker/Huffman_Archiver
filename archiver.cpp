#include "BitReader.h"
#include "BitWriter.h"
#include "Decoder.h"
#include "Encoder.h"

#include <exception>
#include <fstream>
#include <string>
#include <vector>

void HelpUser()
{
    std::cout << "Huffman Archiver" << std::endl;
    std::cout << "-h - Use some help" << std::endl;
    std::cout << "-c archive_name file1 [file2 ...] - "
                 "encode file list ('file1', ...) to 'archive_name'" << std::endl;
    std::cout << "-d archive_name - decode 'archive_name' to current directory" << std::endl;
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc == 1)
        {
            throw std::invalid_argument("No command line arguments given");
        } else if (argc == 2 && static_cast<std::string>(argv[1]) == "-h")
        {
            HelpUser();
        } else if (argc > 3 && static_cast<std::string>(argv[1]) == "-c")
        {
            std::string archive_name = argv[2];
            std::vector<std::string> input_file_names;

            for (size_t i = 3; i < static_cast<size_t>(argc); ++i)
            {
                input_file_names.emplace_back(argv[i]);
            }

            std::ofstream out(archive_name, std::ios::binary | std::ios::out);
            if (out)
            {
                Encoder::EncodeArchive(input_file_names, out);
                out.close();
            } else
            {
                throw std::runtime_error("Error opening archive file");
            }
        } else if (argc == 3 && static_cast<std::string>(argv[1]) == "-d")
        {
            std::string archive_name = static_cast<std::string>(argv[2]);
            std::ifstream in(archive_name, std::ios::binary | std::ios::in);

            if (in)
            {
                Decoder::DecodeArchive(in);
                in.close();
            } else
            {
                throw std::runtime_error("Error opening archive file");
            }
        } else
        {
            throw std::invalid_argument("Given invalid command line arguments");
        }
    } catch (const std::runtime_error &ex)
    {
        std::cout << ex.what() << std::endl;
        std::cout << "Command line arguments may be invalid" << std::endl;
    } catch (const std::invalid_argument &ex)
    {
        std::cout << ex.what() << std::endl;
        std::cout << "Try using help ('-h') command" << std::endl;
    }
}