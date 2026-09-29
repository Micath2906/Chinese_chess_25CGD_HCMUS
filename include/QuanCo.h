#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class BanCo;

enum class Mau {
    DO,
    DEN,
    HOA
};


class QuanCo {
protected:
    int hang;
    int cot;
    Mau mau;
    bool daBiAn;
    
public:
    QuanCo(int hang, int cot, Mau mau);
    virtual ~QuanCo() = default;
    
    // Getters
    int getHang() const { return hang; }
    int getCot() const { return cot; }
    Mau getMau() const { return mau; }
    bool getDaBiAn() const { return daBiAn; }
    
    // Setters
    void datViTri(int hangMoi, int cotMoi);
    void datDaBiAn(bool trangThai) { daBiAn = trangThai; }
    
    // Pure virtual functions
    virtual bool kiemTraNuocDi(const BanCo& banCo, int hangMoi, int cotMoi) const = 0;
    virtual std::string layTen() const = 0;
    virtual std::string layKyHieu() const = 0;
    
    // Virtual functions
    virtual void ve(sf::RenderWindow& window, float x, float y, float kichThuoc) const;
};
