#include "FantasyUI.h"
#include "Menu.h"
#include <algorithm>
#include <cmath>

static bool loadTexSafe(sf::Texture& tex, const std::string& path, bool smooth = false) {
    const std::vector<std::string> prefixes = { "", "../", "../../" };
    for (const auto& p : prefixes) {
        sf::Image img;
        if (img.loadFromFile(p + path)) {
            bool hasAlpha = false;
            sf::Vector2u sz = img.getSize();
            for (unsigned int y = 0; y < sz.y && !hasAlpha; ++y) {
                for (unsigned int x = 0; x < sz.x; ++x) {
                    if (img.getPixel(x, y).a < 250) {
                        hasAlpha = true;
                        break;
                    }
                }
            }
            if (!hasAlpha) {
                img.createMaskFromColor(sf::Color::White);
            }
            tex.loadFromImage(img);
            tex.setSmooth(smooth);
            return true;
        }
    }
    return false;
}

FantasyUI& FantasyUI::get() {
    static FantasyUI instance;
    if (!instance.loaded) {
        instance.init();
    }
    return instance;
}

FantasyUI::FantasyUI() {
    rcBtnNorm     = sf::IntRect(679, 382, 59, 148);
    rcBtnHov      = sf::IntRect(800, 382, 58, 148);
    rcBtnSlimNorm = sf::IntRect(679, 382, 59, 148);
    rcBtnSlimHov  = sf::IntRect(800, 382, 58, 148);
    rcSqNorm      = sf::IntRect(679, 382, 59, 59);
    rcSqHov       = sf::IntRect(800, 382, 58, 58);

    rcPanelOrnate = sf::IntRect(894, 220, 104, 145);
    rcSlotWood    = sf::IntRect(692, 780, 305, 126);
    rcRibbon      = sf::IntRect(551, 46, 79, 171);
}

bool FantasyUI::init() {
    sf::Image masterImg;
    const std::vector<std::string> prefixes = { "", "../", "../../", "resources/", "../resources/" };
    bool ok = false;

    // Load xiangqi_oriental_sheet.png first
    for (const auto& p : prefixes) {
        if (masterImg.loadFromFile(p + "assets/xiangqi_oriental_sheet.png")) {
            ok = true;
            break;
        }
    }
    if (!ok) {
        for (const auto& p : prefixes) {
            if (masterImg.loadFromFile(p + "assets/xiangqi_master_spritesheet.png") ||
                masterImg.loadFromFile(p + "assets/xiangqi_ui_spritesheet.png")) {
                ok = true;
                break;
            }
        }
    }

    if (ok) {
        // Zero-white-box: mask pure white backgrounds if any
        masterImg.createMaskFromColor(sf::Color(255, 255, 255));
        texFantasy.loadFromImage(masterImg);
        texFantasy.setSmooth(false); // Sharp pixel art
        texMasterSpritesheet = texFantasy;
        texMedieval = texFantasy;
    }

    // Initialize Atlas Rects (Stage 1.1)
    rcPanelEbony   = sf::IntRect(0, 0, 192, 192);
    rcParchment    = sf::IntRect(200, 0, 160, 160);
    rcRibbonHeader = sf::IntRect(370, 0, 260, 54);
    rcBtnNormal    = sf::IntRect(0, 200, 220, 50);
    rcBtnHover     = sf::IntRect(0, 260, 220, 50);
    rcBtnPressed   = sf::IntRect(0, 320, 220, 50);
    rcIconUndo     = sf::IntRect(230, 200, 48, 48);
    rcIconRedo     = sf::IntRect(285, 200, 48, 48);
    rcIconDraw     = sf::IntRect(340, 200, 48, 48);
    rcIconResign   = sf::IntRect(395, 200, 48, 48);
    rcIncenseStick = sf::IntRect(0, 380, 320, 24);

    // Backward compatible rect aliases
    rcBtnNorm     = rcBtnNormal;
    rcBtnHov      = rcBtnHover;
    rcBtnSlimNorm = rcBtnNormal;
    rcBtnSlimHov  = rcBtnHover;
    rcSqNorm      = rcIconUndo;
    rcSqHov       = rcIconUndo;
    rcPanelOrnate = rcPanelEbony;
    rcSlotWood    = rcParchment;
    rcRibbon      = rcRibbonHeader;

    loaded = ok;
    return loaded;
}

