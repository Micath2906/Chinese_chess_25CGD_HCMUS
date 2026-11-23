#pragma once
#include <random>

/**
 * @file TurnOrderRandomizer.h
 * @brief Đóng gói thuật toán xác định lượt đi ngẫu nhiên bằng std::mt19937.
 * 
 * Hỗ trợ truyền seed tùy chọn phục vụ kiểm thử đơn vị tái lập được (deterministic test),
 * tách biệt hoàn toàn logic ngẫu nhiên khỏi giao diện đồ họa.
 */

enum class TurnResult {
    PLAYER1_RED, // Player 1 (hoặc Người chơi trong PvAI) đi trước (cầm quân Đỏ)
    PLAYER2_RED  // Player 2 (hoặc Máy AI trong PvAI) đi trước (cầm quân Đỏ)
};

class TurnOrderRandomizer {
public:
    TurnOrderRandomizer()
        : rng(std::random_device{}()) {}

    explicit TurnOrderRandomizer(unsigned int seed)
        : rng(seed) {}

    void setSeed(unsigned int seed) {
        rng.seed(seed);
    }

    TurnResult flip() {
        std::uniform_int_distribution<int> dist(0, 1);
        return (dist(rng) == 0) ? TurnResult::PLAYER1_RED : TurnResult::PLAYER2_RED;
    }

private:
    std::mt19937 rng;
};
