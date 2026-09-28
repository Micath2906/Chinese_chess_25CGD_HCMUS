#include "GameManager.h"
#include <iostream>

int main() {
    try {
        GameManager game;
        game.chay();
    }
    catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
