#pragma once

#include <cstdint>

template <typename T, typename Compare>
class PriorityQueue
{
public:
    void Push(const T& value);
    void Pop();

    T& Top();
    const T& Top() const;

    size_t Size() const;
    bool Empty();

private:
    void SiftDown(size_t pos);
    void SiftUp(size_t pos);

private:
    std::vector<T> heap_;
    Compare comp_;
};

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::Push(const T& value)
{
    heap_.push_back(value);
    SiftUp(heap_.size() - 1);
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::Pop()
{
    heap_.front() = heap_.back();
    heap_.pop_back();
    SiftDown(0);
}

template <typename T, typename Compare>
T& PriorityQueue<T, Compare>::Top()
{
    return heap_.front();
}

template <typename T, typename Compare>
const T& PriorityQueue<T, Compare>::Top() const
{
    return heap_.front();
}

template <typename T, typename Compare>
size_t PriorityQueue<T, Compare>::Size() const
{
    return heap_.size();
}

template <typename T, typename Compare>
bool PriorityQueue<T, Compare>::Empty()
{
    return Size() == 0;
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::SiftDown(size_t pos)
{
    while (pos * 2 + 1 < heap_.size())
    {
        size_t left = pos * 2 + 1;
        size_t right = pos * 2 + 2;
        size_t new_pos = left;

        if (right < heap_.size() && comp_(heap_[right], heap_[left]))
        {
            new_pos = right;
        }
        if (comp_(heap_[pos], heap_[new_pos]))
        {
            break;
        } else
        {
            std::swap(heap_[pos], heap_[new_pos]);
            pos = new_pos;
        }
    }
}

template <typename T, typename Compare>
void PriorityQueue<T, Compare>::SiftUp(size_t pos)
{
    while (pos > 0 && comp_(heap_[pos], heap_[(pos - 1) / 2]))
    {
        std::swap(heap_[pos], heap_[(pos - 1) / 2]);
        pos = (pos - 1) / 2;
    }
}
