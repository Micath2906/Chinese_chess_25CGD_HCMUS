# Hướng dẫn cài đặt

## Yêu cầu hệ thống

- **Windows 10/11** (hoặc Linux/macOS)
- **C++ Compiler**: MSVC, GCC, hoặc Clang hỗ trợ C++17
- **CMake 3.15+**
- **SFML 2.5+**

## Cài đặt trên Windows

### Bước 1: Cài đặt Build Tools

#### Visual Studio (Khuyến nghị)
1. Download [Visual Studio Community](https://visualstudio.microsoft.com/)
2. Cài đặt với workload "Desktop development with C++"
3. Đảm bảo CMake được bao gồm trong installation

#### Hoặc MinGW
1. Download [MinGW-w64](https://www.mingw-w64.org/)
2. Cài đặt và thêm vào PATH

### Bước 2: Cài đặt SFML

#### Phương án 1: Sử dụng vcpkg (Dễ nhất)

```bash
# Cai dat vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat

# Cai dat SFML
vcpkg install sfml:x64-windows

# Tich hop voi CMake
vcpkg integrate install
```

#### Phương án 2: Download thủ công

1. Tải SFML từ: https://www.sfml-dev.org/download/sfml/2.6.1/
2. Chọn phiên bản phù hợp với compiler (Visual C++ 15, 16, hoặc 17)
3. Giải nén vào thư mục (ví dụ: `C:\SFML`)
4. Thêm vào biến môi trường hoặc cấu hình trong CMake

### Bước 3: Tài nguyên (Assets)

Toàn bộ phông chữ Unicode (Viaoda Libre, Nôm Na Tống, Pixeloid Sans), hình ảnh bàn cờ, 14 quân cờ, spritesheet UI và âm thanh đã được tích hợp sẵn đầy đủ trong thư mục `assets/` của dự án. Không cần tải hay cài đặt thêm font ngoài.

### Bước 4: Build Project

#### Sử dụng script có sẵn:
```bash
build.bat
```

#### Hoặc build thủ công:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Bước 5: Kiểm tra tài nguyên & Chạy game

```bash
# Kiểm tra toàn vẹn tài nguyên game:
CoTuong.exe --check-assets
# Kỳ vọng: in "[OK] All required assets are present and validated successfully." và kết quả kiểm thử TurnOrderRandomizer

# Chạy game trực tiếp:
CoTuong.exe
```
cd build\Release
CoTuong.exe
```

---

## Cài đặt trên Linux (Ubuntu/Debian)

### Bước 1: Cài đặt dependencies
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
sudo apt-get install libsfml-dev
sudo apt-get install fonts-dejavu-core  # Font
```

### Bước 2: Build
```bash
mkdir build && cd build
cmake ..
make
```

### Bước 3: Chạy
```bash
./CoTuong
```

---

## Cài đặt trên macOS

### Bước 1: Cài đặt dependencies
```bash
# Cai dat Homebrew neu chua co
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Cai dat SFML
brew install sfml
brew install cmake
```

### Bước 2: Build
```bash
mkdir build && cd build
cmake ..
make
```

### Bước 3: Chạy
```bash
./CoTuong
```

---

## Xử lý lỗi thường gặp

### 1. "Could not find SFML"
**Giải pháp:**
- Đảm bảo SFML đã được cài đặt
- Nếu dùng vcpkg: `vcpkg integrate install`
- Nếu cài thủ công: Thêm đường dẫn vào CMakeLists.txt:
  ```cmake
  set(SFML_DIR "C:/SFML/lib/cmake/SFML")
  ```

### 2. Lỗi thiếu tài nguyên (Missing assets)
**Giải pháp:**
- Chạy `CoTuong.exe --check-assets` để kiểm tra danh sách file thiếu
- Đảm bảo thư mục `assets/` nằm cùng cấp với file thực thi hoặc ở thư mục gốc dự án

### 3. Missing DLL files (Windows)
**Giải pháp:**
- CMake đã tự động sao chép các file `.dll` cần thiết vào thư mục build
- Nếu chạy độc lập, copy các file `.dll` từ `SFML/bin/` vào thư mục chứa `CoTuong.exe`

### 4. Build failed với MSVC
**Giải pháp:**
- Đảm bảo sử dụng đúng phiên bản SFML cho compiler
- Visual Studio 2019 → SFML Visual C++ 16
- Visual Studio 2022 → SFML Visual C++ 17

---

## Cấu trúc thư mục sau khi build

```
Co_tuong/
├── build/
│   └── Release/
│       ├── CoTuong.exe
│       ├── openal32.dll
│       └── assets/
├── assets/
│   ├── fonts/
│   ├── textures/
│   └── audio/
├── include/
├── src/
└── ...
```

---

## Test nhanh

Sau khi build xong, chạy:

```bash
# Kiểm tra tự động tính toàn vẹn của assets & thuật toán tung đồng xu
CoTuong.exe --check-assets
```
Kết quả mong đợi:
```
[OK] All required assets are present and validated successfully.
[TEST] Running TurnOrderRandomizer unit test...
  - Deterministic seed test: PASSED
  - 1000 trials distribution: Red ~50% / Black ~50%
[OK] TurnOrderRandomizer unit test passed successfully.
```
# Windows
cd build\Release
CoTuong.exe

# Linux/macOS
cd build
./CoTuong
```

Nếu thấy menu game xuất hiện → Cài đặt thành công! 🎉

---

## Liên hệ hỗ trợ

Nếu gặp vấn đề, kiểm tra:
1. Phiên bản SFML và compiler có tương thích?
2. CMake có tìm thấy SFML không? (xem output của cmake)
3. Font file có tồn tại không?
4. DLL files có trong thư mục executable không? (Windows)
