#include "GameManager.h"
#include "AssetManager.h"
#include "TurnOrderRandomizer.h"
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

            // Unit Test: TurnOrderRandomizer
            std::cout << "[TEST] Running TurnOrderRandomizer unit test...\n";
            TurnOrderRandomizer t1(42);
            TurnOrderRandomizer t2(42);
            bool seedMatch = true;
            for (int i = 0; i < 20; ++i) {
                if (t1.flip() != t2.flip()) {
                    seedMatch = false;
                    break;
                }
            }
            if (!seedMatch) {
                std::cerr << "[FAIL] Deterministic seed test failed!\n";
                return 1;
            }

            TurnOrderRandomizer rngTest;
            int countP1 = 0;
            const int N = 1000;
            for (int i = 0; i < N; ++i) {
                if (rngTest.flip() == TurnResult::PLAYER1_RED) {
                    countP1++;
                }
            }
            double ratioP1 = (static_cast<double>(countP1) / N) * 100.0;
            double ratioP2 = 100.0 - ratioP1;
            std::cout << "  - Deterministic seed test: PASSED\n";
            std::cout << "  - 1000 trials distribution: Red " << ratioP1 << "% / Black " << ratioP2 << "%\n";
            if (ratioP1 < 40.0 || ratioP1 > 60.0) {
                std::cerr << "[FAIL] Distribution test failed! Out of expected [40%, 60%] range.\n";
                return 1;
            }
            std::cout << "[OK] TurnOrderRandomizer unit test passed successfully.\n";
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
