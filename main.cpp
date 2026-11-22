#include "GameManager.h"
#include "AssetManager.h"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    try {
        AssetManager::get().init();

        bool checkOnly = false;
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--check-assets") {
                checkOnly = true;
                break;
            }
        }

        std::vector<std::string> missingAssets;
        if (!AssetManager::get().validateAll(missingAssets)) {
            std::cerr << "====================================================\n";
            std::cerr << "CRITICAL ERROR: Missing required game assets:\n";
            for (const auto& item : missingAssets) {
                std::cerr << "  - " << item << "\n";
            }
            std::cerr << "Please ensure the assets/ directory is present.\n";
            std::cerr << "====================================================\n";
            return 1;
        }

        if (checkOnly) {
            std::cout << "[OK] All required assets are present and validated successfully.\n";
            return 0;
        }

        GameManager game;
        game.chay();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
