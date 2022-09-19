#pragma once
#include "include.h"

template<class... Args>
class Action {
public:
    Action() {
        m_ArgCount = sizeof...(Args);
    }

    int AddListener(std::function<void(Args...)> func) {
        m_delegats.push_back(func);
        return m_delegats.size() - 1;
    }
    void PopListener(int ind) {
        if (ind < m_delegats.size())
            m_delegats.erase(m_delegats.begin() + ind);
    }


    void Invoke(Args... args) {
        for (auto& i : m_delegats) {
            i(args...);
        }
    }

private:
    std::vector<std::function<void(Args...)>> m_delegats;
    int m_ArgCount;
};