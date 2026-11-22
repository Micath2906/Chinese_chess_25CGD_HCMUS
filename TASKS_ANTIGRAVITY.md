# TASKS_ANTIGRAVITY.md — Refactor assets theo hướng OOP cho đồ án Cờ Tướng (OOP2, 25CGD HCMUS)

> Dán nguyên file này cho Antigravity (hoặc đặt ở thư mục gốc repo và bảo agent đọc nó trước khi làm).
> Các nhận xét dưới đây đến từ việc đọc tĩnh code và kiểm tra file, **chưa build/chạy game**. Agent phải tự xác minh lại từng điểm trước khi sửa, và báo lại nếu điểm nào không đúng.

## 0. Bối cảnh & ràng buộc

- Dự án: Cờ Tướng, C++17, SFML 2.6.2, CMake, Windows x64. Nộp môn **Phương pháp Lập trình Hướng đối tượng**, nên mọi thay đổi phải thể hiện rõ OOP: đóng gói, trừu tượng, đa hình, và design pattern dùng có lý do.
- **KHÔNG** sửa luật chơi và logic: `BanCo`, `QuanCo` và 7 lớp quân, `ChessAI`, các hàm kiểm tra nước đi. Chỉ đụng tới phần nạp/quản lý tài nguyên, cấu trúc thư mục, tài liệu, repo hygiene.
- **KHÔNG** bịa nguồn gốc/giấy phép của asset. Chỗ nào chưa biết thì để `TODO(user)` và liệt kê lại cho người dùng điền.
- **KHÔNG** tự tạo hoặc thay asset bằng ảnh AI mới. Nếu cần asset thay thế, chỉ đề xuất, không tự thêm.
- Mỗi task = 1 commit riêng, message rõ ràng. Sau mỗi task phải build được.
- Làm theo thứ tự T1 → T9. Mỗi task có **Tiêu chí hoàn thành**; chưa đạt thì không chuyển task.

---

## T1. Gộp `assets/` và `resources/` thành MỘT thư mục duy nhất

**Vấn đề:** 5 file trùng hệt nhau giữa hai thư mục (đã đối chiếu md5): `NomNaTong-Regular.ttf` (15,5 MB × 2), `bancofinal.png` = `ban_co.png`, `ViaodaLibre-Regular.ttf`, `PixeloidSans.ttf`, `PixeloidSansBold.ttf`. CMake copy cả hai thư mục vào build.

**Việc cần làm:**
1. Chọn `assets/` làm thư mục duy nhất, cấu trúc đề xuất:
   ```
   assets/
   ├── fonts/        (ViaodaLibre, NomNaTong, PixeloidSans, PixeloidSansBold)
   ├── textures/
   │   ├── backgrounds/   (xiangqi_background_pavilion.png)
   │   ├── board/         (bancofinal.png, đổi tên thành board.png)
   │   ├── pieces/        (14 file r_*.png, b_*.png)
   │   ├── avatars/       (5 file emoji_cho_ava_*.png)
   │   └── ui/            (xiangqi_oriental_sheet.png + sprite TravelBook đang dùng)
   └── audio/
       ├── sfx/           (select, move, capture, check, win)
       └── music/         (Golden Gleam.ogg → đổi tên không dấu cách: golden_gleam.ogg)
   ```
2. Di chuyển bằng `git mv` để giữ lịch sử. Xóa hẳn các bản trùng trong `resources/`, rồi xóa thư mục `resources/`.
3. Âm thanh: `move`/`capture` đang có cả `.ogg` (`dat_co`, `an_co`) lẫn `.wav`. Giữ **một định dạng cho mỗi âm** (ưu tiên `.ogg` cho `dat_co` → move, `an_co` → capture), không giữ bản dự phòng khác định dạng.
4. Với UI pack TravelBook: chỉ giữ 26 sprite đang được `Menu.cpp` dùng, đưa vào `assets/textures/ui/travelbook/` và rút ngắn đường dẫn quá dài (`Complete_UI_Book_Styles_Pack_Free/.../Sprites/`). Giữ lại file license của pack nếu có (xem T5).
5. Cập nhật `CMakeLists.txt`: chỉ copy `assets/` vào thư mục build (bỏ khối copy `resources`), và bỏ dòng `file(COPY ... resources)`.

