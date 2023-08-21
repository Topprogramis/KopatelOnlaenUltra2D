#include"CaveGenerator.h"

value_type CountLiveNeighbours(value_type row, value_type col, const Cave& cave) {
    value_type count = 0;
    for (auto item : { cave(row, col - 1), cave(row, col + 1), cave(row - 1, col), cave(row + 1, col),
                     cave(row - 1, col - 1), cave(row - 1, col + 1), cave(row + 1, col - 1),
                     cave(row + 1, col + 1) })
        if (item != 0)
            count++;
    return count;
}
Cave InitializeRandom(value_type rows, value_type cols, value_type live_chance) {
    std::default_random_engine re(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution <value_type> dist(0, 100);

    // data_type -> std::vector<value_type>
    Cave::data_type generation(rows * cols);
    std::generate(generation.begin(), generation.end(), [&dist, &re, live_chance] {
        value_type chance = dist(re);
        return chance <= live_chance ? 1 : 0;
        });
    return { generation, rows, cols };
}

Cave GenerateCave(const GenerationSettings& s) {
    Cave cave{ InitializeRandom(s.rows, s.cols, s.live_chance) }, tmp = cave;
    value_type generation = 0;
    while (generation++ != s.generation_count) {
        for (value_type row = 0; row != cave.rows; ++row) {
            for (value_type col = 0; col != cave.cols; ++col) {
                value_type count = CountLiveNeighbours(row, col, cave);
                if (cave(row, col) == 1 and (count < s.live_limit.first or count > s.live_limit.second))
                    tmp(row, col) = 0;
                else if (cave(row, col) == 0 and (count >= s.born_limit.first and count <= s.born_limit.second))
                    tmp(row, col) = 1;
            }
        }
        std::copy(tmp.data.begin(), tmp.data.end(), cave.data.begin());
    }
    return cave;
}