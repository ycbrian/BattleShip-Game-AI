#pragma once

#include <vector>

namespace TA
{
    class Board
    {
    public:
        enum class State
        {
            Unknown,
            Empty,
            Hit
        };
        Board(int size)
            :m_size(size)
            ,m_board(size, std::vector<State>(size, State::Unknown))
        {}

        int size() const { return m_size;  }
        auto& operator[](int x) { return m_board[x]; }
        const auto& operator[](int x) const { return m_board[x]; }

    private:
        int m_size;
        std::vector<std::vector<State>> m_board;
    };
} // Namespace TA
