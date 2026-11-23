"""
Generates authentic Chinese Oriental pixel art assets for CoTuong (Chinese Chess):
1. Menu category icons (Sound, Gameplay, Language, Controls)
2. 5 Chinese character avatars (General, Strategist, Emperor, Sage, Heroine)
3. Antique Chinese Hourglass
4. Checkbox icons (checked & unchecked)
5. Board & Piece previews
"""
import os
import math
from PIL import Image, ImageDraw, ImageFont

def ensure_dir(path):
    os.makedirs(os.path.dirname(path), exist_ok=True)

def create_icon_sound(path):
    # 32x32 pixel art: Ancient Chinese Bronze Chime Bell (Biên chung) with gold highlights
    img = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    
    # Wooden crossbeam / frame hook at top
    d.rectangle([13, 2, 18, 5], fill=(160, 115, 55, 255))
    d.rectangle([14, 3, 17, 4], fill=(245, 205, 95, 255)) # Gold ring
    
    # Suspension loop
    d.rectangle([15, 5, 16, 7], fill=(210, 165, 75, 255))
    
    # Bell upper crown (narrower)
    d.rectangle([12, 7, 19, 10], fill=(185, 140, 60, 255))
    d.rectangle([13, 8, 18, 9], fill=(255, 220, 110, 255))
    
    # Bell main body trapezoid (flaring downward)
    points = [
        (10, 11), (21, 11),
        (24, 25), (7, 25)
    ]
    d.polygon(points, fill=(130, 40, 30, 255)) # Imperial vermilion-bronze core
    
    # Inner body shading
    inner_pts = [
        (11, 12), (20, 12),
        (22, 24), (9, 24)
    ]
    d.polygon(inner_pts, fill=(180, 60, 45, 255))
    
    # Golden horizontal decorative bands (Đai chuông hoàng kim)
    d.rectangle([10, 14, 21, 15], fill=(245, 205, 90, 255))
    d.rectangle([9, 19, 22, 20], fill=(245, 205, 90, 255))
    
    # Bell boss studs (Núm chuông / 钟乳)
    studs = [(12, 17), (16, 17), (19, 17), (11, 22), (15, 22), (20, 22)]
    for sx, sy in studs:
        d.rectangle([sx, sy, sx+1, sy+1], fill=(255, 240, 160, 255))
    
    # Curved bottom lip (Vành cung chuông biên đình)
    d.rectangle([6, 25, 25, 27], fill=(215, 165, 70, 255))
    d.rectangle([7, 26, 24, 27], fill=(255, 225, 120, 255))
    # Crescent cutout in bottom center
    d.rectangle([13, 26, 18, 27], fill=(0, 0, 0, 0))
    d.rectangle([14, 25, 17, 25], fill=(100, 30, 25, 255))
    
    # Sound wave vibration ripples on sides
    d.line([(3, 15), (4, 18), (3, 21)], fill=(255, 215, 100, 220), width=1)
    d.line([(28, 15), (27, 18), (28, 21)], fill=(255, 215, 100, 220), width=1)
    d.point([(2, 18), (29, 18)], fill=(255, 240, 160, 240))
    
    ensure_dir(path)
    img.save(path)
    print(f"Created {path}")

