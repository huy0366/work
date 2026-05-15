#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdint>

using namespace sf;
using namespace std;

// --- HÀM TÍNH TOÁN MÀU SẮC ---
Color Mau(float x, float y, float radius)
{
    float angle = atan2(y + x * 0.5f, x);
    float hue = (angle + 3.14159f) / (2 * 3.14159f);
    float r = abs(hue * 6.0f - 3.0f) - 1.0f;
    float g = 2.0f - abs(hue * 6.0f - 2.0f);
    float b = 2.0f - abs(hue * 6.0f - 4.0f);
    auto clamp = [](float v)
    { return (uint8_t)(max(0.0f, min(1.0f, v)) * 255); };
    return Color(clamp(r), clamp(g), clamp(b));
}

// --- HÀM VẼ HÌNH LỤC GIÁC NHỎ ---
void ve_luc_giac_nho(RenderWindow &Window, float posx, float posy, float size, Color color)
{
    ConvexShape tmp(6);
    for (int i = 0; i < 6; i++)
    {
        float angle = 60.f * i;
        float angle_radian = angle * 3.14159f / 180.f;
        tmp.setPoint(i, Vector2f(size * cos(angle_radian), size * sin(angle_radian)));
    }
    tmp.setPosition({posx, posy});
    tmp.setFillColor(color);
    tmp.setOutlineThickness(-1.0f);
    tmp.setOutlineColor(Color(255, 255, 255, 150));
    Window.draw(tmp);
}

// --- HÀM VẼ LƯỚI LỤC GIÁC ---
Color ve_luc_giac_to(RenderWindow &Window, Vector2f center, float size, int radius, bool isClicked)
{
    Vector2i pixelPos = Mouse::getPosition(Window);
    Vector2f mousePosF = Window.mapPixelToCoords(pixelPos);
    float width = size * 1.5f;
    float height = sqrt(3.0f) * size;
    Color selectedColor = Color(0, 0, 0, 0);

    struct Hex
    {
        Vector2f p;
        float s;
        Color c;
    };
    vector<Hex> normal, hovered;

    for (int x = -radius; x <= radius; x++)
    {
        int y_start = (x < 0) ? -radius - x : -radius;
        int y_end = (x > 0) ? radius - x : radius;
        for (int y = y_start; y <= y_end; y++)
        {
            float posx = center.x + x * width;
            float posy = center.y + (y + x * 0.5f) * height;
            float dx = mousePosF.x - posx;
            float dy = mousePosF.y - posy;
            float distance = sqrt(dx * dx + dy * dy);
            Color color = Mau((float)x, (float)y, (float)radius);

            // Kiểm tra chuột có nằm trong lục giác không (khoảng cách gần đúng)
            if (distance < size * 0.9f)
            {
                hovered.push_back({{posx, posy}, size * 1.3f, Color(color.r, color.g, color.b, 255)});
                if (isClicked)
                    selectedColor = color;
            }
            else
            {
                normal.push_back({{posx, posy}, size, Color(color.r, color.g, color.b, 150)});
            }
        }
    }
    // Vẽ các lục giác bình thường trước, sau đó vẽ các lục giác đang được hover lên trên
    for (auto &h : normal)
        ve_luc_giac_nho(Window, h.p.x, h.p.y, h.s, h.c);
    for (auto &h : hovered)
        ve_luc_giac_nho(Window, h.p.x, h.p.y, h.s, h.c);
        
    return selectedColor;
}

// --- HÀM MAIN ---
int main()
{
    // Tạo cửa sổ kích thước 800x600, bật chế độ khử răng cưa (Antialiasing) cho đẹp
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    RenderWindow window(VideoMode(800, 600), "Hexagon Color Picker - SFML", Style::Default, settings);
    window.setFramerateLimit(60);

    // Biến lưu trữ trạng thái màu sắc hiện tại đã chọn
    Color currentColor = Color(50, 50, 50); // Màu mặc định

    // Vòng lặp chính của ứng dụng
    while (window.isOpen())
    {
        Event event;
        bool isClicked = false;

        // Xử lý sự kiện (đóng cửa sổ, click chuột)
        while (window.pollEvent(event))
        {
            if (event.kind == Event::Closed)
                window.close();
                
            // Bắt sự kiện click chuột trái
            if (event.kind == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                isClicked = true;
            }
        }

        // Xóa màn hình với màu nền tối
        window.clear(Color(25, 25, 30));

        // Tọa độ tâm lưới lục giác, kích thước 1 ô, và bán kính (số vòng)
        Vector2f center(400.f, 300.f);
        float hexSize = 20.f;
        int gridRadius = 8;

        // Gọi hàm vẽ và lấy màu được click
        Color pickedColor = ve_luc_giac_to(window, center, hexSize, gridRadius, isClicked);

        // Nếu có màu được chọn (khác màu trong suốt), cập nhật màu hiện tại
        if (isClicked && pickedColor.a != 0)
        {
            currentColor = pickedColor;
        }

        // Vẽ một hình chữ nhật nhỏ ở góc để hiển thị màu đang được chọn
        RectangleShape colorDisplay(Vector2f(80.f, 80.f));
        colorDisplay.setPosition(30.f, 30.f);
        colorDisplay.setFillColor(currentColor);
        colorDisplay.setOutlineThickness(3.f);
        colorDisplay.setOutlineColor(Color::White);
        window.draw(colorDisplay);

        // Hiển thị nội dung ra cửa sổ
        window.display();
    }

    return 0;
}