# TODO - Danh sách tính năng cần bổ sung

## Đã hoàn thành ✅

- [x] Cấu trúc OOP cơ bản với QuanCo abstract class
- [x] Tất cả 7 loại quân cờ (Xe, Mã, Voi, Phao, Tướng, Sĩ, Tốt)
- [x] Luật di chuyển đầy đủ cho từng quân
- [x] Bàn cờ 10x9 với quản lý trạng thái
- [x] Menu chính với SFML
- [x] Giao diện đồ họa cơ bản
- [x] Highlight quân được chọn
- [x] Hiển thị nước đi hợp lệ
- [x] Kiểm tra chiếu tướng
- [x] Chế độ 2 người chơi
- [x] Hoàn tác nước đi (Undo)

## Đang phát triển 🚧

### Ưu tiên cao

- [ ] **Load font fallback**: Xử lý khi không tìm thấy font
- [ ] **Test đầy đủ các luật**: Kiểm tra tất cả edge cases
- [ ] **Chiếu hết/Bí**: Phát hiện kết thúc ván cờ đầy đủ
- [ ] **Âm thanh**: 
  - Di chuyển quân
  - Ăn quân
  - Chiếu tướng
  - Thắng/thua

### Ưu tiên trung bình

- [ ] **Lưu/Load game**:
  - Lưu ván cờ ra file
  - Load ván cờ đã lưu
  - Format: JSON hoặc FEN notation
  
- [ ] **Lịch sử nước đi**:
  - Hiển thị danh sách nước đi
  - Phát lại ván cờ
  - Export sang file

- [ ] **Đồng hồ thi đấu**:
  - Đếm ngược thời gian
  - Tự động chuyển lượt
  - Cài đặt thời gian

- [ ] **Animation**:
  - Smooth movement của quân cờ
  - Hiệu ứng khi ăn quân
  - Particle effects

### Ưu tiên thấp

- [ ] **AI (Chơi với máy)**:
  - Minimax algorithm
  - Alpha-beta pruning
  - Độ khó: Dễ, Trung bình, Khó
  
- [ ] **Chế độ chơi đặc biệt**:
  - Cờ chấp (handicap)
  - Cờ nhanh (blitz)
  - Cờ chớp (bullet)

- [ ] **Online multiplayer**:
  - Kết nối qua mạng
  - Matchmaking
  - Chat

- [ ] **Thống kê**:
  - Win/loss ratio
  - Leaderboard
  - Replay gallery

## Cải thiện kỹ thuật 🔧

### Code quality

- [ ] **Unit tests**: Viết test cho logic game
- [ ] **Documentation**: Bổ sung Doxygen comments
- [ ] **Refactoring**: Tối ưu code, giảm coupling
- [ ] **Design patterns**: Áp dụng Observer, Strategy, Factory

### Performance

- [ ] **Optimize rendering**: Chỉ vẽ khi cần thiết
- [ ] **Move validation caching**: Cache các nước đi hợp lệ
- [ ] **Memory profiling**: Kiểm tra memory leaks

### UX/UI

- [ ] **Responsive design**: Thay đổi kích thước window
- [ ] **Themes**: Light/Dark mode, custom colors
- [ ] **Settings persistence**: Lưu cài đặt người dùng
- [ ] **Localization**: Đa ngôn ngữ (EN/VI)
- [ ] **Tutorial**: Hướng dẫn chơi cho người mới

## Bug cần fix 🐛

- [ ] Kiểm tra tất cả edge cases của từng quân
- [ ] Xử lý khi font không load được
- [ ] Validate nước đi trong trường hợp chiếu tướng
- [ ] Fix memory management với smart pointers

## Ideas 💡

- [ ] Replay famous games
- [ ] Puzzle mode
- [ ] Training mode với hints
- [ ] 3D board rendering
- [ ] VR support (tương lai xa)

---

**Ghi chú**: Đánh dấu [x] khi hoàn thành một task
