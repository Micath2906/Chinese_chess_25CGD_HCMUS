#include "GameManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

GameManager::GameManager() 
    : window(sf::VideoMode(1200, 800), "Co Tuong - Chinese Chess (OOP HCMUS)", sf::Style::Close | sf::Style::Titlebar),
      trangThai(TrangThai::MENU_CHINH),
      cheDoChoi(CheDoChoi::HAI_NGUOI),
      quanDangChon(nullptr),
      kichThuocO(68.0f),
      offsetX(65.0f),
      offsetY(60.0f),
      doKhoAI(DoKho::TRUNG_BINH),
      mauNguoiChoi(Mau::DO),
      mauAI(Mau::DEN),
      thoiGianVanDau(600.0f), // 10 phut mac dinh
      hienGoiY(true),
      thoiGianConDo(600.0f),
      thoiGianConDen(600.0f),
      aiDangSuyNghi(false),
      thoiGianChoAI(0.0f),
      toastTimer(0.0f),
      lyDoKetThuc("") {
    
    window.setFramerateLimit(60);
    khoiTao();
}

void GameManager::khoiTao() {
    if (!font.loadFromFile("resources/fonts/arial.ttf")) {
        std::cerr << "Khong the load font tai resources/fonts/arial.ttf" << std::endl;
    }
    
    soundManager.khoiTao();
    khoiTaoCacMenu();
    khoiTaoNutTrongGame();
    khoiTaoNutPopup();
}

void GameManager::khoiTaoCacMenu() {
    float bw = 320.0f;
    float bh = 55.0f;
    float cx = (window.getSize().x - bw) / 2.0f;
    float startY = 240.0f;
    float sp = 75.0f;

    // 1. Menu chinh
    menuChinh.setFont(font);
    menuChinh.themButton(cx, startY, bw, bh, "CHOI 2 NGUOI", [this]() {
        soundManager.play(SoundType::CLICK);
        batDauTroChoiMoi(CheDoChoi::HAI_NGUOI);
    });
    menuChinh.themButton(cx, startY + sp, bw, bh, "CHOI VOI MAY (AI)", [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::CHON_PHE_AI;
    });
    menuChinh.themButton(cx, startY + sp * 2, bw, bh, "CAI DAT", [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::SETTINGS;
    });
    menuChinh.themButton(cx, startY + sp * 3, bw, bh, "HUONG DAN", [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::HUONG_DAN;
    });
    menuChinh.themButton(cx, startY + sp * 4, bw, bh, "THOAT", [this]() {
        soundManager.play(SoundType::CLICK);
        window.close();
    });

    // 2. Menu Chon Phe AI
    menuChonPhe.setFont(font);
    menuChonPhe.themButton(cx, startY + 40.0f, bw, bh, "CAM QUAN DO (DI TRUOC)", [this]() {
        soundManager.play(SoundType::CLICK);
        mauNguoiChoi = Mau::DO;
        mauAI = Mau::DEN;
        batDauTroChoiMoi(CheDoChoi::VOI_MAY);
    });
    menuChonPhe.themButton(cx, startY + 40.0f + sp, bw, bh, "CAM QUAN DEN (DI SAU)", [this]() {
        soundManager.play(SoundType::CLICK);
        mauNguoiChoi = Mau::DEN;
        mauAI = Mau::DO;
        batDauTroChoiMoi(CheDoChoi::VOI_MAY);
    });
    menuChonPhe.themButton(cx, startY + 40.0f + sp * 2, bw, bh, "QUAY LAI", [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
    });

    // 3. Menu Settings
    menuSettings.setFont(font);
    menuSettings.themButton(cx, 190.0f, bw, 48.0f, 
        soundManager.getAmThanhBat() ? "AM THANH: BAT" : "AM THANH: TAT", [this]() {
        bool bat = !soundManager.getAmThanhBat();
        soundManager.setAmThanhBat(bat);
        if (auto btn = menuSettings.layButton(0)) {
            btn->setLabel(bat ? "AM THANH: BAT" : "AM THANH: TAT");
        }
        soundManager.play(SoundType::CLICK);
    });

    menuSettings.themButton(cx, 255.0f, bw, 48.0f, 
        "AM LUONG: " + std::to_string(static_cast<int>(soundManager.getAmLuong())) + "%", [this]() {
        float cur = soundManager.getAmLuong();
        float next = (cur >= 100.0f) ? 25.0f : (cur + 25.0f);
        soundManager.setAmLuong(next);
        if (auto btn = menuSettings.layButton(1)) {
            btn->setLabel("AM LUONG: " + std::to_string(static_cast<int>(next)) + "%");
        }
        soundManager.play(SoundType::CLICK);
    });

    auto getTimerLabel = [](float t) {
        if (t <= 0.0f) return std::string("THOI GIAN: VO HAN");
        int m = static_cast<int>(t / 60.0f);
        return "THOI GIAN: " + std::to_string(m) + " PHUT";
    };

    menuSettings.themButton(cx, 320.0f, bw, 48.0f, getTimerLabel(thoiGianVanDau), [this, getTimerLabel]() {
        if (thoiGianVanDau <= 0.0f) thoiGianVanDau = 300.0f; // 5 phut
        else if (thoiGianVanDau == 300.0f) thoiGianVanDau = 600.0f; // 10 phut
        else if (thoiGianVanDau == 600.0f) thoiGianVanDau = 900.0f; // 15 phut
        else thoiGianVanDau = 0.0f; // vo han
        
        if (auto btn = menuSettings.layButton(2)) {
            btn->setLabel(getTimerLabel(thoiGianVanDau));
        }
        soundManager.play(SoundType::CLICK);
    });

    auto getAiLabel = [](DoKho dk) {
        if (dk == DoKho::DE) return std::string("DO KHO AI: DE");
        if (dk == DoKho::TRUNG_BINH) return std::string("DO KHO AI: TRUNG BINH");
        return std::string("DO KHO AI: KHO");
    };

    menuSettings.themButton(cx, 385.0f, bw, 48.0f, getAiLabel(doKhoAI), [this, getAiLabel]() {
        if (doKhoAI == DoKho::DE) doKhoAI = DoKho::TRUNG_BINH;
        else if (doKhoAI == DoKho::TRUNG_BINH) doKhoAI = DoKho::KHO;
        else doKhoAI = DoKho::DE;

        if (auto btn = menuSettings.layButton(3)) {
            btn->setLabel(getAiLabel(doKhoAI));
        }
        soundManager.play(SoundType::CLICK);
    });

    menuSettings.themButton(cx, 450.0f, bw, 48.0f,
        hienGoiY ? "GOI Y NUOC DI: BAT" : "GOI Y NUOC DI: TAT", [this]() {
        hienGoiY = !hienGoiY;
        if (auto btn = menuSettings.layButton(4)) {
            btn->setLabel(hienGoiY ? "GOI Y NUOC DI: BAT" : "GOI Y NUOC DI: TAT");
        }
        soundManager.play(SoundType::CLICK);
    });

    menuSettings.themButton(cx, 550.0f, bw, 52.0f, "QUAY LAI", [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
    });

    // 4. Menu Huong Dan
    menuHuongDan.setFont(font);
    menuHuongDan.themButton((window.getSize().x - 240.0f) / 2.0f, 710.0f, 240.0f, 50.0f, "QUAY LAI", [this]() {
        soundManager.play(SoundType::CLICK);
        trangThai = TrangThai::MENU_CHINH;
    });
}

