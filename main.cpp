#include "GameManager.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

static void ensureWorkingDirectory() {
    if (!fs::exists("assets")) {
        if (fs::exists("../assets")) {
            fs::current_path("..");
        } else if (fs::exists("../../assets")) {
            fs::current_path("../..");
        } else if (fs::exists("../../../assets")) {
            fs::current_path("../../..");
        }
    }
}

int main() {
    try {
        ensureWorkingDirectory();
        GameManager game;
        game.chay();
    }
    catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