void FantasyUI::draw9Slice(sf::RenderWindow& window,
                           const sf::Texture& texture,
                           const sf::IntRect& srcRect,
                           int inLeft, int inTop, int inRight, int inBottom,
                           const sf::FloatRect& dstRect,
                           const sf::Color& tint) {
    if (dstRect.width <= 0.0f || dstRect.height <= 0.0f) return;

    float minW = static_cast<float>(inLeft + inRight);
    float minH = static_cast<float>(inTop + inBottom);
    float scaleX = (dstRect.width < minW && minW > 0.0f) ? (dstRect.width / minW) : 1.0f;
    float scaleY = (dstRect.height < minH && minH > 0.0f) ? (dstRect.height / minH) : 1.0f;

    float actInL = static_cast<float>(inLeft) * scaleX;
    float actInR = static_cast<float>(inRight) * scaleX;
    float actInT = static_cast<float>(inTop) * scaleY;
    float actInB = static_cast<float>(inBottom) * scaleY;

    float x[4] = {
        dstRect.left,
        dstRect.left + actInL,
        dstRect.left + dstRect.width - actInR,
        dstRect.left + dstRect.width
    };

    float y[4] = {
        dstRect.top,
        dstRect.top + actInT,
        dstRect.top + dstRect.height - actInB,
        dstRect.top + dstRect.height
    };

    float u[4] = {
        static_cast<float>(srcRect.left),
        static_cast<float>(srcRect.left + inLeft),
        static_cast<float>(srcRect.left + srcRect.width - inRight),
        static_cast<float>(srcRect.left + srcRect.width)
    };

    float v[4] = {
        static_cast<float>(srcRect.top),
        static_cast<float>(srcRect.top + inTop),
        static_cast<float>(srcRect.top + srcRect.height - inBottom),
        static_cast<float>(srcRect.top + srcRect.height)
    };

    sf::VertexArray va(sf::Quads, 36);
    int vIdx = 0;

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            float x0 = x[col];
            float x1 = x[col + 1];
            float y0 = y[row];
            float y1 = y[row + 1];

            float u0 = u[col];
            float u1 = u[col + 1];
            float v0 = v[row];
            float v1 = v[row + 1];

            va[vIdx + 0] = sf::Vertex(sf::Vector2f(x0, y0), tint, sf::Vector2f(u0, v0));
            va[vIdx + 1] = sf::Vertex(sf::Vector2f(x1, y0), tint, sf::Vector2f(u1, v0));
            va[vIdx + 2] = sf::Vertex(sf::Vector2f(x1, y1), tint, sf::Vector2f(u1, v1));
            va[vIdx + 3] = sf::Vertex(sf::Vector2f(x0, y1), tint, sf::Vector2f(u0, v1));
            vIdx += 4;
        }
    }

    window.draw(va, &texture);
}

void FantasyUI::drawPanel(sf::RenderWindow& window,
                          const sf::FloatRect& dstRect,
                          OrientalPanelStyle style,
                          const sf::Color& tint) {
    if (dstRect.width <= 0.0f || dstRect.height <= 0.0f) return;

    if (loaded && texFantasy.getSize().x > 0) {
        switch (style) {
            case OrientalPanelStyle::EBONY_WOOD:
                draw9Slice(window, texFantasy, rcPanelEbony, 24, 24, 24, 24, dstRect, tint);
                return;
            case OrientalPanelStyle::PARCHMENT_SCROLL:
                draw9Slice(window, texFantasy, rcParchment, 20, 20, 20, 20, dstRect, tint);
                return;
            case OrientalPanelStyle::JADE_INSET: {
                sf::RectangleShape slotBg(sf::Vector2f(dstRect.width, dstRect.height));
                slotBg.setPosition(dstRect.left, dstRect.top);
                slotBg.setFillColor(sf::Color(20, 36, 30, 235));
                slotBg.setOutlineThickness(1.2f);
                slotBg.setOutlineColor(sf::Color(46, 230, 166, 180));
                window.draw(slotBg);
                return;
            }
        }
    }

    // Fallback procedural drawing
    switch (style) {
        case OrientalPanelStyle::EBONY_WOOD: {
            sf::RectangleShape panelBg(sf::Vector2f(dstRect.width, dstRect.height));
            panelBg.setPosition(dstRect.left, dstRect.top);
            panelBg.setFillColor(sf::Color(28, 20, 16, 245));
            panelBg.setOutlineThickness(1.8f);
            panelBg.setOutlineColor(sf::Color(201, 154, 62, 230));
            window.draw(panelBg);

            if (dstRect.width > 20.0f && dstRect.height > 20.0f) {
                sf::RectangleShape inner(sf::Vector2f(dstRect.width - 8.0f, dstRect.height - 8.0f));
                inner.setPosition(dstRect.left + 4.0f, dstRect.top + 4.0f);
                inner.setFillColor(sf::Color::Transparent);
                inner.setOutlineThickness(1.0f);
                inner.setOutlineColor(sf::Color(75, 50, 30, 160));
                window.draw(inner);
            }
            ve4GocLuaChon(window, sf::Vector2f(dstRect.left, dstRect.top),
                          sf::Vector2f(dstRect.width, dstRect.height), 2.0f, 1.6f);
            break;
        }
        case OrientalPanelStyle::PARCHMENT_SCROLL: {
            sf::RectangleShape cardBg(sf::Vector2f(dstRect.width, dstRect.height));
            cardBg.setPosition(dstRect.left, dstRect.top);
            cardBg.setFillColor(sf::Color(232, 216, 184, 240));
            cardBg.setOutlineThickness(1.2f);
            cardBg.setOutlineColor(sf::Color(165, 130, 85, 220));
            window.draw(cardBg);
            break;
        }
        case OrientalPanelStyle::JADE_INSET: {
            sf::RectangleShape slotBg(sf::Vector2f(dstRect.width, dstRect.height));
            slotBg.setPosition(dstRect.left, dstRect.top);
            slotBg.setFillColor(sf::Color(26, 61, 52, 235));
            slotBg.setOutlineThickness(1.2f);
            slotBg.setOutlineColor(sf::Color(46, 230, 166, 180));
            window.draw(slotBg);
            break;
        }
    }
}