**Xử lý đặc biệt:**
- `resources/textures/board/board.png` (694×754) không được code tham chiếu → xóa (kiểm tra bằng grep trước).
- `resources/textures/background_game.png` (1,2 MB) chỉ là fallback → xóa.
- `resources/fonts/arial.ttf` (1 MB): Arial là font độc quyền của Microsoft, **không nên phân phối trong repo**. Xóa nó, và chọn font fallback thay thế phải hỗ trợ đủ dấu tiếng Việt (kiểm tra độ phủ glyph của ViaodaLibre/NomNaTong; nếu thiếu thì đề xuất font OFL như Noto Sans hoặc Roboto và ghi vào CREDITS ở T5, **hỏi người dùng trước khi thêm**).
- `resources/fonts/README.txt` (hướng dẫn thêm arial.ttf) xóa luôn.

**Tiêu chí hoàn thành:** không còn thư mục `resources/`; `git ls-files | xargs md5sum | sort | uniq -d -w32` không còn file trùng nội dung; build xong, game chạy hiển thị đủ menu, bàn cờ, quân cờ, avatar, nghe được tiếng nước đi.

---

## T2. Tạo `AssetManager` (trọng tâm OOP của đợt sửa này)

**Vấn đề:** logic dò đường dẫn (`prefixes = {"", "../", "../../", ...}` + `fileExistsUtf8`/`std::filesystem::exists`) bị lặp ở `Menu.cpp`, `FantasyUI.cpp`, `GameManager.cpp`, `SoundManager.cpp` (~19 chỗ). Không có lớp quản lý tài nguyên tập trung dù README khoe nhiều design pattern.

**Thiết kế yêu cầu (nêu rõ pattern trong comment đầu file và README):**
- `class AssetManager` — **Singleton** (Meyers singleton, xóa copy/move) đóng vai **Facade** cho toàn bộ việc nạp tài nguyên.
- `template <typename Resource, typename Id> class ResourceCache` — **Flyweight/Cache**: mỗi tài nguyên chỉ nạp **một lần**, các nơi khác dùng chung qua `const Resource&`. Lưu bằng `std::unordered_map<Id, std::unique_ptr<Resource>>` (đảm bảo con trỏ/tham chiếu không bị vô hiệu khi map rehash).
- Định danh tài nguyên bằng `enum class` (ví dụ `TextureId`, `FontId`, `SoundId`, `MusicId`), **không** truyền chuỗi đường dẫn rải rác trong code game.
- Một file `AssetPaths.h` duy nhất ánh xạ `Id → đường dẫn tương đối trong assets/` (`constexpr`/bảng tĩnh). Đây là nơi duy nhất chứa đường dẫn.
- **Xác định thư mục gốc assets đúng một lần** khi khởi động: thử lần lượt vài vị trí (cạnh exe, thư mục cha, thư mục làm việc), lưu lại kết quả. Bỏ hoàn toàn kiểu dò prefix ở từng lần load.
- Quy tắc RAII: không `new/delete` thủ công; lỗi load → ghi log rõ ràng (id + đường dẫn đã thử), không crash im lặng.
- API gợi ý: `const sf::Texture& texture(TextureId)`, `const sf::Font& font(FontId)`, `const sf::SoundBuffer& sound(SoundId)`, `bool validateAll(std::vector<std::string>& missing)`.
- Việc hậu xử lý ảnh (tạo mask trong suốt, `setSmooth`) đặt vào **loader policy riêng** của từng loại tài nguyên bên trong `AssetManager`, không để rải trong `GameManager`.

