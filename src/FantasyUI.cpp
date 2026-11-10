#include "FantasyUI.h"
#include "Menu.h"
#include <algorithm>
#include <cmath>

static bool loadTexSafe(sf::Texture& tex, const std::string& path, bool smooth = false) {
    const std::vector<std::string> prefixes = { "", "../", "../../" };
    for (const auto& p : prefixes) {
        sf::Image img;
        if (img.loadFromFile(p + path)) {
            img.createMaskFromColor(sf::Color::White);
            sf::Vector2u sz = img.getSize();
            for (unsigned int y = 0; y < sz.y; ++y) {
                for (unsigned int x = 0; x < sz.x; ++x) {
                    sf::Color c = img.getPixel(x, y);
                    if (c.r >= 250 && c.g >= 250 && c.b >= 250) {
                        img.setPixel(x, y, sf::Color(c.r, c.g, c.b, 0));
                    }
                }
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
    const std::vector<std::string> prefixes = { "", "../", "../../" };
    bool ok = false;
    for (const auto& p : prefixes) {
        sf::Image img;
        if (img.loadFromFile(p + "assets/xiangqi_master_spritesheet.png") ||
            img.loadFromFile(p + "assets/xiangqi_ui_spritesheet.png")) {
            img.createMaskFromColor(sf::Color::White);
            sf::Vector2u sz = img.getSize();
            for (unsigned int y = 0; y < sz.y; ++y) {
                for (unsigned int x = 0; x < sz.x; ++x) {
                    sf::Color c = img.getPixel(x, y);
                    if (c.r >= 250 && c.g >= 250 && c.b >= 250) {
                        img.setPixel(x, y, sf::Color(c.r, c.g, c.b, 0));
                    }
                }
            }
            texMasterSpritesheet.loadFromImage(img);
            texMasterSpritesheet.setSmooth(false);
            ok = true;
            break;
        }
    }

    if (!ok) {
        const std::string dir = "assets/Fantasy/MedievalFantasyPaid/";
        for (const auto& p : prefixes) {
            sf::Image img;
            if (img.loadFromFile(p + dir + "medieval.png") ||
                img.loadFromFile(p + "resources/Fantasy/medieval.png")) {
                img.createMaskFromColor(sf::Color::White);
                texMasterSpritesheet.loadFromImage(img);
                texMasterSpritesheet.setSmooth(true);
                ok = true;
                break;
            }
        }
    }

    texMedieval = texMasterSpritesheet;
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
                          FantasyPanelStyle style,
                          const sf::Color& tint) {
    switch (style) {
        case FantasyPanelStyle::ORNATE_WOOD: {
            // Dark rosewood panel with brass corner cloud brackets (Player card, Pause modal)
            sf::RectangleShape panelBg(sf::Vector2f(dstRect.width, dstRect.height));
            panelBg.setPosition(dstRect.left, dstRect.top);
            panelBg.setFillColor(sf::Color(28, 20, 16, 245));
            panelBg.setOutlineThickness(1.8f);
            panelBg.setOutlineColor(sf::Color(145, 105, 55, 230));
            window.draw(panelBg);

            // Subtle inner inset border
            if (dstRect.width > 20.0f && dstRect.height > 20.0f) {
                sf::RectangleShape inner(sf::Vector2f(dstRect.width - 8.0f, dstRect.height - 8.0f));
                inner.setPosition(dstRect.left + 4.0f, dstRect.top + 4.0f);
                inner.setFillColor(sf::Color::Transparent);
                inner.setOutlineThickness(1.0f);
                inner.setOutlineColor(sf::Color(75, 50, 30, 160));
                window.draw(inner);
            }

            // 4 Brass cloud brackets
            ve4GocLuaChon(window, sf::Vector2f(dstRect.left, dstRect.top),
                          sf::Vector2f(dstRect.width, dstRect.height), 2.0f, 1.6f);
            break;
        }
        case FantasyPanelStyle::WOOD_SLOT: {
            // Recessed flat dark wood tray (clean, zero dragon artifacts)
            sf::RectangleShape slotBg(sf::Vector2f(dstRect.width, dstRect.height));
            slotBg.setPosition(dstRect.left, dstRect.top);
            slotBg.setFillColor(sf::Color(22, 16, 13, 235));
            slotBg.setOutlineThickness(1.2f);
            slotBg.setOutlineColor(sf::Color(105, 75, 45, 200));
            window.draw(slotBg);
            break;
        }
        case FantasyPanelStyle::SQUARE_FRAME: {
            // Frame for avatar badges
            sf::RectangleShape frameBg(sf::Vector2f(dstRect.width, dstRect.height));
            frameBg.setPosition(dstRect.left, dstRect.top);
            frameBg.setFillColor(sf::Color(26, 20, 16, 240));
            frameBg.setOutlineThickness(2.0f);
            frameBg.setOutlineColor(tint);
            window.draw(frameBg);
            break;
        }
        case FantasyPanelStyle::PARCHMENT_CARD: {
            // Warm aged parchment card
            sf::RectangleShape cardBg(sf::Vector2f(dstRect.width, dstRect.height));
            cardBg.setPosition(dstRect.left, dstRect.top);
            cardBg.setFillColor(sf::Color(245, 232, 212, 240));
            cardBg.setOutlineThickness(1.2f);
            cardBg.setOutlineColor(sf::Color(165, 130, 85, 220));
            window.draw(cardBg);
            break;
        }
        case FantasyPanelStyle::PARCHMENT_RIBBON: {
            // Parchment scroll ribbon
            sf::RectangleShape ribBg(sf::Vector2f(dstRect.width, dstRect.height));
            ribBg.setPosition(dstRect.left, dstRect.top);
            ribBg.setFillColor(sf::Color(44, 28, 18, 240));
            ribBg.setOutlineThickness(1.5f);
            ribBg.setOutlineColor(tint);
            window.draw(ribBg);
            ve4GocLuaChon(window, sf::Vector2f(dstRect.left, dstRect.top),
                          sf::Vector2f(dstRect.width, dstRect.height), 1.0f, 1.4f);
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

    // 1. Draw rich wood plaque base with warm bevel outline (Never stretch vertical sprites)
    sf::RectangleShape drawShape(sf::Vector2f(dstRect.width, dstRect.height));
    drawShape.setPosition(dstRect.left, dstRect.top);

    if (!isEnabled) {
        drawShape.setFillColor(sf::Color(35, 28, 24, 210));
        drawShape.setOutlineThickness(1.2f);
        drawShape.setOutlineColor(sf::Color(90, 75, 65, 180));
    } else if (highlighted) {
        // Glowing warm gold finish on dark wood
        drawShape.setFillColor(sf::Color(65, 42, 25, 255));
        drawShape.setOutlineThickness(2.0f);
        drawShape.setOutlineColor(sf::Color(235, 185, 75, 255));
    } else {
        // Deep carved dark rosewood
        drawShape.setFillColor(sf::Color(42, 28, 20, 245));
        drawShape.setOutlineThickness(1.5f);
        drawShape.setOutlineColor(sf::Color(160, 115, 65, 220));
    }
    window.draw(drawShape);

    // 2. Corner brackets on highlight
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

        txt.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
        txt.setPosition(tx, ty);

        // Soft carved shadow
        sf::Text shadow = txt;
        shadow.setPosition(tx + 1.2f, ty + 1.2f);
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

void FantasyUI::drawDragonTimer(sf::RenderWindow& window,
                                const sf::FloatRect& dstRect,
                                float remainingSeconds,
                                float totalSeconds,
                                const sf::Font& font,
                                bool isLowTime) {
    // 1. Recessed wood slot tray
    drawPanel(window, dstRect, FantasyPanelStyle::WOOD_SLOT);

    // 2. Burning Incense Stick layout
    float clockW = 75.0f;
    float stickMarginL = 12.0f;
    float incenseW = std::max(20.0f, dstRect.width - clockW - stickMarginL - 10.0f);
    float incenseH = 4.0f;
    float ix = dstRect.left + stickMarginL;
    float iy = dstRect.top + (dstRect.height - incenseH) / 2.0f;

    float r = (totalSeconds > 0.0f) ? std::clamp(remainingSeconds / totalSeconds, 0.0f, 1.0f) : 1.0f;

    // Ceramic / bronze burner trough
    sf::RectangleShape burnerTray(sf::Vector2f(incenseW + 8.0f, 6.0f));
    burnerTray.setPosition(ix - 4.0f, iy - 1.0f);
    burnerTray.setFillColor(sf::Color(24, 18, 14, 210));
    burnerTray.setOutlineThickness(1.0f);
    burnerTray.setOutlineColor(sf::Color(120, 85, 45));
    window.draw(burnerTray);

    // Ash remaining on left (burnt stick portion)
    float ashW = incenseW * (1.0f - r);
    sf::RectangleShape ashStick(sf::Vector2f(ashW, incenseH));
    ashStick.setPosition(ix, iy);
    ashStick.setFillColor(sf::Color(90, 85, 80, 180));
    window.draw(ashStick);

    // Unburnt fragrant sandalwood incense stick on right
    float burnPointX = ix + ashW;
    float unburntW = incenseW * r;
    sf::RectangleShape activeStick(sf::Vector2f(unburntW, incenseH));
    activeStick.setPosition(burnPointX, iy);
    activeStick.setFillColor(sf::Color(175, 75, 50));
    window.draw(activeStick);

    // Glowing burning ember at burnPointX
    static float emberFlicker = 0.0f;
    emberFlicker += 0.08f;
    float pulse = 0.8f + 0.2f * std::sin(emberFlicker);
    sf::CircleShape ember(3.0f * pulse);
    ember.setOrigin(3.0f * pulse, 3.0f * pulse);
    ember.setPosition(burnPointX, iy + incenseH / 2.0f);
    ember.setFillColor(isLowTime ? sf::Color(255, 60, 40)
                                : sf::Color(255, 160 + static_cast<int>(50.0f * pulse), 40));
    window.draw(ember);

    // Faint curling smoke wisps rising from the ember
    for (int p = 0; p < 3; ++p) {
        float smokeAge = std::fmod(emberFlicker * 10.0f + p * 20.0f, 50.0f);
        float smokeY = (iy + incenseH / 2.0f) - smokeAge * 0.4f;
        float smokeX = burnPointX + std::sin(smokeAge * 0.15f + p) * (smokeAge * 0.15f);
        float smokeAlpha = std::clamp(255.0f * (1.0f - smokeAge / 50.0f) * 0.5f, 0.0f, 255.0f);
        sf::CircleShape smokePuff(1.2f + smokeAge * 0.04f);
        smokePuff.setOrigin(smokePuff.getRadius(), smokePuff.getRadius());
        smokePuff.setPosition(smokeX, smokeY);
        smokePuff.setFillColor(sf::Color(220, 220, 225, static_cast<sf::Uint8>(smokeAlpha)));
        window.draw(smokePuff);
    }

    // 3. Digital clock on the right
    auto formatTime = [](float t) {
        if (t <= 0.0f) return std::string("00:00");
        int m = static_cast<int>(t / 60.0f);
        int s = static_cast<int>(t) % 60;
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
        return std::string(buf);
    };

    std::string timeStr = formatTime(remainingSeconds);
    sf::Text tVal;
    tVal.setFont(font);
    tVal.setCharacterSize(18);
    tVal.setStyle(sf::Text::Bold);
    tVal.setString(timeStr);

    sf::FloatRect tvb = tVal.getLocalBounds();
    float tx = dstRect.left + dstRect.width - 12.0f;
    float ty = dstRect.top + dstRect.height / 2.0f;
    tVal.setOrigin(tvb.left + tvb.width, tvb.top + tvb.height / 2.0f);
    tVal.setPosition(tx, ty);

    // Carved shadow
    sf::Text shadow = tVal;
    shadow.setPosition(tx + 1.2f, ty + 1.2f);
    shadow.setFillColor(sf::Color(10, 6, 4, 240));
    window.draw(shadow);

    if (isLowTime) {
        tVal.setFillColor(sf::Color(255, 80, 70));
    } else {
        tVal.setFillColor(sf::Color(255, 225, 110));
    }
    window.draw(tVal);
}

void FantasyUI::drawRibbonTitle(sf::RenderWindow& window,
                                float centerX, float topY, float width, float height,
                                const std::string& title,
                                const sf::Font& font,
                                unsigned int charSize,
                                const sf::Color& textColor,
                                const sf::Color& tint) {
    float left = centerX - width / 2.0f;
    sf::RectangleShape ribbon(sf::Vector2f(width, height));
    ribbon.setPosition(left, topY);
    ribbon.setFillColor(sf::Color(42, 28, 18, 240));
    ribbon.setOutlineThickness(1.5f);
    ribbon.setOutlineColor(tint);
    window.draw(ribbon);

    ve4GocLuaChon(window, sf::Vector2f(left, topY), sf::Vector2f(width, height), 1.0f, 1.4f);

    sf::Text txt;
    txt.setFont(font);
    txt.setString(sf::String::fromUtf8(title.begin(), title.end()));
    txt.setCharacterSize(charSize);
    txt.setStyle(sf::Text::Bold);

    sf::FloatRect tb = txt.getLocalBounds();
    txt.setOrigin(tb.left + tb.width / 2.0f, tb.top + tb.height / 2.0f);
    txt.setPosition(centerX, topY + height / 2.0f - 1.0f);

    // Shadow
    sf::Text shadow = txt;
    shadow.setPosition(centerX + 1.2f, topY + height / 2.0f + 0.5f);
    shadow.setFillColor(sf::Color(12, 6, 3, 230));
    window.draw(shadow);

    txt.setFillColor(textColor);
    window.draw(txt);
}
