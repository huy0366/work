#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>

// --- Cấu trúc dữ liệu cho một ô lục giác ---
struct HexCell {
    sf::ConvexShape shape;  // Dùng ConvexShape cho lục giác chuẩn
    sf::Color       color;
    sf::Vector2f    center;
    float           scaleCur = 1.0f; // Dùng cho hiệu ứng hover
    float           scaleTgt = 1.0f;
    bool            selected = false;
};

// --- Hàm hỗ trợ chuyển HSV -> RGB ---
// h: 0-360, s,v: 0-1
sf::Color hsvToRgb(float h, float s, float v) {
    if (s <= 0.0f) {
        uint8_t c = static_cast<uint8_t>(v * 255.0f);
        return sf::Color(c, c, c);
    }
    float h_prime = std::fmod(h, 360.0f) / 60.0f;
    int   i = static_cast<int>(h_prime);
    float f = h_prime - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));
    float r = 0, g = 0, b = 0;
    switch (i) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
    }
    return sf::Color(
        static_cast<uint8_t>(r * 255.0f),
        static_cast<uint8_t>(g * 255.0f),
        static_cast<uint8_t>(b * 255.0f)
    );
}

int main() {
    // Tăng kích thước cửa sổ để chứa bảng màu lớn
    sf::RenderWindow window(sf::VideoMode({1000u, 900u}), "Mo phong bang mau luc giac lon");
    window.setFramerateLimit(60);

    // --- CẤU HÌNH BẢNG MÀU ---
    const int   numLayers = 10;          // Số lớp từ tâm ra ngoài (tăng lên để có nhiều ô)
    const float hexRadius = 25.0f;       // Bán kính mỗi ô lục giác (giảm đi để chứa được nhiều ô)
    const float hexPadding = 2.0f;      // Khoảng hở nhỏ giữa các ô (tránh rối mắt)
    const float effectiveRadius = hexRadius - hexPadding;

    const sf::Vector2f center(500.0f, 400.0f); // Tâm của bảng màu

    // --- Tự động tính toán số ô mỗi hàng cho hình lục giác đối xứng ---
    std::vector<int> rows;
    int maxCols = 2 * numLayers - 1;
    for (int i = 0; i < maxCols; ++i) {
        int distFromCenter = std::abs(i - (numLayers - 1));
        rows.push_back(maxCols - distFromCenter);
    }
    // Tổng số ô = 3 * numLayers * (numLayers - 1) + 1. 
    // Với 10 lớp, tổng cộng là 271 ô. Rất gần con số 255 bạn muốn!

    // Khoảng cách ngang/dọc giữa các tâm
    float dx = std::sqrt(3.0f) * hexRadius;
    float dy = 1.5f * hexRadius;

    // --- TẠO CÁC Ô LỤC GIÁC ---
    std::vector<HexCell> cells;
    for (int i = 0; i < (int)rows.size(); ++i) {
        int n = rows[i];
        for (int j = 0; j < n; ++j) {
            // Tính toán vị trí tương đối
            float rx = (j - (n - 1) / 2.0f) * dx;
            float ry = (i - (numLayers - 1)) * dy;

            // Tính màu sắc dựa trên HSV
            float dist = std::hypot(rx, ry);
            float maxDist = (numLayers - 1) * dx; // Chuẩn hóa khoảng cách
            float s = (maxDist > 0.0f) ? std::min(1.0f, dist / maxDist) : 0.0f;

            float angle = std::atan2(ry, rx) * (180.0f / 3.14159265f);
            if (angle < 0.0f) angle += 360.0f;

            HexCell cell;
            cell.center = center + sf::Vector2f(rx, ry);
            cell.color = hsvToRgb(angle, s, 1.0f); // Giữ Value = 1.0f cho màu tươi

            // Tạo ConvexShape lục giác chuẩn (flat-top)
            cell.shape.setPointCount(6);
            for (int k = 0; k < 6; ++k) {
                // Xoay 30 độ để có cạnh nằm ngang (flat-top)
                float a = (30.0f + 60.0f * k) * (3.14159265f / 180.0f);
                cell.shape.setPoint(k, { effectiveRadius * std::cos(a), effectiveRadius * std::sin(a) });
            }
            cell.shape.setFillColor(cell.color);
            cell.shape.setPosition(cell.center);
            cell.shape.setOutlineThickness(0.0f);

            cells.push_back(cell);
        }
    }

    // --- Font và Text hiển thị thông tin màu ---
    sf::Font font;
    // ĐẢM BẢO FILE FONT NÀY TỒN TẠI!
    if (!font.openFromFile("arial.ttf")) {
        std::cerr << "Loi load font arial.ttf! Hay chac chan file font nam cung thu muc voi file .cpp." << std::endl;
        // Trả về lỗi hoặc xử lý phù hợp nếu cần. 
        // Trong trường hợp demo, chúng ta tạm chấp nhận rủi ro không có font.
    }

    sf::Text infoText(font, "Di chuot va click de chon mau", 18);
    infoText.setFillColor(sf::Color::White);

    // Căn giữa text ở phía dưới
    auto centerText = [&]() {
        sf::FloatRect b = infoText.getLocalBounds();
        infoText.setOrigin({b.position.x + b.size.x / 2.0f, 0.0f});
        // Đặt text ở phía dưới bảng màu
        infoText.setPosition({center.x, 820.0f});
    };
    centerText();

    sf::Clock clock;

    // --- VÒNG LẶP CHÍNH ---
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window));

        // --- Xử lý sự kiện ---
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    bool hit = false;
                    for (auto& cell : cells) {
                        // Kiểm tra va chạm: chuột có nằm trong ô này không?
                        // Sử dụng ConvexShape::getGlobalBounds để kiểm tra va chạm
                        if (cell.shape.getGlobalBounds().contains(mouse)) {
                            // Hủy chọn tất cả các ô khác
                            for (auto& c : cells) c.selected = false;
                            cell.selected = true;
                            hit = true;
                            break;
                        }
                    }
                    if (!hit) {
                        // Nếu click ra ngoài bảng màu, hủy chọn tất cả
                        for (auto& c : cells) c.selected = false;
                    }
                }
            }
        }

        // --- Cập nhật logic (Animation + Text) ---
        bool anyHovered = false;
        bool anySelected = false;
        std::string displayCode = "Di chuot va click de chon mau";

        for (auto& cell : cells) {
            bool hovered = cell.shape.getGlobalBounds().contains(mouse);

            // Hiệu ứng Hover: phóng to nhẹ (Animation mượt mà)
            cell.scaleTgt = hovered ? 1.2f : 1.0f;
            cell.scaleCur += (cell.scaleTgt - cell.scaleCur) * 15.0f * dt;
            cell.shape.setScale({cell.scaleCur, cell.scaleCur});

            if (hovered) anyHovered = true;

            // Hiệu ứng khi được Chọn (Selected): Viền trắng rõ rệt
            if (cell.selected) {
                cell.shape.setOutlineColor(sf::Color::White);
                cell.shape.setOutlineThickness(-3.0f); // Viền vẽ vào trong
                anySelected = true;

                // Tạo mã màu HEX của ô được chọn
                std::ostringstream ss;
                ss << "#" << std::hex << std::uppercase << std::setfill('0')
                   << std::setw(2) << (int)cell.color.r
                   << std::setw(2) << (int)cell.color.g
                   << std::setw(2) << (int)cell.color.b
                   << " | RGB(" << (int)cell.color.r << "," << (int)cell.color.g << "," << (int)cell.color.b << ")";
                displayCode = ss.str();
            } else {
                // Nếu không được chọn, hủy viền
                cell.shape.setOutlineThickness(0.0f);
            }
        }

        // Cập nhật text hiển thị
        if (anySelected) {
            infoText.setString(displayCode);
        } else if (anyHovered) {
            // Khi hover, chỉ hiển thị mã màu nếu chưa có ô nào được chọn
            infoText.setString(displayCode);
        } else {
            infoText.setString("Di chuot va click de chon mau");
        }
        centerText(); // Phải gọi lại để cập nhật lại vị trí text khi text thay đổi độ dài

        // --- Vẽ ---
        window.clear(sf::Color(20, 20, 20)); // Màu nền tối để bảng màu nổi bật
        for (auto& cell : cells)
            window.draw(cell.shape);
        window.draw(infoText);
        window.display();
    }

    return 0;
}