void FantasyUI::drawButton(sf::RenderWindow& window,
                           const sf::FloatRect& dstRect,
                           const std::string& label,
                           const sf::Font& font,
                           unsigned int charSize,
                           bool isHovered,
                           bool isSelected,
                           bool isEnabled,
                           const sf::Texture* icon,
                           FantasyButtonStyle style) {
    (void)style;
    bool highlighted = (isHovered || isSelected) && isEnabled;

    if (loaded && texFantasy.getSize().x > 0) {
        sf::IntRect srcRect = highlighted ? rcBtnHover : rcBtnNormal;
        sf::Color tint = !isEnabled ? sf::Color(120, 120, 120, 200) : sf::Color::White;
        draw9Slice(window, texFantasy, srcRect, 18, 10, 18, 10, dstRect, tint);
    } else {
        sf::RectangleShape drawShape(sf::Vector2f(dstRect.width, dstRect.height));
        drawShape.setPosition(dstRect.left, dstRect.top);

        if (!isEnabled) {
            drawShape.setFillColor(sf::Color(35, 28, 24, 210));
            drawShape.setOutlineThickness(1.2f);
            drawShape.setOutlineColor(sf::Color(90, 75, 65, 180));
        } else if (highlighted) {
            drawShape.setFillColor(sf::Color(65, 42, 25, 255));
            drawShape.setOutlineThickness(2.0f);
            drawShape.setOutlineColor(sf::Color(235, 185, 75, 255));
        } else {
            drawShape.setFillColor(sf::Color(42, 28, 20, 245));
            drawShape.setOutlineThickness(1.5f);
            drawShape.setOutlineColor(sf::Color(160, 115, 65, 220));
        }
        window.draw(drawShape);
    }

    if (highlighted) {
        ve4GocLuaChon(window, sf::Vector2f(dstRect.left, dstRect.top),
                      sf::Vector2f(dstRect.width, dstRect.height), 2.0f, 1.8f);
    }

    float textOffsetLeft = 0.0f;
    if (icon && icon->getSize().x > 0 && isEnabled) {
        sf::Sprite spIcon(*icon);
        sf::Vector2u isz = icon->getSize();
        float targetH = dstRect.height * 0.52f;
        float sc = targetH / static_cast<float>(isz.y);
        spIcon.setScale(sc, sc);
        float iconW = static_cast<float>(isz.x) * sc;
        float iconH = static_cast<float>(isz.y) * sc;

        spIcon.setPosition(dstRect.left + 12.0f, dstRect.top + (dstRect.height - iconH) / 2.0f);
        if (highlighted) {
            spIcon.setColor(sf::Color(255, 235, 160));
        } else {
            spIcon.setColor(sf::Color(210, 180, 140));
        }
        window.draw(spIcon);
        textOffsetLeft = iconW + 8.0f;
    }

    if (!label.empty()) {
        sf::Text txt;
        txt.setFont(font);
        txt.setString(sf::String::fromUtf8(label.begin(), label.end()));
        txt.setCharacterSize(charSize);
        txt.setStyle(sf::Text::Bold);

        sf::FloatRect tb = txt.getLocalBounds();
        float tx = dstRect.left + textOffsetLeft + (dstRect.width - textOffsetLeft) / 2.0f;
        float ty = dstRect.top + dstRect.height / 2.0f;

        txt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        txt.setPosition(std::round(tx), std::round(ty));

        // Soft carved shadow
        sf::Text shadow = txt;
        shadow.setPosition(std::round(tx + 1.2f), std::round(ty + 1.2f));
        shadow.setFillColor(sf::Color(12, 6, 3, 230));
        window.draw(shadow);

        if (!isEnabled) {
            txt.setFillColor(sf::Color(140, 130, 120));
        } else if (highlighted) {
            txt.setFillColor(sf::Color(255, 245, 140));
        } else {
            txt.setFillColor(sf::Color(245, 230, 205));
        }
        window.draw(txt);
    }
}

