#pragma once
#include "include.h"

using value_type = int;

struct Cave {
    using value_type = int;
    using data_type = std::vector<value_type>;

    data_type data;
    value_type rows = 0, cols = 0;

    int& operator()(std::size_t row, std::size_t col) {
        return data[row * cols + col];
    }

    int  operator()(std::size_t row, std::size_t col) const {
        if (row >= rows or col >= cols)
            return 1;
        return data[row * cols + col];
    }
};

struct GenerationSettings final {
    value_type rows, cols, live_chance, generation_count;
    std::pair <value_type, value_type> live_limit, born_limit;
};

value_type CountLiveNeighbours(value_type row, value_type col, const Cave& cave);

Cave InitializeRandom(value_type rows, value_type cols, value_type live_chance);
Cave GenerateCave(const GenerationSettings& s);