**Việc cần làm:**
1. Viết `include/AssetManager.h`, `include/AssetPaths.h`, `src/AssetManager.cpp` (CMake đã `GLOB` `src/*.cpp` nên tự nhận).
2. Thay toàn bộ các hàm load cục bộ (`loadTexSafe`, `loadFontSafe`, `tryLoadTextureCandidates`, `tryLoadImageCandidates`, `loadPieceSafe`, `loadSoundFile`, `taiAvatarTextures`…) bằng lời gọi `AssetManager`.
3. `SoundManager` lấy `SoundBuffer` từ `AssetManager`, vẫn giữ tính Singleton riêng của nó.
4. Xóa các hàm helper cũ sau khi thay xong; `grep` không còn chuỗi `prefixes` hay `"../../"` trong `src/` và `include/`.
5. Khi khởi động gọi `validateAll()`; nếu thiếu asset thì hiện thông báo lỗi dễ hiểu (log + màn hình lỗi đơn giản hoặc `std::cerr`) rồi thoát có kiểm soát, thay vì chạy tiếp với màn hình trống.

**Tiêu chí hoàn thành:** mọi truy cập asset đi qua `AssetManager`; không còn đường dẫn chuỗi cứng ngoài `AssetPaths.h`; game chạy giống hệt trước; chạy thử từ 2 working directory khác nhau (thư mục gốc repo và thư mục build) đều thấy đủ asset; cố tình xóa một asset thì có thông báo lỗi rõ ràng.

---

## T3. Dọn code chết & magic number trong `FantasyUI`

**Vấn đề:**
- Constructor `FantasyUI::FantasyUI()` gán các `IntRect` kiểu cũ (`679,382,59,148`…) rồi `init()` ghi đè hết bằng giá trị mới → nửa code là code chết.
- Toàn bộ vùng cắt spritesheet là số cứng rải trong `init()`.
- `GameManager::taiTextures()` quét ~1 triệu pixel của bàn cờ bằng `getPixel/setPixel` để tạo mask, trong khi `bancofinal.png` **đã có kênh alpha thật** (kiểm tra: 0% pixel bị mask) nên vòng lặp này thừa.

**Việc cần làm:**
1. Xóa các `IntRect` cũ không còn dùng trong constructor (kiểm tra bằng grep các biến `rcBtnSlimNorm`, `rcSqNorm`, … còn được tham chiếu ở đâu; giữ alias nào còn dùng thật, xóa phần còn lại).
2. Gom tọa độ spritesheet vào một nơi: `include/UIAtlas.h` với `struct AtlasRegion { int x, y, w, h; }` và các hằng `constexpr` đặt tên rõ nghĩa (`PanelEbony`, `Parchment`, `RibbonHeader`, `ButtonNormal`, `ButtonHover`, `ButtonPressed`, `IconUndo`, `IconRedo`, `IconDraw`, `IconResign`, `IncenseStick`). `FantasyUI` chỉ chuyển `AtlasRegion` sang `sf::IntRect`.
3. Bỏ vòng lặp mask thừa cho bàn cờ; chỉ giữ bước tạo mask khi ảnh **thực sự không có alpha** (và đặt bước này trong loader policy ở T2).
4. Cắt `xiangqi_oriental_sheet.png` còn khoảng 640×400 (giữ nguyên gốc tọa độ góc trên-trái nên các vùng cắt không đổi) để giảm vùng trống. Không bắt buộc nếu làm lệch tọa độ.

**Tiêu chí hoàn thành:** `FantasyUI.cpp` không còn số cứng kích thước sprite ngoài `UIAtlas.h`; giao diện nút/panel/ribbon hiển thị y hệt trước; thời gian khởi động không tệ hơn (nên nhanh hơn).

---

## T4. Tối ưu dung lượng & hiển thị