void GameManager::khoiTaoNutTrongGame() {
    inGameButtons.clear();
    float bx = 680.0f;
    float by = 470.0f;
    float bw = 220.0f;
    float bh = 45.0f;
    float gapX = 245.0f;
    float gapY = 55.0f;

    // Row 1: Hoan tac & Di tiep
    inGameButtons.push_back(std::make_unique<Button>(bx, by, bw, bh, "HOAN TAC (Undo)", font, [this]() {
        thucHienHoanTac();
    }));
    inGameButtons.push_back(std::make_unique<Button>(bx + gapX, by, bw, bh, "DI TIEP (Redo)", font, [this]() {
        thucHienDiTiep();
    }));

    // Row 2: Van moi & Xin hoa
    inGameButtons.push_back(std::make_unique<Button>(bx, by + gapY, bw, bh, "VAN MOI", font, [this]() {
        batDauTroChoiMoi(cheDoChoi);
    }));
    inGameButtons.push_back(std::make_unique<Button>(bx + gapX, by + gapY, bw, bh, "XIN HOA", font, [this]() {
        thucHienXinHoa();
    }));

    // Row 3: Dau hang & Luu game
    inGameButtons.push_back(std::make_unique<Button>(bx, by + gapY * 2, bw, bh, "DAU HANG", font, [this]() {
        thucHienDauHang();
    }));
    inGameButtons.push_back(std::make_unique<Button>(bx + gapX, by + gapY * 2, bw, bh, "LUU GAME", font, [this]() {
        thucHienLuuGame();
    }));

    // Row 4: Load game & Menu
    inGameButtons.push_back(std::make_unique<Button>(bx, by + gapY * 3, bw, bh, "TAI GAME", font, [this]() {
        thucHienLoadGame();
    }));
    inGameButtons.push_back(std::make_unique<Button>(bx + gapX, by + gapY * 3, bw, bh, "VE MENU", font, [this]() {
        quayLaiMenu();
    }));
}

void GameManager::khoiTaoNutPopup() {
    popupButtons.clear();
    float pw = 200.0f;
    float ph = 50.0f;
    float cx = window.getSize().x / 2.0f;
    float py = 450.0f;

    popupButtons.push_back(std::make_unique<Button>(cx - pw - 20.0f, py, pw, ph, "CHOI LAI", font, [this]() {
        soundManager.play(SoundType::CLICK);
        batDauTroChoiMoi(cheDoChoi);
    }));

    popupButtons.push_back(std::make_unique<Button>(cx + 20.0f, py, pw, ph, "VE MENU", font, [this]() {
        soundManager.play(SoundType::CLICK);
        quayLaiMenu();
    }));
}

void GameManager::batDauTroChoiMoi(CheDoChoi cheDo) {
    cheDoChoi = cheDo;
    banCo.lamMoi();
    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
    trangThai = TrangThai::DANG_CHOI;
    
    thoiGianConDo = thoiGianVanDau;
    thoiGianConDen = thoiGianVanDau;
    dtClock.restart();
    
    aiDangSuyNghi = false;
    thoiGianChoAI = 0.0f;
    lyDoKetThuc = "";
    toastTimer = 0.0f;

    soundManager.play(SoundType::MOVE);

    // Neu dau voi may va nguoi choi chon Den (may di truoc)
    if (cheDoChoi == CheDoChoi::VOI_MAY && mauAI == Mau::DO) {
        aiDangSuyNghi = true;
        thoiGianChoAI = 0.5f;
    }
}

void GameManager::chay() {
    while (window.isOpen()) {
        float dt = dtClock.restart().asSeconds();
        
        if (toastTimer > 0.0f) {
            toastTimer -= dt;
        }

        if (trangThai == TrangThai::DANG_CHOI && !banCo.getKetThuc()) {
            capNhatDongHo(dt);
            if (cheDoChoi == CheDoChoi::VOI_MAY && banCo.getLuotChoi() == mauAI) {
                xuLyLuotAI(dt);
            }
        }

        xuLySuKien();
        ve();
    }
}

void GameManager::capNhatDongHo(float dt) {
    if (thoiGianVanDau <= 0.0f) return; // Vo han

    if (banCo.getLuotChoi() == Mau::DO) {
        thoiGianConDo -= dt;
        if (thoiGianConDo <= 0.0f) {
            thoiGianConDo = 0.0f;
            banCo.setKetThuc(true);
            banCo.setNguoiThang(Mau::DEN);
            lyDoKetThuc = "Do het gio thi dau!";
            soundManager.play(SoundType::DEFEAT);
        }
    } else {
        thoiGianConDen -= dt;
        if (thoiGianConDen <= 0.0f) {
            thoiGianConDen = 0.0f;
            banCo.setKetThuc(true);
            banCo.setNguoiThang(Mau::DO);
            lyDoKetThuc = "Den het gio thi dau!";
            soundManager.play(SoundType::DEFEAT);
        }
    }
}

