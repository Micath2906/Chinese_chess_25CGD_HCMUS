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

### Đã hoàn thành
- ✅ Tất cả quân cờ với luật di chuyển đầy đủ
- ✅ Menu chính với các chế độ chơi
- ✅ Chế độ 2 người chơi
- ✅ Kiểm tra chiếu tướng
- ✅ Kiểm tra chiếu tướng đối mặt
- ✅ Highlight quân đang chọn
- ✅ Hiển thị nước di hợp lệ
- ✅ Hoàn tác nước đi (Ctrl+Z)
- ✅ Menu cài đặt
- ✅ Giao diện đẹp với SFML

### Có thể mở rộng
- ⬜ Chế độ chơi với máy (AI)
- ⬜ Lưu/Load game
- ⬜ Đồng hồ thi đấu
- ⬜ Lịch sử nước đi
- ⬜ Âm thanh hiệu ứng
- ⬜ Animation di chuyển
- ⬜ Chế độ chơi online

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