1. Nén lại `golden_gleam.ogg` (hiện ~397 kbps, 6,6 MB, dài 2:17) xuống khoảng 128 kbps (khoảng 2 MB) bằng `ffmpeg`/`oggenc`; giữ bản gốc ngoài repo, đừng ghi đè nếu người dùng chưa đồng ý. Báo lại dung lượng trước/sau.
2. Kiểm tra cách game xử lý **phóng to cửa sổ/fullscreen**: README quảng cáo 1920×1080 nhưng asset ở mức ~1200×800. Ưu tiên giải pháp đóng gói đơn giản: dùng `sf::View` với độ phân giải logic cố định 1200×800 + letterbox, không kéo giãn méo. Không tự vẽ lại asset.
3. Báo cáo kích thước repo trước/sau T1–T4 (`du -sh assets .git`, tổng dung lượng thư mục làm việc).

**Tiêu chí hoàn thành:** nhạc nền nghe bình thường, không giật; tổng dung lượng `assets/` giảm rõ rệt so với 24 MB + 19 MB ban đầu.

---

## T5. Bổ sung `LICENSE` và `CREDITS.md` (không bịa thông tin)

**Vấn đề:** repo không có `LICENSE`/`CREDITS`. README ghi MIT cho cả dự án, nhưng MIT chỉ áp dụng cho **mã nguồn của nhóm**, không áp dụng cho asset của bên thứ ba.

**Việc cần làm:**
1. Thêm file `LICENSE` (MIT) cho **mã nguồn**, ghi tên/MSSV nhóm theo `TODO(user)`.
2. Tạo `CREDITS.md` dạng bảng, mỗi asset hoặc nhóm asset một dòng: `Asset | Dùng ở đâu | Tác giả/nguồn | Link | Giấy phép | Ghi chú`. Điền đầy đủ cho:
   - Bộ UI "Complete UI Book Styles Pack Free" (TravelBook Lite)
   - Nhạc nền Golden Gleam
   - Bộ avatar `emoji_cho_ava_*`
   - Font: Viaoda Libre, Nôm Na Tống, Pixeloid Sans (+ font fallback nếu thêm ở T1)
   - Nền đình núi pixel-art, bàn cờ, 14 quân cờ, spritesheet UI, hiệu ứng âm thanh `.wav`/`.ogg`
   - Thư viện SFML 2.6.2 và các thư viện đi kèm trong `SFML/` (freetype, OpenAL, vorbis…)
3. Với mỗi dòng mà agent **không tự xác minh được từ file trong repo**, ghi `TODO(user): điền nguồn/giấy phép`. Tuyệt đối không đoán.
4. Hai asset **nền đình núi** và **bàn cờ** nhìn giống ảnh do AI tạo (nhận định qua hình ảnh, chưa xác nhận). Thêm vào CREDITS dòng `TODO(user): xác nhận có phải ảnh AI không, công cụ nào, ngày tạo`.
5. Sửa phần "Tác giả & Bản quyền" trong README: tách rõ *mã nguồn* (MIT) và *tài nguyên bên thứ ba* (xem CREDITS.md).
6. Cuối task, xuất danh sách tất cả `TODO(user)` để người dùng tự điền.

**Tiêu chí hoàn thành:** mọi asset trong `assets/` đều có mặt trong CREDITS (dù còn TODO); không có thông tin nguồn nào do agent tự bịa.

---

## T6. Đồng bộ tài liệu với code

Sửa các chỗ lệch đã phát hiện:
1. **README.md** — cây thư mục: bỏ `resources/`, bỏ `background play.jpg` (không tồn tại), bỏ `CoTuong.exe` khỏi cây nếu file không nằm trong repo (hoặc ghi rõ "tải ở mục Releases"); cập nhật cấu trúc `assets/` mới và thêm `AssetManager.h`, `AssetPaths.h`, `UIAtlas.h`.
2. **TODO.md** — mục "Hệ thống Âm thanh" đang ghi "sinh âm thanh thời gian thực 100% C++, không dùng file" nhưng code thật load `.ogg`/`.wav` → sửa cho đúng.
3. README quảng cáo 3 kiểu quân (Realistic Wood / Traditional Hanzi / Modern Alphabet), trong khi `assets/` chỉ có một bộ texture quân. **Xác minh trong code** các kiểu này có thật không (có thể vẽ bằng code): nếu không có thì sửa README, nếu có thì ghi rõ cách hiện thực.
4. Mô tả "tiếng đàn tranh Guzheng" trong README: chỉ giữ nếu đúng với file nhạc thật; nếu không chắc thì dùng mô tả trung tính.
5. Thêm mục **"Quản lý tài nguyên (Resource Management)"** vào phần Kiến trúc OOP & Design Patterns của README: giải thích Singleton + Flyweight + Facade của `AssetManager`, kèm sơ đồ Mermaid (class diagram) các lớp `AssetManager`, `ResourceCache<T>`, `AssetPaths`, và các nơi dùng (`Menu`, `GameManager`, `SoundManager`, `FantasyUI`).

