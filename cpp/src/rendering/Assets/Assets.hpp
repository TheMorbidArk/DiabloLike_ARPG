#pragma once

#include "../../core/Config.hpp"
#include "../../tic80/Tic80.hpp"
#include <array>

namespace rendering {

class Assets {
public:
    static auto get() -> Assets& {
        static Assets instance;
        return instance;
    }

    Assets(const Assets&) = delete;
    auto operator=(const Assets&) -> Assets& = delete;

    void init() {
    }

private:
    Assets() = default;
};

}
