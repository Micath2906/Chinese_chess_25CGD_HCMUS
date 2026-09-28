#include "GameManager.h"
#include <iostream>

GameManager::GameManager() 
    : window(sf::VideoMode(1200, 800), "Co Tuong - Chinese Chess", sf::Style::Close),
      trangThai(TrangThai::MENU_CHINH),
      quanDangChon(nullptr),
      amThanhBat(true) {
    
    window.setFramerateLimit(60);
    khoiTao();
}

void GameManager::khoiTao() {
    // Load font
    if (!font.loadFromFile("resources/fonts/arial.ttf")) {
        std::cerr << "Khong the load font!" << std::endl;
        // Fallback: use system font or create placeholder
    }
    
    // Tinh toan kich thuoc ban co
    kichThuocO = 70.0f;
    khoangCachBanCo = 10.0f;
    offsetX = 150.0f;
    offsetY = 50.0f;
    
    khoiTaoMenu();
    khoiTaoAmThanh();
}

void GameManager::khoiTaoMenu() {
    menuChinh.loadFont("resources/fonts/arial.ttf");
    
    float buttonWidth = 300.0f;
    float buttonHeight = 60.0f;
    float startX = (window.getSize().x - buttonWidth) / 2.0f;
    float startY = 250.0f;
    float spacing = 80.0f;
    
    menuChinh.themButton(startX, startY, buttonWidth, buttonHeight,
        "CHOI 2 NGUOI", [this]() { batDauTroChoiMoi(true); });
    
    menuChinh.themButton(startX, startY + spacing, buttonWidth, buttonHeight,
        "CHOI VOI MAY", [this]() { batDauTroChoiMoi(false); });
    
    menuChinh.themButton(startX, startY + spacing * 2, buttonWidth, buttonHeight,
        "CAI DAT", [this]() { trangThai = TrangThai::SETTINGS; });
    
    menuChinh.themButton(startX, startY + spacing * 3, buttonWidth, buttonHeight,
        "THOAT", [this]() { window.close(); });
    
    // Settings menu
    menuSettings.loadFont("resources/fonts/arial.ttf");
    menuSettings.themButton(startX, startY, buttonWidth, buttonHeight,
        "BAT/TAT AM THANH", [this]() { amThanhBat = !amThanhBat; });
    
    menuSettings.themButton(startX, startY + spacing * 3, buttonWidth, buttonHeight,
        "QUAY LAI", [this]() { trangThai = TrangThai::MENU_CHINH; });
}

void GameManager::khoiTaoAmThanh() {
    // Load sound buffers (placeholder - can be implemented later)
    // bufferDiChuyen.loadFromFile("resources/sounds/move.wav");
    // bufferAnQuan.loadFromFile("resources/sounds/capture.wav");
}

void GameManager::chay() {
    while (window.isOpen()) {
        xuLySuKien();
        ve();
    }
}

void GameManager::xuLySuKien() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                xuLyClickChuot(mousePos);
            }
        }
        
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                if (trangThai == TrangThai::DANG_CHOI) {
                    quayLaiMenu();
                }
            }
            
            if (event.key.code == sf::Keyboard::Z && 
                sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
                if (trangThai == TrangThai::DANG_CHOI) {
                    banCo.hoanTac();
                    quanDangChon = nullptr;
                    cacNuocDiHopLe.clear();
                }
            }
        }
    }
    
    // Update menu hover states
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (trangThai == TrangThai::MENU_CHINH) {
        menuChinh.update(mousePos);
    } else if (trangThai == TrangThai::SETTINGS) {
        menuSettings.update(mousePos);
    }
}

void GameManager::xuLyClickChuot(const sf::Vector2i& viTri) {
    switch (trangThai) {
        case TrangThai::MENU_CHINH:
            menuChinh.handleClick(viTri);
            break;
            
        case TrangThai::SETTINGS:
            menuSettings.handleClick(viTri);
            break;
            
        case TrangThai::DANG_CHOI: {
            sf::Vector2i viTriBanCo = chuyenDoiToaDoManHinhThanhBanCo(viTri);
            
            if (viTriBanCo.x >= 0 && viTriBanCo.x < BanCo::getSOHANG() &&
                viTriBanCo.y >= 0 && viTriBanCo.y < BanCo::getSOCOT()) {
                
                int hang = viTriBanCo.x;
                int cot = viTriBanCo.y;
                
                if (quanDangChon == nullptr) {
                    chonQuan(hang, cot);
                } else {
                    diChuyenQuan(hang, cot);
                }
            }
            break;
        }
        
        default:
            break;
    }
}

sf::Vector2i GameManager::chuyenDoiToaDoManHinhThanhBanCo(const sf::Vector2i& viTriChuot) {
    int cot = static_cast<int>((viTriChuot.x - offsetX) / kichThuocO);
    int hang = static_cast<int>((viTriChuot.y - offsetY) / kichThuocO);
    
    return sf::Vector2i(hang, cot);
}

