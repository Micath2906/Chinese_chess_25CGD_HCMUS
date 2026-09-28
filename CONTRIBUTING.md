# Hướng dẫn đóng góp

## Quy trình đóng góp

1. **Fork** repository này
2. **Clone** về máy của bạn
3. Tạo **branch mới** cho tính năng: `git checkout -b feature/ten-tinh-nang`
4. **Commit** thay đổi: `git commit -m "Them tinh nang X"`
5. **Push** lên GitHub: `git push origin feature/ten-tinh-nang`
6. Tạo **Pull Request**

## Coding Standards

### C++ Style Guide

```cpp
// 1. Đặt tên class: PascalCase
class BanCo { };
class QuanCo { };

// 2. Đặt tên method/function: camelCase
void diChuyenQuan();
bool kiemTraNuocDi();

// 3. Đặt tên biến: camelCase
int hangMoi;
Mau luotChoi;

// 4. Đặt tên constant: UPPER_SNAKE_CASE
const int SO_HANG = 10;
static const int SO_COT = 9;

// 5. Indentation: 4 spaces
class Example {
public:
    void method() {
        if (condition) {
            // code here
        }
    }
};
```

### Header Guards

```cpp
#pragma once  // Khuyến nghị sử dụng

// Hoặc traditional guards
#ifndef TEN_FILE_H
#define TEN_FILE_H
// content
#endif
```

### Include Order

```cpp
// 1. Header của class này (nếu có)
#include "QuanCo.h"

// 2. C++ standard library
#include <iostream>
#include <vector>
#include <memory>

// 3. External libraries
#include <SFML/Graphics.hpp>

// 4. Project headers
#include "BanCo.h"
```

## Git Commit Messages

### Format
```
<type>: <subject>

<body>
```

### Types
- `feat`: Tính năng mới
- `fix`: Sửa bug
- `docs`: Cập nhật documentation
- `style`: Format code, không thay đổi logic
- `refactor`: Refactor code
- `test`: Thêm tests
- `chore`: Build, dependencies

### Examples
```bash
git commit -m "feat: them quan Phao voi luat di chuyen"
git commit -m "fix: sua loi kiem tra chieu tuong"
git commit -m "docs: cap nhat huong dan cai dat"
```

## Testing

Trước khi submit PR:
- [ ] Code compile không có warning
- [ ] Chức năng mới hoạt động đúng
- [ ] Không làm hỏng chức năng cũ
- [ ] Code đã được format đúng style

## Pull Request Template

```markdown
## Mô tả
Mô tả ngắn gọn về thay đổi

## Loại thay đổi
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation

## Testing
Mô tả cách đã test

## Screenshots (nếu có)
```

## Câu hỏi?

Mở issue hoặc liên hệ qua email của giảng viên.
