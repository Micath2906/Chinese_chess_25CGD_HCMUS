# Cờ Tướng - Chinese Chess

Đồ án môn Phương Pháp Lập Trình Hướng Đối Tượng 2

## Mô tả

Game cờ tướng được xây dựng bằng C++ với SFML, áp dụng các nguyên lý OOP:
- **Encapsulation**: Đóng gói dữ liệu và phương thức
- **Inheritance**: Kế thừa từ lớp QuanCo
- **Abstraction**: Lớp trừu tượng QuanCo
- **Polymorphism**: Đa hình qua virtual functions

## Cấu trúc project

```
Co_tuong/
├── include/           # Header files (.h)
│   ├── QuanCo.h      # Lớp cơ sở trừu tượng
│   ├── Xe.h          # Quân Xe
│   ├── Ma.h          # Quân Mã
│   ├── Voi.h         # Quân Voi
│   ├── Phao.h        # Quân Pháo
│   ├── Tuong.h       # Quân Tướng
│   ├── Si.h          # Quân Sĩ
│   ├── Tot.h         # Quân Tốt
│   ├── BanCo.h       # Quản lý bàn cờ
│   ├── Menu.h        # Menu và Button
│   └── GameManager.h # Quản lý game
│
├── src/              # Implementation files (.cpp)
│   ├── QuanCo.cpp
│   ├── Xe.cpp
│   ├── Ma.cpp
│   ├── Voi.cpp
│   ├── Phao.cpp
│   ├── Tuong.cpp
│   ├── Si.cpp
│   ├── Tot.cpp
│   ├── BanCo.cpp
│   ├── Menu.cpp
│   ├── GameManager.cpp
│   └── main.cpp
│
├── resources/        # Tài nguyên
│   ├── fonts/       # Font chữ
│   └── sounds/      # Âm thanh
│
└── CMakeLists.txt   # Build configuration
```

## Yêu cầu

- **C++17** trở lên
- **SFML 2.5+** (graphics, window, system, audio)
- **CMake 3.15+**

## Cài đặt SFML

### Windows
```bash
# Sử dụng vcpkg
vcpkg install sfml

# Hoặc download từ https://www.sfml-dev.org/download.php
```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libsfml-dev
```

### macOS
```bash
brew install sfml
```

## Build Project

```bash
# Tạo thư mục build
mkdir build
cd build

# Generate với CMake
cmake ..

# Build
cmake --build .

# Chạy game
./CoTuong
```

## Các tính năng

### Đã hoàn thành ✅
- ✅ **Chuẩn hóa 100% luật di chuyển Cờ Tướng**:
  - Tướng: đi ngang/dọc 1 ô trong cung, kiểm tra chống mặt Tướng
  - Sĩ: đi chéo 1 ô trong cung
  - Voi: đi chéo 2 ô, không qua sông, cản mắt voi
  - Xe: đi ngang/dọc tự do
  - Pháo: đi như Xe, ăn quân nhảy qua 1 quân làm ngòi
  - Mã: đi chữ Nhật, cản chân mã
  - Tốt: đi thẳng trước khi qua sông; qua sông đi ngang hoặc tiến
- ✅ **Kiểm tra nước đi hợp lệ & Chiếu bí**:
  - Mô phỏng nước đi đảm bảo Tướng phe mình không bị chiếu
  - Tự động phát hiện Chiếu tướng, Chiếu bí (Checkmate) và Bí nước (Stalemate)
- ✅ **Âm thanh sống động (Thuần C++ & SFML Audio)**:
  - Sinh sóng âm thời gian thực: Tiếng gõ gỗ, tiếng ăn quân, chuông chiếu tướng, chiến thắng, thất bại
  - Điều chỉnh âm lượng và bật/tắt trong Cài đặt
- ✅ **Chế độ chơi đa dạng**:
  - 2 người chơi (PvP đối kháng)
  - Chơi với máy (AI - Minimax + Alpha-Beta Pruning + Bảng điểm vị trí) với 3 mức độ Dễ, Vừa, Khó
  - Chọn phe cầm Đỏ (đi trước) hoặc Đen (đi sau) khi đấu với máy
- ✅ **Đồng hồ thi đấu (Chess Clock)**:
  - Đếm ngược thời gian cho mỗi bên (Vô hạn, 5 phút, 10 phút, 15 phút)
  - Xử thua ngay khi hết giờ
- ✅ **Tiện ích trong ván cờ (In-game Controls)**:
  - Hoàn tác nước đi (Undo) & Đi tiếp (Redo)
  - Làm mới ván cờ (New Game)
  - Xin hòa cờ (Offer Draw) & Đầu hàng (Resign)
  - Lưu ván cờ (Save game) & Tải ván cờ (Load game)
  - Lịch sử nước đi (Move History)
  - Highlight nước đi vừa đi (Last Move) và Tướng bị chiếu
- ✅ **Giao diện truyền thống sắc nét**:
  - Bàn cờ gỗ giao điểm 10x9, sông Sở Hà - Hán Giới, cung Cửu Cung
  - Quân cờ đĩa gỗ 2 lớp với ký hiệu tiếng Việt rõ ràng
- ✅ **Màn hình Cài đặt & Hướng dẫn chi tiết**


## Cách chơi

1. **Chọn quân**: Click vào quân cờ của bạn
2. **Di chuyển**: Click vào ô muốn di chuyển (ô màu xanh)
3. **Bỏ chọn**: Click vào quân khác hoặc nhấn ESC
4. **Hoàn tác**: Nhấn Ctrl+Z
5. **Quay lại menu**: Nhấn ESC

## Luật cờ tướng cơ bản

- **Xe**: Di chuyển thẳng theo hàng/cột, không giới hạn ô
- **Mã**: Di chuyển chữ "Nhật" (2-1), không bị chặn chân mã
- **Voi**: Di chuyển chéo 2 ô, không qua sông, không bị chặn mắt voi
- **Pháo**: Di như Xe, ăn quân cần có 1 quân làm cầu
- **Tướng**: Di trong cung 3x3, mỗi nước 1 ô
- **Sĩ**: Di chéo trong cung 3x3
- **Tốt**: Đi thẳng, qua sông được đi ngang

## Kiến thức OOP áp dụng

### 1. Encapsulation
```cpp
class QuanCo {
protected:
    int hang, cot;  // Dữ liệu được đóng gói
    Mau mau;
public:
    int getHang() const;  // Getter
    void datViTri(int h, int c);  // Setter
};
```

### 2. Inheritance
```cpp
class Xe : public QuanCo {
    // Xe kế thừa từ QuanCo
};
```

### 3. Abstraction
```cpp
class QuanCo {
    virtual bool kiemTraNuocDi(...) = 0;  // Pure virtual
    virtual std::string layTen() const = 0;
};
```

### 4. Polymorphism
```cpp
std::shared_ptr<QuanCo> quan = std::make_shared<Xe>(...);
quan->kiemTraNuocDi(...);  // Gọi phiên bản của Xe
```

### 5. Smart Pointers
```cpp
std::vector<std::shared_ptr<QuanCo>> cacQuan;
// Tự động quản lý bộ nhớ, không cần delete
```

### 6. Forward Declaration & Circular Dependency
```cpp
class BanCo;  // Forward declaration

class QuanCo {
    bool kiemTraNuocDi(const BanCo& banCo, ...);
    // QuanCo cần BanCo, BanCo cần QuanCo
};
```

## Tác giả

Trường Đại học Khoa học Tự nhiên TP.HCM
Khoa Công nghệ Thông tin

## License

MIT License - Tự do sử dụng cho mục đích học tập