void FantasyUI::drawIncenseTimer(sf::RenderWindow& window,
                                const sf::FloatRect& rect,
                                float remainingSeconds,
                                float totalSeconds) {
    if (rect.width <= 0.0f || rect.height <= 0.0f) return;
    if (totalSeconds <= 0.0f) totalSeconds = 1.0f;
    float ratio = std::clamp(remainingSeconds / totalSeconds, 0.0f, 1.0f);

    // 1. Đế cắm nhang / Rãnh đồng đỡ tàn nhang
    sf::RectangleShape tray(sf::Vector2f(rect.width, 6.0f));
    tray.setPosition(rect.left, rect.top + rect.height / 2.0f - 3.0f);
    tray.setFillColor(sf::Color(45, 30, 20));
    tray.setOutlineThickness(1.0f);
    tray.setOutlineColor(sf::Color(140, 100, 50));
    window.draw(tray);

    // 2. Thân que nhang đang cháy dần (ngắn lại theo tỉ lệ ratio)
    float currentLength = rect.width * ratio;
    if (currentLength > 0.0f) {
        sf::RectangleShape incense(sf::Vector2f(currentLength, 4.0f));
        incense.setPosition(rect.left, rect.top + rect.height / 2.0f - 2.0f);
        incense.setFillColor(sf::Color(180, 80, 50)); // Màu bột nhang truyền thống
        window.draw(incense);

        // 3. Đốm than đỏ rực ở đầu ngọn nhang
        static float emberTick = 0.0f;
        emberTick += 0.08f;
        float pulse = 0.85f + 0.15f * std::sin(emberTick);
        sf::CircleShape ember(3.5f * pulse);
        ember.setOrigin(3.5f * pulse, 3.5f * pulse);
        ember.setPosition(rect.left + currentLength, rect.top + rect.height / 2.0f);
        ember.setFillColor(sf::Color(255, 80, 20));
        window.draw(ember);

        // Khói mờ bốc lên
        for (int p = 0; p < 3; ++p) {
            float age = std::fmod(emberTick * 8.0f + p * 15.0f, 30.0f);
            float smokeY = (rect.top + rect.height / 2.0f) - age * 0.45f;
            float smokeX = (rect.left + currentLength) + std::sin(age * 0.18f + p * 1.5f) * (age * 0.2f);
            float alpha = std::clamp(100.0f * (1.0f - age / 30.0f), 0.0f, 100.0f);
            sf::CircleShape puff(1.2f + age * 0.05f);
            puff.setOrigin(puff.getRadius(), puff.getRadius());
            puff.setPosition(smokeX, smokeY);
            puff.setFillColor(sf::Color(200, 200, 200, static_cast<sf::Uint8>(alpha)));
            window.draw(puff);
        }
    }
}

void FantasyUI::drawRibbonTitle(sf::RenderWindow& window,
                                float centerX, float topY, float width, float height,
                                const std::string& title,
                                const sf::Font& font,
                                unsigned int charSize,
                                const sf::Color& textColor,
                                const sf::Color& tint) {
    float left = centerX - width / 2.0f;
    sf::FloatRect dstRect(left, topY, width, height);

    if (loaded && texFantasy.getSize().x > 0 && rcRibbonHeader.width > 0) {
        draw9Slice(window, texFantasy, rcRibbonHeader, 30, 10, 30, 10, dstRect, tint);
    } else {
        sf::RectangleShape ribbon(sf::Vector2f(width, height));
        ribbon.setPosition(left, topY);
        ribbon.setFillColor(sf::Color(42, 28, 18, 240));
        ribbon.setOutlineThickness(1.5f);
        ribbon.setOutlineColor(tint);
        window.draw(ribbon);
        ve4GocLuaChon(window, sf::Vector2f(left, topY), sf::Vector2f(width, height), 1.0f, 1.4f);
    }

    if (!title.empty()) {
        sf::Text txt;
        txt.setFont(font);
        txt.setString(sf::String::fromUtf8(title.begin(), title.end()));
        txt.setCharacterSize(charSize);
        txt.setStyle(sf::Text::Bold);

        sf::FloatRect tb = txt.getLocalBounds();
        txt.setOrigin(std::round(tb.left + tb.width / 2.0f), std::round(tb.top + tb.height / 2.0f));
        txt.setPosition(std::round(centerX), std::round(topY + height / 2.0f - 1.0f));

        sf::Text shadow = txt;
        shadow.setPosition(std::round(centerX + 1.2f), std::round(topY + height / 2.0f + 0.5f));
        shadow.setFillColor(sf::Color(12, 6, 3, 230));
        window.draw(shadow);

        txt.setFillColor(textColor);
        window.draw(txt);
    }
}