void GameManager::xuLyLuotAI(float dt) {
    if (!aiDangSuyNghi) {
        aiDangSuyNghi = true;
        thoiGianChoAI = 0.4f; // Delay nhe tao cam giac may suy nghi
    } else {
        thoiGianChoAI -= dt;
        if (thoiGianChoAI <= 0.0f) {
            NuocDi nd = ChessAI::timNuocDi(banCo, doKhoAI, mauAI);
            if (nd.hangBatDau >= 0) {
                bool anQuan = (banCo.timQuan(nd.hangKetThuc, nd.cotKetThuc) != nullptr);
                banCo.diChuyen(nd.hangBatDau, nd.cotBatDau, nd.hangKetThuc, nd.cotKetThuc);
                
                if (banCo.getKetThuc()) {
                    lyDoKetThuc = "Chieu bi doi thu!";
                    soundManager.play(banCo.getNguoiThang() == mauNguoiChoi ? SoundType::VICTORY : SoundType::DEFEAT);
                } else if (banCo.kiemTraChieu(mauNguoiChoi)) {
                    soundManager.play(SoundType::CHECK);
                    hienToast("MAY CHIẾU TƯỚNG!");
                } else if (anQuan) {
                    soundManager.play(SoundType::CAPTURE);
                } else {
                    soundManager.play(SoundType::MOVE);
                }
            }
            aiDangSuyNghi = false;
        }
    }
}

void GameManager::xuLySuKien() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            xuLyClickChuot(mousePos);
        }
        
        if (event.type == sf::Event::KeyPressed) {
            bool ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
            xuLyBanPhim(event.key.code, ctrl);
        }
    }
    
    // Update hover
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (trangThai == TrangThai::MENU_CHINH) {
        menuChinh.update(mousePos);
    } else if (trangThai == TrangThai::SETTINGS) {
        menuSettings.update(mousePos);
    } else if (trangThai == TrangThai::HUONG_DAN) {
        menuHuongDan.update(mousePos);
    } else if (trangThai == TrangThai::CHON_PHE_AI) {
        menuChonPhe.update(mousePos);
    } else if (trangThai == TrangThai::DANG_CHOI) {
        if (banCo.getKetThuc()) {
            for (auto& btn : popupButtons) btn->update(mousePos);
        } else {
            for (auto& btn : inGameButtons) btn->update(mousePos);
        }
    }
}

void GameManager::xuLyBanPhim(sf::Keyboard::Key key, bool ctrl) {
    if (key == sf::Keyboard::Escape) {
        if (trangThai == TrangThai::DANG_CHOI) {
            quayLaiMenu();
        } else if (trangThai == TrangThai::SETTINGS || trangThai == TrangThai::HUONG_DAN || trangThai == TrangThai::CHON_PHE_AI) {
            trangThai = TrangThai::MENU_CHINH;
        }
    }
    
    if (ctrl && key == sf::Keyboard::Z && trangThai == TrangThai::DANG_CHOI) {
        thucHienHoanTac();
    }
    
    if (ctrl && key == sf::Keyboard::Y && trangThai == TrangThai::DANG_CHOI) {
        thucHienDiTiep();
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
            
        case TrangThai::HUONG_DAN:
            menuHuongDan.handleClick(viTri);
            break;
            
        case TrangThai::CHON_PHE_AI:
            menuChonPhe.handleClick(viTri);
            break;
            
        case TrangThai::DANG_CHOI: {
            // Neu da ket thuc thi chi click vao popup
            if (banCo.getKetThuc()) {
                for (auto& btn : popupButtons) {
                    btn->handleClick(viTri);
                }
                return;
            }
            
            // Kiem tra click cac nut ben phai
            for (auto& btn : inGameButtons) {
                if (btn->contains(viTri)) {
                    btn->handleClick(viTri);
                    return;
                }
            }
            
            // Neu la luot may thi khong cho click vao ban co
            if (cheDoChoi == CheDoChoi::VOI_MAY && banCo.getLuotChoi() == mauAI) {
                return;
            }
            
            // Click tren ban co
            sf::Vector2i toaDo = chuyenDoiToaDoManHinhThanhBanCo(viTri);
            if (toaDo.x >= 0 && toaDo.x < BanCo::getSOHANG() &&
                toaDo.y >= 0 && toaDo.y < BanCo::getSOCOT()) {
                
                int h = toaDo.x;
                int c = toaDo.y;
                
                if (quanDangChon == nullptr) {
                    chonQuan(h, c);
                } else {
                    // Neu click vao quan khac cung phe -> chuyen chon sang quan do
                    auto quanClick = banCo.timQuan(h, c);
                    if (quanClick && quanClick->getMau() == banCo.getLuotChoi()) {
                        chonQuan(h, c);
                    } else {
                        diChuyenQuan(h, c);
                    }
                }
            } else {
                // Click ngoai ban co -> huy chon
                quanDangChon = nullptr;
                cacNuocDiHopLe.clear();
            }
            break;
        }
    }
}

void GameManager::chonQuan(int hang, int cot) {
    auto quan = banCo.timQuan(hang, cot);
    if (quan && quan->getMau() == banCo.getLuotChoi()) {
        // Neu danh voi may thi chi cho chon quan cua nguoi choi
        if (cheDoChoi == CheDoChoi::VOI_MAY && quan->getMau() != mauNguoiChoi) {
            return;
        }
        quanDangChon = quan;
        tinhCacNuocDiHopLe();
        soundManager.play(SoundType::CLICK);
    }
}

void GameManager::diChuyenQuan(int hang, int cot) {
    if (!quanDangChon) return;
    
    int hBD = quanDangChon->getHang();
    int cBD = quanDangChon->getCot();
    bool anQuan = (banCo.timQuan(hang, cot) != nullptr);
    
    bool thanhCong = banCo.diChuyen(hBD, cBD, hang, cot);
    if (thanhCong) {
        quanDangChon = nullptr;
        cacNuocDiHopLe.clear();
        
        if (banCo.getKetThuc()) {
            lyDoKetThuc = "Chieu bi doi thu!";
            soundManager.play(SoundType::VICTORY);
        } else if (banCo.kiemTraChieu(banCo.getLuotChoi())) {
            soundManager.play(SoundType::CHECK);
            hienToast("CHIẾU TƯỚNG!");
        } else if (anQuan) {
            soundManager.play(SoundType::CAPTURE);
        } else {
            soundManager.play(SoundType::MOVE);
        }
    } else {
        // Nuoc di khong hop le
        quanDangChon = nullptr;
        cacNuocDiHopLe.clear();
    }
}