sf::Vector2f GameManager::chuyenDoiToaDoBanCoThanhManHinh(int hang, int cot) {
    float x = offsetX + cot * kichThuocO + kichThuocO / 2.0f;
    float y = offsetY + hang * kichThuocO + kichThuocO / 2.0f;
    
    return sf::Vector2f(x, y);
}

void GameManager::ve() {
    window.clear(sf::Color(40, 40, 40));
    
    switch (trangThai) {
        case TrangThai::MENU_CHINH:
            veMenuChinh();
            break;
            
        case TrangThai::SETTINGS:
            veMenuSettings();
            break;
            
        case TrangThai::DANG_CHOI:
            veBanCo();
            veQuanCo();
            veHighlight();
            veThongTin();
            break;
            
        default:
            break;
    }
    
    window.display();
}

void GameManager::veBanCo() {
    // Ve nen ban co
    sf::RectangleShape nenBanCo(sf::Vector2f(
        BanCo::getSOCOT() * kichThuocO + khoangCachBanCo * 2,
        BanCo::getSOHANG() * kichThuocO + khoangCachBanCo * 2
    ));
    nenBanCo.setPosition(offsetX - khoangCachBanCo, offsetY - khoangCachBanCo);
    nenBanCo.setFillColor(sf::Color(139, 90, 43));
    nenBanCo.setOutlineThickness(5);
    nenBanCo.setOutlineColor(sf::Color(101, 67, 33));
    window.draw(nenBanCo);
    
    // Ve cac o
    for (int h = 0; h < BanCo::getSOHANG(); h++) {
        for (int c = 0; c < BanCo::getSOCOT(); c++) {
            sf::RectangleShape o(sf::Vector2f(kichThuocO - 2, kichThuocO - 2));
            o.setPosition(offsetX + c * kichThuocO + 1, offsetY + h * kichThuocO + 1);
            
            if ((h + c) % 2 == 0) {
                o.setFillColor(sf::Color(222, 184, 135));
            } else {
                o.setFillColor(sf::Color(205, 170, 125));
            }
            
            window.draw(o);
        }
    }
    
    // Ve duong song (giua hang 4 va 5)
    sf::RectangleShape duongSong(sf::Vector2f(BanCo::getSOCOT() * kichThuocO, 4));
    duongSong.setPosition(offsetX, offsetY + 4.5f * kichThuocO);
    duongSong.setFillColor(sf::Color(100, 100, 200, 100));
    window.draw(duongSong);
    
    // Ve cung (dinh cua Tuong)
    sf::Color colorCung(180, 0, 0, 100);
    
    // Cung Do
    sf::ConvexShape cungDo;
    cungDo.setPointCount(4);
    cungDo.setPoint(0, chuyenDoiToaDoBanCoThanhManHinh(7, 3));
    cungDo.setPoint(1, chuyenDoiToaDoBanCoThanhManHinh(7, 5));
    cungDo.setPoint(2, chuyenDoiToaDoBanCoThanhManHinh(9, 5));
    cungDo.setPoint(3, chuyenDoiToaDoBanCoThanhManHinh(9, 3));
    cungDo.setFillColor(colorCung);
    window.draw(cungDo);
    
    // Cung Den
    sf::ConvexShape cungDen;
    cungDen.setPointCount(4);
    cungDen.setPoint(0, chuyenDoiToaDoBanCoThanhManHinh(0, 3));
    cungDen.setPoint(1, chuyenDoiToaDoBanCoThanhManHinh(0, 5));
    cungDen.setPoint(2, chuyenDoiToaDoBanCoThanhManHinh(2, 5));
    cungDen.setPoint(3, chuyenDoiToaDoBanCoThanhManHinh(2, 3));
    cungDen.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(cungDen);
}

void GameManager::veQuanCo() {
    for (const auto& quan : banCo.getCacQuan()) {
        if (!quan->getDaBiAn()) {
            sf::Vector2f viTri = chuyenDoiToaDoBanCoThanhManHinh(
                quan->getHang(), quan->getCot()
            );
            
            // Ve hinh tron
            float banKinh = kichThuocO * 0.35f;
            sf::CircleShape circle(banKinh);
            circle.setOrigin(banKinh, banKinh);
            circle.setPosition(viTri);
            
            if (quan->getMau() == Mau::DO) {
                circle.setFillColor(sf::Color(200, 50, 50));
                circle.setOutlineColor(sf::Color(150, 0, 0));
            } else {
                circle.setFillColor(sf::Color(50, 50, 50));
                circle.setOutlineColor(sf::Color(0, 0, 0));
            }
            circle.setOutlineThickness(3);
            
            window.draw(circle);
            
            // Ve chu
            sf::Text text;
            text.setFont(font);
            text.setString(quan->layKyHieu());
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::White);
            text.setStyle(sf::Text::Bold);
            
            sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(textBounds.left + textBounds.width / 2.0f,
                          textBounds.top + textBounds.height / 2.0f);
            text.setPosition(viTri);
            
            window.draw(text);
        }
    }
}

