#pragma once
#include <memory>
#include <string>
#include "QuanCo.h"
#include "Xe.h"
#include "Ma.h"
#include "Voi.h"
#include "Si.h"
#include "Tuong.h"
#include "Phao.h"
#include "Tot.h"

/**
 * @brief Factory Method Pattern (Mẫu thiết kế Nhà máy)
 * Chuyên trách khởi tạo đối tượng Quân cờ tương ứng dựa trên tên loại quân.
 * Giúp tách biệt hoàn toàn logic khởi tạo khỏi lớp BanCo (tuân thủ Open/Closed Principle).
 */
class QuanCoFactory {
public:
    static std::shared_ptr<QuanCo> taoQuanCo(const std::string& loaiQuan, int hang, int cot, Mau mau) {
        if (loaiQuan == "Xe" || loaiQuan == "X") return std::make_shared<Xe>(hang, cot, mau);
        if (loaiQuan == "Ma" || loaiQuan == "M") return std::make_shared<Ma>(hang, cot, mau);
        if (loaiQuan == "Voi" || loaiQuan == "V") return std::make_shared<Voi>(hang, cot, mau);
        if (loaiQuan == "Si" || loaiQuan == "S") return std::make_shared<Si>(hang, cot, mau);
        if (loaiQuan == "Tuong" || loaiQuan == "T") return std::make_shared<Tuong>(hang, cot, mau);
        if (loaiQuan == "Phao" || loaiQuan == "P") return std::make_shared<Phao>(hang, cot, mau);
        if (loaiQuan == "Tot" || loaiQuan == "t") return std::make_shared<Tot>(hang, cot, mau);
        return nullptr;
    }
};