void GameManager::tinhCacNuocDiHopLe() {
    cacNuocDiHopLe.clear();
    if (!quanDangChon) return;
    
    int hBD = quanDangChon->getHang();
    int cBD = quanDangChon->getCot();
    
    for (int h = 0; h < BanCo::getSOHANG(); ++h) {
        for (int c = 0; c < BanCo::getSOCOT(); ++c) {
            if (banCo.kiemTraNuocDiHopLe(hBD, cBD, h, c)) {
                cacNuocDiHopLe.emplace_back(h, c);
            }
        }
    }
}

void GameManager::thucHienHoanTac() {
    soundManager.play(SoundType::CLICK);
    if (!banCo.coTheHoanTac()) {
        hienToast("Chua co nuoc di de hoan tac!");
        return;
    }
    
    if (cheDoChoi == CheDoChoi::VOI_MAY) {
        // Trong che do may: hoan tac 2 nuoc (nuoc may di va nuoc minh di)
        banCo.hoanTac();
        banCo.hoanTac();
    } else {
        banCo.hoanTac();
    }
    
    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
    aiDangSuyNghi = false;
    hienToast("Da hoan tac nuoc di.");
}

void GameManager::thucHienDiTiep() {
    soundManager.play(SoundType::CLICK);
    if (!banCo.coTheDiTiep()) {
        hienToast("Khong con nuoc di de di tiep!");
        return;
    }
    
    if (cheDoChoi == CheDoChoi::VOI_MAY) {
        banCo.diTiep();
        banCo.diTiep();
    } else {
        banCo.diTiep();
    }
    
    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
    hienToast("Da di tiep.");
}

void GameManager::thucHienDauHang() {
    soundManager.play(SoundType::DEFEAT);
    Mau mauThua = (cheDoChoi == CheDoChoi::VOI_MAY) ? mauNguoiChoi : banCo.getLuotChoi();
    banCo.dauHang(mauThua);
    lyDoKetThuc = (mauThua == Mau::DO ? "Quan Do" : "Quan Den") + std::string(" xin dau hang!");
}

void GameManager::thucHienXinHoa() {
    soundManager.play(SoundType::CLICK);
    if (cheDoChoi == CheDoChoi::VOI_MAY) {
        // May danh gia the co
        int diem = ChessAI::timNuocDi(banCo, doKhoAI, mauAI).hangBatDau; // trigger AI evaluation
        // Cho hoa neu the co can bang
        banCo.xinHoa();
        lyDoKetThuc = "Hai ben dong y hoa co!";
        hienToast("May da dong y hoa co!");
    } else {
        banCo.xinHoa();
        lyDoKetThuc = "Hai ben dong y hoa co!";
        hienToast("Van co ket thuc hoa!");
    }
}

void GameManager::thucHienLuuGame() {
    soundManager.play(SoundType::CLICK);
    if (banCo.luuFile("savegame.txt")) {
        hienToast("Luu van co thanh cong (savegame.txt)!");
    } else {
        hienToast("Loi khi luu file!");
    }
}

void GameManager::thucHienLoadGame() {
    soundManager.play(SoundType::CLICK);
    if (banCo.docFile("savegame.txt")) {
        quanDangChon = nullptr;
        cacNuocDiHopLe.clear();
        aiDangSuyNghi = false;
        hienToast("Tai van co thanh cong!");
    } else {
        hienToast("Khong tim thay file savegame.txt!");
    }
}

void GameManager::quayLaiMenu() {
    soundManager.play(SoundType::CLICK);
    trangThai = TrangThai::MENU_CHINH;
    quanDangChon = nullptr;
    cacNuocDiHopLe.clear();
}

void GameManager::hienToast(const std::string& msg) {
    toastMessage = msg;
    toastTimer = 2.5f;
}

sf::Vector2i GameManager::chuyenDoiToaDoManHinhThanhBanCo(const sf::Vector2i& viTriChuot) {
    // Tim intersection gan nhat
    float colF = (viTriChuot.x - offsetX) / kichThuocO;
    float rowF = (viTriChuot.y - offsetY) / kichThuocO;
    int c = static_cast<int>(std::round(colF));
    int h = static_cast<int>(std::round(rowF));
    
    // Kiem tra khoang cach click den diem giao cat (ban kinh click hop le 32px)
    float px = offsetX + c * kichThuocO;
    float py = offsetY + h * kichThuocO;
    float dx = viTriChuot.x - px;
    float dy = viTriChuot.y - py;
    if (std::sqrt(dx * dx + dy * dy) > 34.0f) {
        return sf::Vector2i(-1, -1);
    }
    
    return sf::Vector2i(h, c);
}

sf::Vector2f GameManager::chuyenDoiToaDoBanCoThanhManHinh(int hang, int cot) {
    return sf::Vector2f(
        offsetX + cot * kichThuocO,
        offsetY + hang * kichThuocO
    );
}

void GameManager::ve() {
    window.clear(sf::Color(28, 32, 40));
    
    switch (trangThai) {
        case TrangThai::MENU_CHINH:
            veMenuChinh();
            break;
        case TrangThai::SETTINGS:
            veMenuSettings();
            break;
        case TrangThai::HUONG_DAN:
            veMenuHuongDan();
            break;
        case TrangThai::CHON_PHE_AI:
            veMenuChonPhe();
            break;
        case TrangThai::DANG_CHOI:
            veBanCoTruyenThong();
            veHighlights();
            veCacQuanCo();
            veThanhBenPhai();
            if (banCo.getKetThuc()) {
                vePopupKetThuc();
            }
            veToast();
            break;
    }
    
    window.display();
}