def create_icon_gameplay(path):
    # 32x32 pixel art: Xiangqi piece disc with crossed dao swords (Đao kiếm & Quân cờ)
    img = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    
    # Crossed sword 1 (Top-Left to Bottom-Right)
    d.line([(5, 5), (26, 26)], fill=(225, 235, 245, 255), width=2)
    d.line([(6, 6), (25, 25)], fill=(255, 255, 255, 255), width=1)
    # Sword 1 Hilt & Pommel
    d.rectangle([4, 4, 7, 7], fill=(215, 165, 65, 255))
    d.point([(3, 3), (2, 2)], fill=(185, 40, 30, 255)) # Red tassel
    
    # Crossed sword 2 (Top-Right to Bottom-Left)
    d.line([(26, 5), (5, 26)], fill=(225, 235, 245, 255), width=2)
    d.line([(25, 6), (6, 25)], fill=(255, 255, 255, 255), width=1)
    # Sword 2 Hilt & Pommel
    d.rectangle([24, 4, 27, 7], fill=(215, 165, 65, 255))
    d.point([(28, 3), (29, 2)], fill=(185, 40, 30, 255)) # Red tassel
    
    # Central Xiangqi Wooden Disc (Circle radius ~ 9)
    cx, cy, r = 16, 16, 9
    d.ellipse([cx-r, cy-r, cx+r, cy+r], fill=(42, 22, 14, 255), outline=(215, 165, 70, 255), width=2)
    # Inner wood disc
    d.ellipse([cx-r+2, cy-r+2, cx+r-2, cy+r-2], fill=(195, 145, 85, 255))
    d.ellipse([cx-r+3, cy-r+3, cx+r-3, cy+r-3], fill=(225, 185, 125, 255))
    
    # Engraved Red Character "帥" / "将" pixel symbol in center
    # Symmetrical calligraphy strokes
    d.rectangle([15, 11, 16, 20], fill=(195, 30, 20, 255)) # Center stem
    d.rectangle([13, 12, 18, 13], fill=(195, 30, 20, 255)) # Top bar
    d.rectangle([12, 16, 19, 17], fill=(195, 30, 20, 255)) # Mid bar
    d.point([(13, 19), (18, 19)], fill=(195, 30, 20, 255)) # Base hooks
    
    ensure_dir(path)
    img.save(path)
    print(f"Created {path}")

def create_icon_language(path):
    # 32x32 pixel art: Imperial Decree Scroll (Thánh Chỉ) & Calligraphy Brush (Bút Lông)
    img = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    
    # Unrolled Parchment Scroll Body
    d.rectangle([6, 7, 24, 23], fill=(245, 230, 185, 255), outline=(150, 105, 50, 255), width=1)
    d.rectangle([8, 8, 22, 22], fill=(255, 245, 215, 255))
    
    # Left Scroll Roller Handle
    d.rectangle([4, 5, 7, 25], fill=(185, 45, 35, 255), outline=(235, 185, 80, 255))
    d.point([(4, 4), (4, 26), (6, 4), (6, 26)], fill=(255, 215, 100, 255)) # Gold finials
    
    # Right Scroll Roller Handle
    d.rectangle([23, 5, 26, 25], fill=(185, 45, 35, 255), outline=(235, 185, 80, 255))
    d.point([(24, 4), (24, 26), (26, 4), (26, 26)], fill=(255, 215, 100, 255))
    
    # Calligraphy text lines on scroll (Simulated ink characters)
    d.line([(10, 11), (20, 11)], fill=(65, 35, 25, 230), width=1)
    d.line([(10, 14), (18, 14)], fill=(65, 35, 25, 230), width=1)
    d.line([(10, 17), (20, 17)], fill=(65, 35, 25, 230), width=1)
    # Red Imperial Vermilion Seal Stamp (Dấu Triện)
    d.rectangle([17, 18, 20, 21], fill=(205, 40, 30, 255), outline=(255, 180, 80, 255))
    
    # Diagonal Calligraphy Brush resting across corner
    d.line([(15, 28), (28, 15)], fill=(95, 55, 30, 255), width=2) # Bamboo shaft
    d.line([(16, 27), (27, 16)], fill=(160, 105, 60, 255), width=1)
    # Brush Tip (Lông bút nhúng mực)
    d.rectangle([13, 28, 15, 30], fill=(25, 20, 18, 255))
    d.point([(12, 30), (14, 29)], fill=(15, 10, 8, 255))
    
    ensure_dir(path)
    img.save(path)
    print(f"Created {path}")

