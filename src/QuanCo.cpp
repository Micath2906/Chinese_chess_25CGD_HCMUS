#include "QuanCo.h"
#include "BanCo.h"

QuanCo::QuanCo(int hang, int cot, Mau mau) 
    : hang(hang), cot(cot), mau(mau), daBiAn(false) {
}

void QuanCo::datViTri(int hangMoi, int cotMoi) {
    hang = hangMoi;
    cot = cotMoi;
}

void QuanCo::ve(sf::RenderWindow& window, float x, float y, float kichThuoc) const {
    // Ve hinh tron quan co
    sf::CircleShape circle(kichThuoc * 0.4f);
    circle.setPosition(x - kichThuoc * 0.4f, y - kichThuoc * 0.4f);
    
    if (mau == Mau::DO) {
        circle.setFillColor(sf::Color(220, 80, 80));
        circle.setOutlineColor(sf::Color(180, 40, 40));
    } else {
        circle.setFillColor(sf::Color(60, 60, 60));
        circle.setOutlineColor(sf::Color(20, 20, 20));
    }
    circle.setOutlineThickness(3.0f);
    
    window.draw(circle);
    
    // Ve chu tren quan co (se duoc override trong cac lop con neu can)
}