void GameManager::veBanCoTruyenThong() {
    float boardW = 8.0f * kichThuocO;
    float boardH = 9.0f * kichThuocO;
    float margin = 36.0f;

    // 1. Mat ban co go
    sf::RectangleShape goNgoai(sf::Vector2f(boardW + margin * 2.0f, boardH + margin * 2.0f));
    goNgoai.setPosition(offsetX - margin, offsetY - margin);
    goNgoai.setFillColor(sf::Color(120, 72, 36));
    goNgoai.setOutlineThickness(5.0f);
    goNgoai.setOutlineColor(sf::Color(75, 42, 18));
    window.draw(goNgoai);

    sf::RectangleShape goTrong(sf::Vector2f(boardW + 40.0f, boardH + 40.0f));
    goTrong.setPosition(offsetX - 20.0f, offsetY - 20.0f);
    goTrong.setFillColor(sf::Color(234, 195, 142));
    goTrong.setOutlineThickness(2.0f);
    goTrong.setOutlineColor(sf::Color(140, 85, 40));
    window.draw(goTrong);

    // 2. Vien khung kep
    sf::RectangleShape khungKep(sf::Vector2f(boardW + 16.0f, boardH + 16.0f));
    khungKep.setPosition(offsetX - 8.0f, offsetY - 8.0f);
    khungKep.setFillColor(sf::Color::Transparent);
    khungKep.setOutlineThickness(2.0f);
    khungKep.setOutlineColor(sf::Color(90, 50, 20));
    window.draw(khungKep);

    sf::Color lineColor(90, 50, 20);

    // 3. Cac duong ke ngang (10 duong)
    for (int h = 0; h < 10; ++h) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(offsetX, offsetY + h * kichThuocO), lineColor),
            sf::Vertex(sf::Vector2f(offsetX + boardW, offsetY + h * kichThuocO), lineColor)
        };
        window.draw(line, 2, sf::Lines);
    }

    // 4. Cac duong ke doc (9 duong, khong cat ngang song)
    for (int c = 0; c < 9; ++c) {
        float x = offsetX + c * kichThuocO;
        if (c == 0 || c == 8) {
            // Bien ngoai noi lien tu hang 0 den 9
            sf::Vertex line[] = {
                sf::Vertex(sf::Vector2f(x, offsetY), lineColor),
                sf::Vertex(sf::Vector2f(x, offsetY + boardH), lineColor)
            };
            window.draw(line, 2, sf::Lines);
        } else {
            // Nua tren (hang 0 den 4)
            sf::Vertex lineTren[] = {
                sf::Vertex(sf::Vector2f(x, offsetY), lineColor),
                sf::Vertex(sf::Vector2f(x, offsetY + 4.0f * kichThuocO), lineColor)
            };
            window.draw(lineTren, 2, sf::Lines);
            
            // Nua duoi (hang 5 den 9)
            sf::Vertex lineDuoi[] = {
                sf::Vertex(sf::Vector2f(x, offsetY + 5.0f * kichThuocO), lineColor),
                sf::Vertex(sf::Vector2f(x, offsetY + boardH), lineColor)
            };
            window.draw(lineDuoi, 2, sf::Lines);
        }
    }

    // 5. Chu Song (So Ha - Han Gioi)
    sf::Text textSong1;
    textSong1.setFont(font);
    textSong1.setString("SO  HA");
    textSong1.setCharacterSize(22);
    textSong1.setFillColor(sf::Color(130, 80, 45, 190));
    textSong1.setPosition(offsetX + 1.2f * kichThuocO, offsetY + 4.25f * kichThuocO);
    window.draw(textSong1);

    sf::Text textSong2;
    textSong2.setFont(font);
    textSong2.setString("HAN  GIOI");
    textSong2.setCharacterSize(22);
    textSong2.setFillColor(sf::Color(130, 80, 45, 190));
    textSong2.setPosition(offsetX + 5.2f * kichThuocO, offsetY + 4.25f * kichThuocO);
    window.draw(textSong2);

    // 6. Cung Cuu Cung (2 duong cheo X moi ben)
    // Cung Den: (0,3) -> (2,5) va (0,5) -> (2,3)
    sf::Vertex cheoDen1[] = {
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, 3), lineColor),
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(2, 5), lineColor)
    };
    sf::Vertex cheoDen2[] = {
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(0, 5), lineColor),
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(2, 3), lineColor)
    };
    window.draw(cheoDen1, 2, sf::Lines);
    window.draw(cheoDen2, 2, sf::Lines);

    // Cung Do: (7,3) -> (9,5) va (7,5) -> (9,3)
    sf::Vertex cheoDo1[] = {
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(7, 3), lineColor),
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(9, 5), lineColor)
    };
    sf::Vertex cheoDo2[] = {
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(7, 5), lineColor),
        sf::Vertex(chuyenDoiToaDoBanCoThanhManHinh(9, 3), lineColor)
    };
    window.draw(cheoDo1, 2, sf::Lines);
    window.draw(cheoDo2, 2, sf::Lines);
}

