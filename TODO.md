# TODO - Danh sách tính năng và tiến độ dự án

## Đã hoàn thành ✅

- [x] **Cấu trúc OOP chuẩn mực**:
  - Áp dụng triệt để Encapsulation, Inheritance, Abstraction, Polymorphism
  - Lớp trừu tượng `QuanCo` kế thừa cho 7 loại quân cờ: `Xe`, `Ma`, `Voi`, `Phao`, `Tuong`, `Si`, `Tot`
  - Quản lý bộ nhớ an toàn với Modern C++ `std::shared_ptr`, `std::unique_ptr`
- [x] **Luật di chuyển chuẩn xác 100% Cờ Tướng**:
  - Tướng: Di chuyển 1 ô ngang hoặc dọc trong cung 3x3
  - Sĩ: Di chuyển 1 ô chéo trong cung 3x3
  - Voi (Tượng): Di chuyển chéo 2 ô, không qua sông, chặn mắt voi
  - Xe: Di chuyển tự do theo hàng/cột, không nhảy qua quân
  - Pháo: Di chuyển như Xe, ăn quân phải nhảy qua đúng 1 quân làm ngòi
  - Mã: Di chuyển chữ Nhật (2-1), chặn chân mã
  - Tốt: Đi thẳng trước khi qua sông; sau khi qua sông đi ngang hoặc tiến
  - Luật Chống Tướng (Hai tướng không được đối mặt trực tiếp trên cùng cột)
- [x] **Kiểm tra hợp lệ nâng cao (Legal Move Simulation)**:
  - Nước đi chỉ hợp lệ khi không để Tướng phe mình bị chiếu
  - Gợi ý nước đi (Legal move dots) chuẩn xác 100%
  - Tự động phát hiện Chiếu tướng, Chiếu bí (Checkmate) và Bí nước (Stalemate)
- [x] **Hệ thống Âm thanh đa dạng (100% C++ & SFML Audio)**:
  - Sinh âm thanh thời gian thực (Procedural Audio Synthesis) qua `sf::SoundBuffer`
  - Âm thanh nước đi (wood tap), ăn quân (heavy capture), chiếu tướng (warning chime), chiến thắng (fanfare), thất bại
  - Điều chỉnh âm lượng (25%, 50%, 75%, 100%) và Bật/Tắt âm thanh trong Settings
- [x] **Chế độ chơi đa dạng (Game Modes)**:
  - Chế độ 2 người chơi (PvP đối kháng cục bộ)
  - Chế độ Chơi với máy (AI - PvE):
    - Thuật toán Minimax kết hợp Alpha-Beta Pruning
    - Đánh giá giá trị quân và bảng điểm vị trí chiến thuật (Piece-Square Tables)
    - 3 mức độ khó: Dễ (Easy), Vừa (Medium), Khó (Hard)
    - Tùy chọn chọn phe: Cầm quân Đỏ (đi trước) hoặc Cầm quân Đen (đi sau)
- [x] **Đồng hồ thi đấu (Chess Clock)**:
  - Đếm ngược thời gian thi đấu cho cả Đỏ và Đen
  - Các mốc thời gian: Vô hạn, 5 phút (Cờ chớp), 10 phút (Cờ nhanh), 15 phút (Cờ tiêu chuẩn)
  - Xử thua ngay khi hết giờ (Time Out)
- [x] **Tiện ích trong ván cờ (In-game Utilities)**:
  - Hoàn tác (Undo - lùi 1 nước trong PvP, lùi 2 nước trong đấu AI)
  - Đi tiếp (Redo - phục hồi nước đi vừa hoàn tác)
  - Ván mới (New game)
  - Xin hòa (Offer draw - máy tự động đánh giá thế cờ để đồng ý/từ chối)
  - Đầu hàng (Resign)
  - Lưu ván cờ (Save game ra file `savegame.txt`)
  - Tải ván cờ (Load game để tiếp tục chơi)
  - Bảng lịch sử nước đi trực quan (Move History)
  - Highlight nước đi vừa đi (Last Move) và viền đỏ cảnh báo khi Tướng bị chiếu
- [x] **Giao diện Cờ Tướng truyền thống (Authentic Board Visuals)**:
  - Bàn cờ gỗ giao điểm 10x9 chuẩn mực với viền đôi
  - Sông Sở Hà - Hán Giới ngăn cách hai bên
  - Cung Cửu Cung có 2 đường chéo X
  - Quân cờ đĩa gỗ 2 lớp sắc nét với chữ tiếng Việt rõ ràng
- [x] **Màn hình Cài đặt & Hướng dẫn (Settings & Tutorial UI)**:
  - Menu Cài đặt: Âm thanh, Âm lượng, Thời gian, Độ khó AI, Gợi ý nước đi
  - Menu Hướng dẫn: Chi tiết luật chơi, cách đi của từng quân, phím tắt

## Ý tưởng phát triển tương lai 💡

- [ ] Kết nối mạng LAN / Online Multiplayer
- [ ] Chế độ giải cờ thế (Puzzle mode)
- [ ] Animation di chuyển quân mượt mà (Smooth interpolation)
- [ ] Theme giao diện thay đổi theo mùa
