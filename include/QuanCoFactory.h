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

// Factory method
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