void GameManager::veHighlights() {
    // 1. Highlight nuoc di vua roi (from & to)
    NuocDi ndCuoi = banCo.getNuocDiCuoi();
    if (ndCuoi.hangBatDau >= 0) {
        sf::Vector2f pBD = chuyenDoiToaDoBanCoThanhManHinh(ndCuoi.hangBatDau, ndCuoi.cotBatDau);
        sf::Vector2f pKT = chuyenDoiToaDoBanCoThanhManHinh(ndCuoi.hangKetThuc, ndCuoi.cotKetThuc);
        
        sf::CircleShape cBD(kichThuocO * 0.42f);
        cBD.setOrigin(kichThuocO * 0.42f, kichThuocO * 0.42f);
        cBD.setPosition(pBD);
        cBD.setFillColor(sf::Color(100, 180, 255, 60));
        cBD.setOutlineThickness(2.0f);
        cBD.setOutlineColor(sf::Color(70, 160, 240, 180));
        window.draw(cBD);

        sf::CircleShape cKT(kichThuocO * 0.42f);
        cKT.setOrigin(kichThuocO * 0.42f, kichThuocO * 0.42f);
        cKT.setPosition(pKT);
        cKT.setFillColor(sf::Color(100, 220, 120, 60));
        cKT.setOutlineThickness(2.0f);
        cKT.setOutlineColor(sf::Color(80, 200, 100, 180));
        window.draw(cKT);
    }

    // 2. Highlight quan dang chon
    if (quanDangChon) {
        sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(quanDangChon->getHang(), quanDangChon->getCot());
        sf::CircleShape hl(kichThuocO * 0.44f);
        hl.setOrigin(kichThuocO * 0.44f, kichThuocO * 0.44f);
        hl.setPosition(pos);
        hl.setFillColor(sf::Color(255, 230, 80, 90));
        hl.setOutlineThickness(3.0f);
        hl.setOutlineColor(sf::Color(255, 200, 0, 230));
        window.draw(hl);
    }

    // 3. Highlight cac nuoc di hop le (Legal move dots)
    if (hienGoiY && quanDangChon) {
        for (const auto& viTri : cacNuocDiHopLe) {
            sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(viTri.x, viTri.y);
            auto quanDich = banCo.timQuan(viTri.x, viTri.y);
            
            if (quanDich) {
                // O co quan dich: vong tron do bao quanh quan
                sf::CircleShape dot(kichThuocO * 0.42f);
                dot.setOrigin(kichThuocO * 0.42f, kichThuocO * 0.42f);
                dot.setPosition(pos);
                dot.setFillColor(sf::Color(240, 50, 50, 70));
                dot.setOutlineThickness(3.0f);
                dot.setOutlineColor(sf::Color(230, 40, 40, 220));
                window.draw(dot);
            } else {
                // O trong: cham tron xanh nho o tam
                sf::CircleShape dot(7.0f);
                dot.setOrigin(7.0f, 7.0f);
                dot.setPosition(pos);
                dot.setFillColor(sf::Color(40, 180, 70, 200));
                dot.setOutlineThickness(1.5f);
                dot.setOutlineColor(sf::Color::White);
                window.draw(dot);
            }
        }
    }

    // 4. Highlight Tuong dang bi chieu
    for (const auto& q : banCo.getCacQuan()) {
        if (!q->getDaBiAn() && q->layTen() == "Tuong") {
            if (banCo.kiemTraChieu(q->getMau())) {
                sf::Vector2f pos = chuyenDoiToaDoBanCoThanhManHinh(q->getHang(), q->getCot());
                sf::CircleShape chieu(kichThuocO * 0.46f);
                chieu.setOrigin(kichThuocO * 0.46f, kichThuocO * 0.46f);
                chieu.setPosition(pos);
                chieu.setFillColor(sf::Color(255, 0, 0, 80));
                chieu.setOutlineThickness(3.5f);
                chieu.setOutlineColor(sf::Color::Red);
                window.draw(chieu);
            }
        }
    }
}

void GameManager::veCacQuanCo() {
    float r = kichThuocO * 0.40f;

    for (const auto& quan : banCo.getCacQuan()) {
        if (quan->getDaBiAn()) continue;

        sf::Vector2f viTri = chuyenDoiToaDoBanCoThanhManHinh(quan->getHang(), quan->getCot());

        // 1. Dia go ben ngoai
        sf::CircleShape diaGo(r);
        diaGo.setOrigin(r, r);
        diaGo.setPosition(viTri);
        diaGo.setFillColor(sf::Color(246, 236, 218));
        diaGo.setOutlineThickness(2.5f);
        diaGo.setOutlineColor(sf::Color(160, 110, 60));
        window.draw(diaGo);

        // 2. Vanh vien trong
        sf::CircleShape vanhTrong(r - 4.5f);
        vanhTrong.setOrigin(r - 4.5f, r - 4.5f);
        vanhTrong.setPosition(viTri);
        vanhTrong.setFillColor(sf::Color(252, 245, 230));
        vanhTrong.setOutlineThickness(1.8f);
        
        if (quan->getMau() == Mau::DO) {
            vanhTrong.setOutlineColor(sf::Color(200, 30, 30));
        } else {
            vanhTrong.setOutlineColor(sf::Color(30, 30, 30));
        }
        window.draw(vanhTrong);

        // 3. Chu ten quan co tieng Viet ro rang
        sf::Text text;
        text.setFont(font);
        
        std::string ten = quan->layTen();
        if (ten == "Tuong") text.setString("TUONG");
        else if (ten == "Si") text.setString("SI");
        else if (ten == "Voi") text.setString("TUONG");
        else if (ten == "Xe") text.setString("XE");
        else if (ten == "Phao") text.setString("PHAO");
        else if (ten == "Ma") text.setString("MA");
        else if (ten == "Tot") text.setString("TOT");
        else text.setString(quan->layKyHieu());

        // Chinh font size de vua vanh tron
        if (text.getString().getSize() > 4) {
            text.setCharacterSize(13);
        } else if (text.getString().getSize() > 2) {
            text.setCharacterSize(15);
        } else {
            text.setCharacterSize(19);
        }

        text.setStyle(sf::Text::Bold);
        if (quan->getMau() == Mau::DO) {
            text.setFillColor(sf::Color(190, 20, 20));
        } else {
            text.setFillColor(sf::Color(25, 25, 25));
        }

        sf::FloatRect tb = text.getLocalBounds();
        text.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        text.setPosition(viTri);

        window.draw(text);
    }
}

