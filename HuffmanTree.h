#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace huffman_tree
{
    using huffman_code = std::pair<uint32_t, std::vector<bool>>;
    const uint32_t LEFT = 0;
    const uint32_t RIGHT = 1;
}

class HuffmanTree
{
public:
    class Node
    {
    public:
        Node() = default;

        Node(Node* lhs, Node* rhs);

        explicit Node(uint32_t val);

        ~Node();

        void AddValue(uint32_t val, const std::vector<bool>& edges, size_t pos);
        Node* Move(uint32_t edge);

        bool IsTerminal();
        unsigned int GetValue();

        Node* GetLeft();
        Node* GetRight();

    private:
        Node* left_ = nullptr;
        Node* right_ = nullptr;
        bool is_terminal_ = false;
        unsigned int value_ = 0;
    };

public:
    static Node* BuildTree(const std::unordered_map<uint32_t, uint32_t>& symbols_frequency);
};