def create_icon_controls(path):
    # 32x32 pixel art: Octagonal Bagua Talisman / Oriental Gamepad Key plaque
    img = Image.new("RGBA", (32, 32), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    
    # Octagonal Bronze Plaque (Bát Quái bài)
    # Cut corners: (8, 4) to (23, 4), (27, 8) to (27, 23), etc.
    poly = [
        (9, 4), (22, 4),
        (27, 9), (27, 22),
        (22, 27), (9, 27),
        (4, 22), (4, 9)
    ]
    d.polygon(poly, fill=(35, 25, 18, 255), outline=(235, 185, 80, 255))
    
    inner_poly = [
        (10, 6), (21, 6),
        (25, 10), (25, 21),
        (21, 25), (10, 25),
        (6, 21), (6, 10)
    ]
    d.polygon(inner_poly, fill=(65, 42, 28, 255), outline=(175, 130, 60, 255))
    
    # Center D-pad Cross in Imperial Jade & Gold (Bàn phím tứ phương)
    # Up
    d.rectangle([14, 8, 17, 12], fill=(225, 175, 75, 255), outline=(32, 115, 68, 255))
    d.point([(15, 9), (16, 9)], fill=(255, 240, 160, 255))
    # Down
    d.rectangle([14, 19, 17, 23], fill=(225, 175, 75, 255), outline=(32, 115, 68, 255))
    d.point([(15, 22), (16, 22)], fill=(255, 240, 160, 255))
    # Left
    d.rectangle([8, 14, 12, 17], fill=(225, 175, 75, 255), outline=(32, 115, 68, 255))
    d.point([(9, 15), (9, 16)], fill=(255, 240, 160, 255))
    # Right
    d.rectangle([19, 14, 23, 17], fill=(225, 175, 75, 255), outline=(32, 115, 68, 255))
    d.point([(22, 15), (22, 16)], fill=(255, 240, 160, 255))
    # Center Hub
    d.rectangle([13, 13, 18, 18], fill=(32, 115, 68, 255), outline=(245, 215, 95, 255))
    d.point([(15, 15), (16, 16)], fill=(120, 230, 160, 255)) # Jade center glint
    
    ensure_dir(path)
    img.save(path)
    print(f"Created {path}")

def create_chinese_avatars():
    # 5 avatars, 64x64 pixel art
    # Avatar 0: Grand Marshal (Tướng Quân)
    img0 = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d0 = ImageDraw.Draw(img0)
    
    # Outer circular frame
    d0.ellipse([2, 2, 61, 61], fill=(28, 18, 14, 255), outline=(225, 175, 75, 255), width=2)
    d0.ellipse([5, 5, 58, 58], fill=(45, 24, 20, 255))
    
    # Battle cloak shoulders (Vermilion red)
    d0.polygon([(8, 60), (22, 44), (42, 44), (56, 60)], fill=(175, 30, 25, 255))
    # Golden Armor Neck Gorget & Plates
    d0.polygon([(18, 60), (25, 46), (39, 46), (46, 60)], fill=(215, 165, 65, 255))
    d0.rectangle([28, 48, 36, 59], fill=(255, 215, 100, 255))
    # Dragon chest crest
    d0.point([(31, 52), (32, 52), (32, 53)], fill=(180, 40, 30, 255))
    
    # Face skin
    d0.rectangle([23, 26, 41, 42], fill=(240, 195, 155, 255))
    d0.polygon([(23, 38), (41, 38), (35, 45), (29, 45)], fill=(230, 180, 140, 255))
    
    # Determined warrior eyes & fierce eyebrows
    d0.line([(25, 30), (29, 32)], fill=(30, 15, 10, 255), width=2)
    d0.line([(39, 30), (35, 32)], fill=(30, 15, 10, 255), width=2)
    d0.point([(27, 33), (37, 33)], fill=(20, 10, 5, 255)) # Pupils
    
    # Handsome warrior mustache & goatee beard
    d0.polygon([(27, 40), (37, 40), (41, 44), (23, 44)], fill=(35, 22, 18, 255))
    d0.polygon([(28, 43), (36, 43), (32, 50)], fill=(30, 18, 14, 255))
    
    # Warrior Golden Helmet with Phoenix Wings
    d0.polygon([(20, 27), (44, 27), (32, 12)], fill=(215, 165, 65, 255), outline=(255, 225, 120, 255))
    d0.rectangle([21, 24, 43, 27], fill=(245, 205, 90, 255)) # Brow band with jade gem
    d0.rectangle([30, 24, 34, 27], fill=(35, 135, 75, 255)) # Center Jade jewel
    
    # Tall Red Feather Plume (Lông trĩ hùng dũng)
    d0.line([(32, 12), (32, 4)], fill=(235, 45, 35, 255), width=3)
    d0.polygon([(30, 8), (34, 8), (32, 2)], fill=(255, 70, 55, 255))
    d0.line([(32, 4), (37, 7)], fill=(255, 90, 75, 255), width=2)
    
    ensure_dir("assets/textures/avatars/chinese_ava_0.png")
    img0.save("assets/textures/avatars/chinese_ava_0.png")
    print("Created chinese_ava_0.png")
    
    # Avatar 1: Master Strategist (Quân Sư / Khổng Minh)
    img1 = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d1 = ImageDraw.Draw(img1)
    
    # Outer frame
    d1.ellipse([2, 2, 61, 61], fill=(16, 22, 32, 255), outline=(130, 180, 235, 255), width=2)
    d1.ellipse([5, 5, 58, 58], fill=(24, 34, 52, 255))
    
    # Deep blue scholar robe
    d1.polygon([(8, 60), (22, 44), (42, 44), (56, 60)], fill=(35, 55, 95, 255))
    d1.polygon([(24, 60), (28, 46), (36, 46), (40, 60)], fill=(215, 230, 255, 255)) # White inner collar
    d1.line([(32, 46), (32, 60)], fill=(120, 160, 220, 255), width=2)
    
    # Face skin
    d1.rectangle([24, 26, 40, 42], fill=(245, 205, 170, 255))
    
    # Calm, wise eyes & gentle smile
    d1.line([(26, 32), (30, 32)], fill=(30, 20, 15, 255), width=2)
    d1.line([(34, 32), (38, 32)], fill=(30, 20, 15, 255), width=2)
    d1.point([(28, 33), (36, 33)], fill=(15, 10, 5, 255))
    
    # Neat elegant goatee beard
    d1.polygon([(29, 42), (35, 42), (32, 51)], fill=(35, 25, 20, 255))
    
    # Scholar Cap (Khăn Tiêu Dao / Khổng Minh Quán)
    d1.polygon([(21, 26), (43, 26), (41, 12), (23, 12)], fill=(20, 30, 50, 255), outline=(145, 190, 245, 255))
    d1.rectangle([21, 24, 43, 26], fill=(120, 165, 225, 255))
    # Jade ornament on cap
    d1.rectangle([30, 16, 34, 20], fill=(45, 155, 90, 255), outline=(245, 225, 120, 255))
    # White feather fan peeking out at bottom right
    d1.polygon([(46, 58), (56, 44), (60, 52)], fill=(245, 250, 255, 255), outline=(180, 200, 225, 255))
    d1.line([(50, 56), (56, 48)], fill=(195, 160, 95, 255), width=1) # Fan handle
    
    img1.save("assets/textures/avatars/chinese_ava_1.png")
    print("Created chinese_ava_1.png")

    # Avatar 2: Dragon Emperor (Hoàng Đế)
    img2 = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d2 = ImageDraw.Draw(img2)
    
    d2.ellipse([2, 2, 61, 61], fill=(32, 22, 10, 255), outline=(255, 215, 80, 255), width=2)
    d2.ellipse([5, 5, 58, 58], fill=(65, 40, 16, 255))
    
    # Imperial Yellow Robe with Red Dragons
    d2.polygon([(8, 60), (22, 44), (42, 44), (56, 60)], fill=(245, 190, 45, 255))
    d2.polygon([(24, 60), (28, 46), (36, 46), (40, 60)], fill=(185, 30, 25, 255)) # Vermilion dragon trim
    d2.circle((32, 54), 4, fill=(255, 235, 110, 255), outline=(175, 40, 20, 255)) # Dragon pearl
    
    # Dignified face
    d2.rectangle([24, 26, 40, 42], fill=(245, 205, 165, 255))
    # Regal noble eyes
    d2.line([(26, 31), (30, 31)], fill=(30, 15, 10, 255), width=2)
    d2.line([(34, 31), (38, 31)], fill=(30, 15, 10, 255), width=2)
    # Royal mustache
    d2.line([(27, 39), (37, 39)], fill=(40, 25, 15, 255), width=2)
    d2.polygon([(30, 41), (34, 41), (32, 47)], fill=(40, 25, 15, 255))
    
    # Imperial Crown (Miện Quan / 冕冠) - Flat board with hanging jade bead strings
    d2.rectangle([14, 12, 50, 16], fill=(25, 18, 12, 255), outline=(255, 215, 90, 255))
    d2.rectangle([24, 16, 40, 25], fill=(195, 35, 25, 255), outline=(255, 215, 90, 255))
    # Hanging bead strings (Lưu thùy ngọc châu)
    for bx in [18, 22, 26, 32, 38, 42, 46]:
        d2.line([(bx, 16), (bx, 24)], fill=(120, 225, 150, 240), width=1)
        d2.point([(bx, 24)], fill=(245, 40, 30, 255)) # Red bead at tip
        
    img2.save("assets/textures/avatars/chinese_ava_2.png")
    print("Created chinese_ava_2.png")

    # Avatar 3: Ancient Chess Sage (Lão Kỳ Thánh)
    img3 = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d3 = ImageDraw.Draw(img3)
    
    d3.ellipse([2, 2, 61, 61], fill=(24, 26, 22, 255), outline=(190, 175, 140, 255), width=2)
    d3.ellipse([5, 5, 58, 58], fill=(42, 46, 38, 255))
    
    # Sage linen robe (Grey-green hemp)
    d3.polygon([(8, 60), (22, 44), (42, 44), (56, 60)], fill=(95, 105, 90, 255))
    d3.polygon([(25, 60), (28, 46), (36, 46), (39, 60)], fill=(225, 220, 205, 255))
    
    # Face skin with aged character
    d3.rectangle([24, 26, 40, 40], fill=(235, 195, 160, 255))
    
    # Smiling squinting kind eyes of an immortal
    d3.line([(26, 31), (30, 30)], fill=(50, 35, 25, 255), width=1)
    d3.line([(34, 30), (38, 31)], fill=(50, 35, 25, 255), width=1)
    # Long flowing white eyebrows
    d3.line([(24, 28), (30, 27)], fill=(255, 255, 255, 255), width=2)
    d3.line([(34, 27), (40, 28)], fill=(255, 255, 255, 255), width=2)
    
    # Magnificent flowing white beard down to chest
    d3.polygon([(24, 38), (40, 38), (36, 58), (28, 58)], fill=(245, 245, 245, 255))
    d3.polygon([(27, 40), (37, 40), (34, 56), (30, 56)], fill=(255, 255, 255, 255))
    
    # White hair bun with green jade pin
    d3.ellipse([26, 12, 38, 24], fill=(235, 235, 235, 255))
    d3.line([(22, 18), (42, 18)], fill=(45, 165, 85, 255), width=2) # Jade hairpin
    
    img3.save("assets/textures/avatars/chinese_ava_3.png")
    print("Created chinese_ava_3.png")

    # Avatar 4: Martial Heroine (Nữ Hiệp)
    img4 = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    d4 = ImageDraw.Draw(img4)
    
    d4.ellipse([2, 2, 61, 61], fill=(28, 16, 22, 255), outline=(235, 145, 165, 255), width=2)
    d4.ellipse([5, 5, 58, 58], fill=(52, 26, 38, 255))
    
    # Silk martial tunic (Crimson & black combat attire)
    d4.polygon([(8, 60), (23, 44), (41, 44), (56, 60)], fill=(165, 30, 50, 255))
    d4.polygon([(26, 60), (29, 46), (35, 46), (38, 60)], fill=(35, 25, 30, 255)) # Black collar
    d4.line([(28, 48), (36, 48)], fill=(245, 205, 90, 255), width=2) # Gold clasp
    
    # Fair feminine face skin
    d4.rectangle([25, 27, 39, 41], fill=(250, 215, 190, 255))
    d4.polygon([(25, 37), (39, 37), (35, 44), (29, 44)], fill=(245, 205, 180, 255))
    
    # Sharp, elegant phoenix eyes (Mắt phượng sắc sảo)
    d4.line([(26, 32), (30, 31)], fill=(40, 20, 25, 255), width=2)
    d4.line([(34, 31), (38, 32)], fill=(40, 20, 25, 255), width=2)
    d4.point([(29, 32), (35, 32)], fill=(20, 10, 15, 255))
    # Red vermilion lip
    d4.point([(32, 40)], fill=(215, 45, 55, 255))
    # Brow vermilion mark (Hoa điền đỏ trên trán)
    d4.point([(32, 28)], fill=(225, 35, 45, 255))
    
    # High warrior ponytail with red ribbon & silver hairpin
    d4.ellipse([24, 18, 40, 28], fill=(25, 18, 22, 255))
    d4.polygon([(30, 18), (34, 18), (32, 6)], fill=(25, 18, 22, 255))
    # Red ribbon ties
    d4.line([(26, 12), (38, 12)], fill=(215, 40, 50, 255), width=2)
    d4.line([(38, 12), (44, 20)], fill=(215, 40, 50, 255), width=2) # Flowing ribbon
    d4.line([(24, 14), (40, 10)], fill=(225, 235, 245, 255), width=1) # Silver hairpin
    
    img4.save("assets/textures/avatars/chinese_ava_4.png")
    print("Created chinese_ava_4.png")

def create_chinese_hourglass(path):
    # 72x120 pixel art: Antique Chinese Carved Rosewood & Gold Hourglass
    img = Image.new("RGBA", (72, 120), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    
    # Top Carved Base (Mái đình cổ phong)
    d.polygon([(10, 14), (62, 14), (58, 22), (14, 22)], fill=(65, 28, 16, 255), outline=(225, 175, 70, 255))
    d.rectangle([16, 8, 56, 14], fill=(85, 36, 20, 255), outline=(245, 205, 90, 255))
    d.point([(10, 12), (62, 12)], fill=(255, 225, 110, 255)) # Upturned eaves tips
    d.rectangle([30, 4, 42, 8], fill=(215, 165, 65, 255)) # Center finial
    
    # Bottom Carved Base (Chân đế chạm rồng)
    d.polygon([(14, 98), (58, 98), (62, 106), (10, 106)], fill=(65, 28, 16, 255), outline=(225, 175, 70, 255))
    d.rectangle([16, 106, 56, 112], fill=(85, 36, 20, 255), outline=(245, 205, 90, 255))
    d.point([(10, 108), (62, 108)], fill=(255, 225, 110, 255))
    
    # Twin Ornate Side Pillars (Cột đồng mạ vàng chạm khắc)
    # Left pillar
    d.rectangle([15, 22, 19, 98], fill=(185, 135, 55, 255), outline=(115, 75, 30, 255))
    d.line([(17, 23), (17, 97)], fill=(255, 225, 125, 255)) # Highlight
    # Right pillar
    d.rectangle([53, 22, 57, 98], fill=(185, 135, 55, 255), outline=(115, 75, 30, 255))
    d.line([(55, 23), (55, 97)], fill=(255, 225, 125, 255))
    
    # Glass Ampoule (Thủy tinh hồ lô)
    # Upper Bulb
    upper_bulb = [(22, 24), (50, 24), (39, 58), (33, 58)]
    d.polygon(upper_bulb, fill=(210, 235, 255, 50), outline=(180, 220, 250, 180))
    # Lower Bulb
    lower_bulb = [(33, 62), (39, 62), (50, 96), (22, 96)]
    d.polygon(lower_bulb, fill=(210, 235, 255, 50), outline=(180, 220, 250, 180))
    
    # Golden Sand in Upper Bulb
    upper_sand = [(25, 36), (47, 36), (37, 57), (35, 57)]
    d.polygon(upper_sand, fill=(235, 180, 65, 255))
    d.polygon([(26, 37), (46, 37), (36, 52), (36, 52)], fill=(255, 215, 95, 255))
    
    # Flowing Sand Stream through waist
    d.line([(36, 57), (36, 82)], fill=(255, 225, 110, 255), width=2)
    d.point([(36, 70), (36, 76)], fill=(255, 250, 180, 255))
    
    # Sand Mound in Lower Bulb
    lower_sand = [(36, 80), (48, 95), (24, 95)]
    d.polygon(lower_sand, fill=(235, 175, 60, 255))
    d.polygon([(36, 82), (44, 94), (28, 94)], fill=(255, 210, 85, 255))
    
    # Glass Specular Reflections
    d.line([(24, 28), (27, 45)], fill=(255, 255, 255, 160), width=1)
    d.line([(25, 75), (28, 92)], fill=(255, 255, 255, 140), width=1)
    
    ensure_dir(path)
    img.save(path)
    print(f"Created {path}")

def create_checkboxes():
    # Checked (24x24): Lacquer box with gold trim & emerald jade checkmark
    img_c = Image.new("RGBA", (24, 24), (0, 0, 0, 0))
    dc = ImageDraw.Draw(img_c)
    dc.rectangle([2, 2, 21, 21], fill=(22, 16, 12, 255), outline=(225, 175, 75, 255), width=1)
    dc.rectangle([4, 4, 19, 19], fill=(18, 38, 26, 255), outline=(65, 135, 85, 200))
    # Emerald Checkmark
    dc.line([(6, 12), (10, 17)], fill=(90, 230, 135, 255), width=2)
    dc.line([(10, 17), (18, 6)], fill=(90, 230, 135, 255), width=2)
    dc.line([(10, 17), (17, 6)], fill=(180, 255, 205, 255), width=1) # Glint
    ensure_dir("assets/textures/ui/checkbox_checked.png")
    img_c.save("assets/textures/ui/checkbox_checked.png")
    print("Created checkbox_checked.png")

    # Unchecked (24x24): Lacquer box with gold trim, dark interior
    img_u = Image.new("RGBA", (24, 24), (0, 0, 0, 0))
    du = ImageDraw.Draw(img_u)
    du.rectangle([2, 2, 21, 21], fill=(22, 16, 12, 255), outline=(160, 120, 60, 255), width=1)
    du.rectangle([4, 4, 19, 19], fill=(32, 24, 18, 255), outline=(75, 55, 35, 180))
    img_u.save("assets/textures/ui/checkbox_unchecked.png")
    print("Created checkbox_unchecked.png")

if __name__ == "__main__":
    create_icon_sound("assets/textures/ui/icon_sound.png")
    create_icon_gameplay("assets/textures/ui/icon_gameplay.png")
    create_icon_language("assets/textures/ui/icon_language.png")
    create_icon_controls("assets/textures/ui/icon_controls.png")
    create_chinese_avatars()
    create_chinese_hourglass("assets/textures/ui/chinese_hourglass.png")
    create_checkboxes()