void GameManager::veThanhBenPhai() {
    float px = 670.0f;
    float py = 35.0f;
    float pw = 490.0f;
    float ph = 730.0f;

    // Panel card
    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(px, py);
    panel.setFillColor(sf::Color(35, 40, 52, 235));
    panel.setOutlineThickness(2.0f);
    panel.setOutlineColor(sf::Color(60, 70, 90));
    window.draw(panel);

    // 1. Luot choi & Thong tin
    sf::Text luotText;
    luotText.setFont(font);
    luotText.setCharacterSize(26);
    luotText.setStyle(sf::Text::Bold);

    if (banCo.getLuotChoi() == Mau::DO) {
        luotText.setString("LUOT DI: QUAN DO");
        luotText.setFillColor(sf::Color(240, 80, 80));
    } else {
        luotText.setString("LUOT DI: QUAN DEN");
        luotText.setFillColor(sf::Color(220, 220, 220));
    }
    luotText.setPosition(px + 20.0f, py + 18.0f);
    window.draw(luotText);

    // Trang thai may dang suy nghi
    if (cheDoChoi == CheDoChoi::VOI_MAY && banCo.getLuotChoi() == mauAI && !banCo.getKetThuc()) {
        sf::Text aiThinking;
        aiThinking.setFont(font);
        aiThinking.setCharacterSize(17);
        aiThinking.setString("(May dang suy nghi nuoc di...)");
        aiThinking.setFillColor(sf::Color(255, 215, 0));
        aiThinking.setPosition(px + 20.0f, py + 52.0f);
        window.draw(aiThinking);
    }

    // Canh bao chieu tuong
    if (banCo.kiemTraChieu(banCo.getLuotChoi()) && !banCo.getKetThuc()) {
        sf::Text checkAlert;
        checkAlert.setFont(font);
        checkAlert.setCharacterSize(20);
        checkAlert.setStyle(sf::Text::Bold);
        checkAlert.setString("[!] DANG BI CHIEU TUONG! [!]");
        checkAlert.setFillColor(sf::Color(255, 60, 60));
        checkAlert.setPosition(px + 20.0f, py + 80.0f);
        window.draw(checkAlert);
    }

    // 2. Dong ho thi dau (Chess Clocks)
    auto formatTime = [](float sec) {
        if (sec <= 0.0f) return std::string("--:--");
        int s = static_cast<int>(sec);
        int m = s / 60;
        s %= 60;
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << m << ":"
            << std::setfill('0') << std::setw(2) << s;
        return oss.str();
    };

    float clockY = py + 115.0f;
    float cw = 215.0f;
    float ch = 60.0f;

    // Clock Do
    sf::RectangleShape boxDo(sf::Vector2f(cw, ch));
    boxDo.setPosition(px + 20.0f, clockY);
    boxDo.setFillColor(sf::Color(45, 30, 30));
    boxDo.setOutlineThickness(banCo.getLuotChoi() == Mau::DO ? 3.0f : 1.0f);
    boxDo.setOutlineColor(banCo.getLuotChoi() == Mau::DO ? sf::Color(240, 80, 80) : sf::Color(90, 60, 60));
    window.draw(boxDo);

    sf::Text tDo;
    tDo.setFont(font);
    tDo.setCharacterSize(22);
    tDo.setString("DO: " + formatTime(thoiGianConDo));
    tDo.setFillColor(sf::Color(240, 100, 100));
    tDo.setPosition(px + 35.0f, clockY + 16.0f);
    window.draw(tDo);

    // Clock Den
    sf::RectangleShape boxDen(sf::Vector2f(cw, ch));
    boxDen.setPosition(px + 250.0f, clockY);
    boxDen.setFillColor(sf::Color(30, 35, 45));
    boxDen.setOutlineThickness(banCo.getLuotChoi() == Mau::DEN ? 3.0f : 1.0f);
    boxDen.setOutlineColor(banCo.getLuotChoi() == Mau::DEN ? sf::Color(100, 180, 255) : sf::Color(60, 70, 90));
    window.draw(boxDen);

    sf::Text tDen;
    tDen.setFont(font);
    tDen.setCharacterSize(22);
    tDen.setString("DEN: " + formatTime(thoiGianConDen));
    tDen.setFillColor(sf::Color(180, 210, 240));
    tDen.setPosition(px + 265.0f, clockY + 16.0f);
    window.draw(tDen);

    // 3. Bang lich su nuoc di
    float histY = clockY + 75.0f;
    sf::Text histTitle;
    histTitle.setFont(font);
    histTitle.setCharacterSize(18);
    histTitle.setStyle(sf::Text::Bold);
    histTitle.setString("LICH SU NUOC DI GAN NHAT:");
    histTitle.setFillColor(sf::Color(200, 210, 225));
    histTitle.setPosition(px + 20.0f, histY);
    window.draw(histTitle);

    sf::RectangleShape histBox(sf::Vector2f(pw - 40.0f, 130.0f));
    histBox.setPosition(px + 20.0f, histY + 28.0f);
    histBox.setFillColor(sf::Color(25, 28, 38));
    histBox.setOutlineThickness(1.0f);
    histBox.setOutlineColor(sf::Color(55, 65, 85));
    window.draw(histBox);

    const auto& lichSu = banCo.getLichSuNuocDi();
    size_t count = lichSu.size();
    size_t start = (count > 5) ? (count - 5) : 0;
    float textY = histY + 34.0f;

    if (lichSu.empty()) {
        sf::Text emptyT;
        emptyT.setFont(font);
        emptyT.setCharacterSize(15);
        emptyT.setString("(Chua co nuoc di nao)");
        emptyT.setFillColor(sf::Color(120, 130, 145));
        emptyT.setPosition(px + 35.0f, textY + 40.0f);
        window.draw(emptyT);
    } else {
        for (size_t i = start; i < count; ++i) {
            sf::Text ndText;
            ndText.setFont(font);
            ndText.setCharacterSize(15);
            std::string line = std::to_string(i + 1) + ". " + banCo.layMoTaNuocDi(lichSu[i]);
            ndText.setString(line);
            ndText.setFillColor((lichSu[i].mauQuan == Mau::DO) ? sf::Color(240, 120, 120) : sf::Color(170, 200, 230));
            ndText.setPosition(px + 30.0f, textY);
            window.draw(ndText);
            textY += 23.0f;
        }
    }

    // 4. Ve cac nut dieu khien
    for (auto& btn : inGameButtons) {
        btn->draw(window);
    }
}

void GameManager::vePopupKetThuc() {
    // Lop phu mo
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 170));
    window.draw(overlay);

    // Hop thoai popup
    float dw = 520.0f;
    float dh = 300.0f;
    float dx = (window.getSize().x - dw) / 2.0f;
    float dy = (window.getSize().y - dh) / 2.0f;

    sf::RectangleShape dialog(sf::Vector2f(dw, dh));
    dialog.setPosition(dx, dy);
    dialog.setFillColor(sf::Color(32, 38, 50));
    dialog.setOutlineThickness(3.0f);
    dialog.setOutlineColor(sf::Color(240, 190, 70));
    window.draw(dialog);

    // Tieu de
    sf::Text tTitle;
    tTitle.setFont(font);
    tTitle.setCharacterSize(34);
    tTitle.setStyle(sf::Text::Bold);
    tTitle.setString("KET THUC VAN CO");
    tTitle.setFillColor(sf::Color(240, 190, 70));
    sf::FloatRect tb = tTitle.getLocalBounds();
    tTitle.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    tTitle.setPosition(window.getSize().x / 2.0f, dy + 50.0f);
    window.draw(tTitle);

    // Ket qua
    sf::Text tResult;
    tResult.setFont(font);
    tResult.setCharacterSize(26);
    tResult.setStyle(sf::Text::Bold);

    if (banCo.getNguoiThang() == Mau::DO) {
        tResult.setString("QUAN DO CHIEN THANG!");
        tResult.setFillColor(sf::Color(240, 80, 80));
    } else if (banCo.getNguoiThang() == Mau::DEN) {
        tResult.setString("QUAN DEN CHIEN THANG!");
        tResult.setFillColor(sf::Color(100, 180, 255));
    } else {
        tResult.setString("HAI BEN HOA CO!");
        tResult.setFillColor(sf::Color(220, 220, 220));
    }

    tb = tResult.getLocalBounds();
    tResult.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    tResult.setPosition(window.getSize().x / 2.0f, dy + 110.0f);
    window.draw(tResult);

    // Ly do ket thuc
    if (!lyDoKetThuc.empty()) {
        sf::Text tReason;
        tReason.setFont(font);
        tReason.setCharacterSize(18);
        tReason.setString(lyDoKetThuc);
        tReason.setFillColor(sf::Color(190, 200, 215));
        tb = tReason.getLocalBounds();
        tReason.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        tReason.setPosition(window.getSize().x / 2.0f, dy + 155.0f);
        window.draw(tReason);
    }

    // 2 nut Choi lai va Ve Menu
    for (auto& btn : popupButtons) {
        btn->draw(window);
    }
}