void GameManager::veHighlight() {
    // Highlight quan dang chon
    if (quanDangChon) {
        sf::Vector2f viTri = chuyenDoiToaDoBanCoThanhManHinh(
            quanDangChon->getHang(), quanDangChon->getCot()
        );
        
        sf::CircleShape highlight(kichThuocO * 0.4f);
        highlight.setOrigin(kichThuocO * 0.4f, kichThuocO * 0.4f);
        highlight.setPosition(viTri);
        highlight.setFillColor(sf::Color(255, 255, 0, 80));
        highlight.setOutlineThickness(3);
        highlight.setOutlineColor(sf::Color::Yellow);
        
        window.draw(highlight);
    }
    
    // Highlight cac nuoc di hop le
    for (const auto& nuocDi : cacNuocDiHopLe) {
        sf::Vector2f viTri = chuyenDoiToaDoBanCoThanhManHinh(nuocDi.x, nuocDi.y);
        
        sf::CircleShape dot(kichThuocO * 0.15f);
        dot.setOrigin(kichThuocO * 0.15f, kichThuocO * 0.15f);
        dot.setPosition(viTri);
        dot.setFillColor(sf::Color(0, 255, 0, 150));
        
        window.draw(dot);
    }
}

void GameManager::veThongTin() {
    float infoX = offsetX + BanCo::getSOCOT() * kichThuocO + 50;
    float infoY = 100;
    
    // Thong tin luot choi
    sf::Text luotChoiText;
    luotChoiText.setFont(font);
    luotChoiText.setString(banCo.getLuotChoi() == Mau::DO ? 
        "Luot: DO" : "Luot: DEN");
    luotChoiText.setCharacterSize(28);
    luotChoiText.setFillColor(sf::Color::White);
    luotChoiText.setPosition(infoX, infoY);
    window.draw(luotChoiText);
    
    // Ket thuc van co
    if (banCo.getKetThuc()) {
        sf::Text ketThucText;
        ketThucText.setFont(font);
        ketThucText.setString(banCo.getNguoiThang() == Mau::DO ? 
            "DO THANG!" : "DEN THANG!");
        ketThucText.setCharacterSize(32);
        ketThucText.setFillColor(sf::Color::Yellow);
        ketThucText.setStyle(sf::Text::Bold);
        ketThucText.setPosition(infoX, infoY + 60);
        window.draw(ketThucText);
    }
    
    // Huong dan
    sf::Text huongDan;
    huongDan.setFont(font);
    huongDan.setString("ESC: Menu\nCtrl+Z: Hoan tac");
    huongDan.setCharacterSize(18);
    huongDan.setFillColor(sf::Color(200, 200, 200));
    huongDan.setPosition(infoX, infoY + 150);
    window.draw(huongDan);
}

void GameManager::veMenuChinh() {
    // Title
    sf::Text title;
    title.setFont(font);
    title.setString("CO TUONG");
    title.setCharacterSize(72);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                   titleBounds.top + titleBounds.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 120.0f);
    
    window.draw(title);
    
    menuChinh.draw(window);
}

void GameManager::veMenuSettings() {
    sf::Text title;
    title.setFont(font);
    title.setString("CAI DAT");
    title.setCharacterSize(56);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
                   titleBounds.top + titleBounds.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 120.0f);
    
    window.draw(title);
    
    sf::Text amThanhText;
    amThanhText.setFont(font);
    amThanhText.setString(amThanhBat ? "Am thanh: BAT" : "Am thanh: TAT");
    amThanhText.setCharacterSize(24);
    amThanhText.setFillColor(sf::Color::White);
    amThanhText.setPosition(window.getSize().x / 2.0f - 100, 320);
    window.draw(amThanhText);
    
    menuSettings.draw(window);
}

void GameManager::batDauTroChoiMoi(bool cheDo2Nguoi) {
    banCo.lamMoi();
    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
    trangThai = TrangThai::DANG_CHOI;
}

void GameManager::chonQuan(int hang, int cot) {
    auto quan = banCo.timQuan(hang, cot);
    
    if (quan && quan->getMau() == banCo.getLuotChoi()) {
        quanDangChon = quan;
        tinhCacNuocDiHopLe();
    }
}

void GameManager::diChuyenQuan(int hang, int cot) {
    if (quanDangChon) {
        bool thanhCong = banCo.diChuyen(
            quanDangChon->getHang(), 
            quanDangChon->getCot(),
            hang, 
            cot
        );
        
        if (thanhCong) {
            if (amThanhBat) {
                // Play sound
            }
        }
        
        quanDangChon = nullptr;
        cacNuocDiHopLe.clear();
    }
}

void GameManager::tinhCacNuocDiHopLe() {
    cacNuocDiHopLe.clear();
    
    if (!quanDangChon) return;
    
    for (int h = 0; h < BanCo::getSOHANG(); h++) {
        for (int c = 0; c < BanCo::getSOCOT(); c++) {
            if (quanDangChon->kiemTraNuocDi(banCo, h, c)) {
                cacNuocDiHopLe.push_back(sf::Vector2i(h, c));
            }
        }
    }
}

void GameManager::quayLaiMenu() {
    trangThai = TrangThai::MENU_CHINH;
    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
}
