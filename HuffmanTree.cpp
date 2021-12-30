#include "HuffmanTree.h"
#include "PriorityQueue.h"

HuffmanTree::Node::Node(const uint32_t val)
{
    is_terminal_ = true;
    value_ = val;
}

HuffmanTree::Node::Node(HuffmanTree::Node* lhs, HuffmanTree::Node* rhs)
{
    left_ = lhs;
    right_ = rhs;
}

HuffmanTree::Node::~Node()
{
    delete left_;
    delete right_;
}

void HuffmanTree::Node::AddValue(const uint32_t val, const std::vector<bool>& edges, size_t pos)
{
    if (pos == edges.size())
    {
        value_ = val;
        is_terminal_ = true;
    } else
    {
        if (static_cast<uint32_t>(edges[pos]) == huffman_tree::LEFT)
        {
            if (left_ == nullptr)
            {
                left_ = new Node();
            }
            left_->AddValue(val, edges, pos + 1);
        } else
        {
            if (right_ == nullptr)
            {
                right_ = new Node();
            }
            right_->AddValue(val, edges, pos + 1);
        }
    }
}

HuffmanTree::Node* HuffmanTree::Node::Move(const uint32_t edge)
{
    auto* node = this;
    if (edge == huffman_tree::LEFT)
    {
        node = node->left_;
    } else
    {
        node = node->right_;
    }

    return node;
}

HuffmanTree::Node* HuffmanTree::BuildTree(const std::unordered_map<uint32_t, uint32_t>& symbols_frequency)
{
    PriorityQueue<std::pair<int, Node*>, std::less<>> priority_queue;

    for (const auto& kvp : symbols_frequency)
    {
        priority_queue.Push({kvp.second, new Node(kvp.first)});
    }

    while (priority_queue.Size() > 1)
    {
        auto lhs = priority_queue.Top();
        priority_queue.Pop();
        auto rhs = priority_queue.Top();
        priority_queue.Pop();

        priority_queue.Push({lhs.first + rhs.first, new Node(lhs.second, rhs.second)});
    }

    return priority_queue.Top().second;
}

unsigned int HuffmanTree::Node::GetValue()
{
    return value_;
}

bool HuffmanTree::Node::IsTerminal()
{
    return is_terminal_;
}

HuffmanTree::Node* HuffmanTree::Node::GetLeft()
{
    return left_;
}

HuffmanTree::Node* HuffmanTree::Node::GetRight()
{
    return right_;
}