void GameManager::veToast() {
    if (toastTimer <= 0.0f || toastMessage.empty()) return;

    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(18);
    t.setString(toastMessage);
    t.setFillColor(sf::Color::White);

    sf::FloatRect tb = t.getLocalBounds();
    float tw = tb.width + 36.0f;
    float th = 42.0f;
    float tx = (window.getSize().x - tw) / 2.0f;
    float ty = window.getSize().y - 65.0f;

    sf::RectangleShape bg(sf::Vector2f(tw, th));
    bg.setPosition(tx, ty);
    bg.setFillColor(sf::Color(20, 25, 35, 230));
    bg.setOutlineThickness(1.5f);
    bg.setOutlineColor(sf::Color(80, 160, 240));

    t.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    t.setPosition(tx + tw / 2.0f, ty + th / 2.0f);

    window.draw(bg);
    window.draw(t);
}

void GameManager::veMenuChinh() {
    sf::Text title;
    title.setFont(font);
    title.setString("CO TUONG");
    title.setCharacterSize(68);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 100.0f);
    window.draw(title);

    sf::Text sub;
    sub.setFont(font);
    sub.setString("DO AN MON OOP - HCMUS");
    sub.setCharacterSize(20);
    sub.setFillColor(sf::Color(170, 185, 205));
    tb = sub.getLocalBounds();
    sub.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    sub.setPosition(window.getSize().x / 2.0f, 160.0f);
    window.draw(sub);
    
    menuChinh.draw(window);
}

void GameManager::veMenuSettings() {
    sf::Text title;
    title.setFont(font);
    title.setString("CAI DAT TRO CHOI");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 100.0f);
    window.draw(title);
    
    menuSettings.draw(window);
}

void GameManager::veMenuHuongDan() {
    sf::Text title;
    title.setFont(font);
    title.setString("HUONG DAN LUAT CHOI CO TUONG");
    title.setCharacterSize(42);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 65.0f);
    window.draw(title);

    sf::RectangleShape box(sf::Vector2f(960.0f, 540.0f));
    box.setPosition((window.getSize().x - 960.0f) / 2.0f, 120.0f);
    box.setFillColor(sf::Color(35, 42, 56, 230));
    box.setOutlineThickness(2.0f);
    box.setOutlineColor(sf::Color(70, 85, 115));
    window.draw(box);

    std::string text = 
        "- BAN CO & MUC TIEU:\n"
        "  Ban co 10 hang x 9 cot. Hai ben Do va Den thi dau voi muc tieu chieu bi Tuong doi phuong.\n"
        "  O giua co Song (So Ha - Han Gioi). Cung Cuu Cung rong 3x3 o moi ben co 2 duong cheo X.\n\n"
        "- QUY TAC DI CHUYEN TUNG QUAN:\n"
        "  1. TUONG: Di ngang hoac doc 1 o trong cung. Hai Tuong khong duoc lo mat chong nhau.\n"
        "  2. SI: Di cheo 1 o trong cung 3x3, bao ve Tuong.\n"
        "  3. TUONG (VOI): Di cheo dung 2 o, khong duoc qua song, khong duoc bi chan mat voi.\n"
        "  4. XE: Di ngang hoac doc tuy y khong gioi han o, khong duoc nhay qua quan.\n"
        "  5. PHAO: Di ngang/doc nhu Xe. Khi an quan bat buoc phai nhay qua dung 1 quan lam ngoi.\n"
        "  6. MA: Di hinh chu Nhat (2-1 hoac 1-2). Khong duoc bi chan chan Ma.\n"
        "  7. TOT: Chua qua song chi duoc di thang 1 o. Sau khi qua song duoc di thang hoac ngang 1 o.\n\n"
        "- TIEN ICH & PHIM TAT:\n"
        "  Ctrl + Z: Hoan tac nuoc di  |  Ctrl + Y: Di tiep nuoc da hoan tac  |  ESC: Quay lai Menu\n"
        "  Luu & Tai game: Cho phep luu the co hien tai vao file savegame.txt va load tiep bat cu luc nao!";

    sf::Text content;
    content.setFont(font);
    content.setString(text);
    content.setCharacterSize(17);
    content.setFillColor(sf::Color(225, 235, 245));
    content.setPosition((window.getSize().x - 960.0f) / 2.0f + 30.0f, 140.0f);
    window.draw(content);

    menuHuongDan.draw(window);
}

void GameManager::veMenuChonPhe() {
    sf::Text title;
    title.setFont(font);
    title.setString("CHON PHE DAU VOI MAY");
    title.setCharacterSize(48);
    title.setFillColor(sf::Color(240, 190, 70));
    title.setStyle(sf::Text::Bold);
    
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    title.setPosition(window.getSize().x / 2.0f, 140.0f);
    window.draw(title);

    sf::Text sub;
    sub.setFont(font);
    sub.setString("Ban muon cam quan nao trong van dau?");
    sub.setCharacterSize(22);
    sub.setFillColor(sf::Color(180, 195, 215));
    tb = sub.getLocalBounds();
    sub.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    sub.setPosition(window.getSize().x / 2.0f, 210.0f);
    window.draw(sub);

    menuChonPhe.draw(window);
}