**Tiêu chí hoàn thành:** mọi đường dẫn, tên file, tính năng nêu trong README/TODO đều khớp repo thật.

---

## T7. Vệ sinh repo

1. `SFML/` nặng khoảng 60 MB gồm cả `.pdb`/`.lib` bản Debug. **Không xóa** thư viện cần để build Release. Làm tối thiểu: `git rm --cached` các `SFML/lib/Debug/*.pdb` và thêm vào `.gitignore`. Nếu muốn gọn hơn nữa, đề xuất (không tự làm) chuyển sang `FetchContent`/vcpkg và hỏi người dùng.
2. Kiểm tra `main.zip` (~124 KB) ở thư mục gốc: nếu là bản sao thừa của mã nguồn thì xóa; nếu không rõ thì **hỏi lại người dùng**.
3. Dòng `# Resources (if large)` trong `.gitignore` cập nhật cho khớp cấu trúc mới.
4. Kiểm tra `ChayGame.bat`, `build.bat`, `INSTALLATION.md`, `CMakeSettings.json` có tham chiếu `resources/` không và sửa cho khớp.

**Tiêu chí hoàn thành:** `git status` sạch; script chạy/build vẫn hoạt động; không còn file rác lớn không cần thiết.

---

## T8. Kiểm thử asset (tăng điểm "chuyên nghiệp")

1. Thêm một chế độ kiểm tra nhẹ, ví dụ tham số dòng lệnh `--check-assets` (hoặc một target CMake riêng `AssetCheck`) gọi `AssetManager::validateAll()` và in danh sách asset thiếu, trả về mã thoát khác 0 nếu thiếu.
2. Thêm test thủ công vào `INSTALLATION.md`: các bước chạy `--check-assets` và kết quả mong đợi.

**Tiêu chí hoàn thành:** chạy `CoTuong.exe --check-assets` in `OK` khi đủ asset và liệt kê file thiếu khi cố tình xóa một asset.

---

## T9. Nghiệm thu cuối

Agent phải thực hiện và báo cáo từng mục, **không được tuyên bố xong khi chưa chạy**:

- [ ] Build `x64-Release` và `x64-Debug` bằng CMake thành công, không warning mới.
- [ ] Chạy game từ thư mục gốc repo **và** từ thư mục build: đủ nền, menu, nút, icon, bàn cờ, 14 quân, avatar, font tiếng Việt có dấu, âm thanh nước đi/ăn quân/chiếu/thắng, nhạc nền.
- [ ] Chơi nhanh PvP và PvAI vài nước, Undo/Redo, Save/Load vẫn hoạt động (không đụng logic nhưng phải kiểm tra không vỡ).
- [ ] `grep -rn '"\.\./' src include` không còn kết quả; `grep -rn 'resources/' .` chỉ còn trong lịch sử/ghi chú hợp lý.
- [ ] Không còn file trùng nội dung trong repo.
- [ ] `CREDITS.md` đầy đủ; danh sách `TODO(user)` được liệt kê lại.
- [ ] Bảng "Trước / Sau": dung lượng repo, số file asset, số chỗ chứa logic load, số đường dẫn cứng ngoài `AssetPaths.h`.
- [ ] Báo cáo ngắn: việc đã làm, việc **không** làm được (kèm lý do), việc cần người dùng quyết định.