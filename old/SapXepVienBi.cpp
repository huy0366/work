#include <iostream>
#include <vector>
#include <cstdlib> 
#include <ctime>   
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>

using namespace std;
using namespace sf;

// 1. TẠO CLASS HÌNH BO GÓC


class RoundedRectangleShape : public sf::Shape {
public:
    RoundedRectangleShape(sf::Vector2f size, float radius, unsigned int cornerPoints = 10) 
        : mySize(size), myRadius(radius), myCornerPoints(cornerPoints) // Số điểm để tạo góc bo tròn
    {
        update(); // Cập nhật hình dạng sau khi khởi tạo
    }

    virtual std::size_t getPointCount() const override { return myCornerPoints * 4; } // Tổng số điểm là số điểm mỗi góc nhân với 4 góc

    virtual sf::Vector2f getPoint(std::size_t index) const override {
        static const float PI = 3.141592654f; 
        
        // Xác định góc nào (0: Dưới-Phải, 1: Dưới-Trái, 2: Trên-Trái, 3: Trên-Phải)
        unsigned int centerIndex = index / myCornerPoints; // Chia để xác định góc
        unsigned int pointIndex  = index % myCornerPoints; // Lấy phần dư để xác định điểm trong góc đó

        // Tính góc dựa trên index (đổi sang Radian luôn cho nhanh)
        float angle = (centerIndex * 90.f + pointIndex * (90.f / (myCornerPoints - 1))) * PI / 180.f;  // Góc bắt đầu từ 0 độ ở góc dưới phải và tăng ngược chiều kim đồng hồ

        // Xác định tâm của góc bo tròn bằng cách dịch chuyển từ 4 đỉnh
        sf::Vector2f center;
        center.x = (centerIndex == 0 || centerIndex == 3) ? mySize.x - myRadius : myRadius; // Góc dưới phải và trên phải có tâm cách mép phải, góc dưới trái và trên trái có tâm cách mép trái
        center.y = (centerIndex == 0 || centerIndex == 1) ? mySize.y - myRadius : myRadius; // Góc dưới phải và dưới trái có tâm cách mép dưới, góc trên trái và trên phải có tâm cách mép trên

        return { center.x + myRadius * std::cos(angle), center.y + myRadius * std::sin(angle) }; // Tính toán điểm trên đường tròn dựa trên tâm và góc
    }

private:
    sf::Vector2f mySize;
    float myRadius;
    unsigned int myCornerPoints;
};

struct Colordata
{
    string name;
    Color color;
};

int main() {

    RenderWindow window(VideoMode({1920, 1080}), "Sắp xếp viên bi", Style::Default, State::Windowed);
    window.setFramerateLimit(60);

    // Khoi tao mau;
    vector<Colordata> Mau = {{"Red", Color::Red},
                             {"Orange", Color(255, 165, 0)}, // Cam
                             {"Green", Color::Green},
                             {"Yellow", Color::Yellow},
                             {"Blue", Color::Blue},
                             {"Indigo", Color(75, 0, 130)}, // Chàm
                             {"Violet", Color(238, 130, 238)}};

    Font font;
    if (!font.openFromFile("C:\\Windows\\Fonts\\Arial.ttf")) {
        cerr << "Không thể tải font!" << endl;
        return -1;
    }

    Font VNfont;
    if (!VNfont.openFromFile("c:\\WINDOWS\\Fonts\\TIMES.TTF")) {
        cerr << "Không thể tải font!" << endl;
        return -1;
    }

    Text text(font);
    text.setString(L"SẮP XẾP VIÊN BI");
    text.setCharacterSize(50); 
    text.setFillColor(Color::White);
    text.setPosition({100, 70}); // Chỉnh vị trí của tiêu đề
    text.setStyle(Text::Bold); // Đặt kiểu chữ đậm

    Text text2(VNfont);
    text2.setString(L"Chọn 3 màu, đặt thứ tự mong muốn, sắp tự động (Start) hoặc sắp bằng tay (kéo/thả). Thay đổi tốc độ/ pause/stop khi đang chạy.");
    text2.setCharacterSize(30);
    text2.setFillColor(Color::White);
    text2.setPosition({100, 140}); // Chỉnh vị trí của hướng dẫn    

    Text text3(VNfont);
    text3.setString(L"Chọn 3 màu mong muốn\n (cũng là thứ tự sắp xếp): ");
    text3.setCharacterSize(30);
    text3.setFillColor(Color::White);
    text3.setPosition({130, 220}); // Chỉnh vị trí của hướng dẫn chọn màu
    text3.setStyle(Text::Bold); // Đặt kiểu chữ đậm

    Text text4(VNfont);
    text4.setString(L"Thêm viên bi: ");
    text4.setCharacterSize(30);
    text4.setFillColor(Color::White);
    text4.setPosition({130, 450});  
    text4.setStyle(Text::Bold); // Đặt kiểu chữ đậm

    Text textadd(VNfont);
    textadd.setString(L"+");
    textadd.setCharacterSize(30);
    textadd.setFillColor(Color::White);
    textadd.setPosition({190, 535});
    textadd.setStyle(Text::Bold); // Đặt kiểu chữ đậm

    // tạo hình chữ nhật bo góc làm nền
    RoundedRectangleShape roundedSquare(sf::Vector2f(1820.f, 980.f), 30.f); // chiều dài, rộng, bán kính bo góc
    roundedSquare.setFillColor(Color(17, 29, 59));
    roundedSquare.setPosition({50.f, 50.f});

    // tạo hcn chứ 3 màu ban đầu
    RoundedRectangleShape roundedSquare2(sf::Vector2f(506.f, 200.f), 30.f); // chiều dài, rộng, bán kính bo góc
    roundedSquare2.setFillColor(Color(38, 54, 94));
    roundedSquare2.setPosition({100.f, 210.f});

    // tao hình chữ nhật thêm viên bi
    RoundedRectangleShape roundedSquare3(sf::Vector2f(506.f, 180.f), 30.f); // chiều dài, rộng, bán kính bo góc
    roundedSquare3.setFillColor(Color(38, 54, 94));
    roundedSquare3.setPosition({100.f, 430.f});

    // tạo hcn chứa speen ...
    RoundedRectangleShape roundedSquare4(sf::Vector2f(1194.f, 400.f), 30.f); // chiều dài, rộng, bán kính bo góc
    roundedSquare4.setFillColor(Color(38, 54, 94));
    roundedSquare4.setPosition({626.f, 210.f});

    // tạo hình chữ nhật để sắp xếp viên bi
    RoundedRectangleShape roundedSquare5(sf::Vector2f(1720.f, 380.f), 30.f); // chiều dài, rộng, bán kính bo góc
    roundedSquare5.setFillColor(Color(38, 54, 94));
    roundedSquare5.setPosition({100.f, 630.f});

    RoundedRectangleShape roundedSquarecolor1(sf::Vector2f(130.f, 80.f), 30.f); // chiều dài, rộng, bán kính bo góc
    roundedSquarecolor1.setFillColor(Color::White);
    roundedSquarecolor1.setPosition({130.f, 310.f});

    RoundedRectangleShape roundedSquarecolor2(sf::Vector2f(130.f, 80.f), 30.f); // chiều dài, rộng, bán kính bo góc
    roundedSquarecolor2.setFillColor(Color::White);
    roundedSquarecolor2.setPosition({290.f, 310.f});

    RoundedRectangleShape roundedSquarecolor3(sf::Vector2f(130.f, 80.f), 30.f); // chiều dài, rộng, bán kính bo góc
    roundedSquarecolor3.setFillColor(Color::White);
    roundedSquarecolor3.setPosition({450.f, 310.f});

    RectangleShape color1(Vector2f(80.f, 45.f));
    color1.setFillColor(Color::Red);
    color1.setPosition({155.f, 325.f});

    RectangleShape color2(Vector2f(80.f, 45.f));
    color2.setFillColor(Color::Green);
    color2.setPosition({315.f, 325.f});

    RectangleShape color3(Vector2f(80.f, 45.f));
    color3.setFillColor(Color::Blue);
    color3.setPosition({475.f, 325.f});

    RoundedRectangleShape addcolor1(sf::Vector2f(130.f, 80.f), 30.f); // chiều dài, rộng, bán kính bo góc
    addcolor1.setFillColor(Color::Red);
    addcolor1.setPosition({130.f, 510.f});

    RoundedRectangleShape addcolor2(sf::Vector2f(130.f, 80.f), 30.f); // chiều dài, rộng, bán kính bo góc
    addcolor2.setFillColor(Color::Green);
    addcolor2.setPosition({290.f, 510.f});

    RoundedRectangleShape addcolor3(sf::Vector2f(130.f, 80.f), 30.f); // chiều dài, rộng, bán kính bo góc
    addcolor3.setFillColor(Color::Blue);
    addcolor3.setPosition({450.f, 510.f});

    //thêm nút start, pause, stop, speed up, slow down ở đây nếu muốn
    RoundedRectangleShape startButton(sf::Vector2f(250.f, 80.f), 30.f);
    startButton.setFillColor(Color(47, 167, 65));
    startButton.setPosition({700.f, 240.f});

    RoundedRectangleShape pauseButton(sf::Vector2f(250.f, 80.f), 30.f);
    pauseButton.setFillColor(Color(239, 182, 46));
    pauseButton.setPosition({700.f, 340.f});

    RoundedRectangleShape stopButton(sf::Vector2f(250.f, 80.f), 30.f);
    stopButton.setFillColor(Color(210, 45, 51));
    stopButton.setPosition({700.f, 440.f});

    RoundedRectangleShape resetButton(sf::Vector2f(250.f, 80.f), 30.f);
    resetButton.setFillColor(Color(70, 130, 180));
    resetButton.setPosition({700.f, 540.f});

    int selectedIndex = -1;  
    bool isSwapping = false; 
    bool wasPressed = false;
    int nextColorIndex = 0;
    int color1Index = 0;
    int color2Index = 2;        
    int color3Index = 4;
    vector<CircleShape> circles; // Vector để lưu các viên bi
    

    while (window.isOpen()) {
        while (const optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
        }
        
        // Xử lý sự kiện chuột để chọn màuau
        if (Mouse::isButtonPressed(Mouse::Button::Left)) {
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
            if (!wasPressed) {
                if (color1.getGlobalBounds().contains(mousePos)) {
                    Color oldColor = Mau[color1Index].color;
                    int nextColorIndex = (color1Index + 1) % Mau.size(); // Tìm màu tiếp theo
                    while (nextColorIndex == color2Index || nextColorIndex == color3Index) {
                        nextColorIndex = (nextColorIndex + 1) % Mau.size(); // Tìm màu tiếp theo chưa được chọn
                    }
                    color1.setFillColor(Mau[nextColorIndex].color); // Đổi màu khi chọn
                    addcolor1.setFillColor(Mau[nextColorIndex].color); // Đồng bộ màu add viên bi
                    
                    for (auto& circle : circles) {  
                        if (circle.getFillColor() == oldColor) {
                            circle.setFillColor(Mau[nextColorIndex].color); // Đồng bộ màu viên bi đã thêm
                        }
                    }
                    color1Index = nextColorIndex;
                }
                else if (color2.getGlobalBounds().contains(mousePos)) {
                    Color oldColor = Mau[color2Index].color;
                    int nextColorIndex = (color2Index + 1) % Mau.size(); // Tìm màu tiếp theo
                    while (nextColorIndex == color1Index || nextColorIndex == color3Index) {
                        nextColorIndex = (nextColorIndex + 1) % Mau.size(); // Tìm màu tiếp theo chưa được chọn
                    }
                    color2.setFillColor(Mau[nextColorIndex].color); // Đổi màu khi chọn
                    addcolor2.setFillColor(Mau[nextColorIndex].color); // Đồng bộ màu add viên bi
                    
                    for (auto& circle : circles) {
                        if (circle.getFillColor() == oldColor) {
                            circle.setFillColor(Mau[nextColorIndex].color); // Đồng bộ màu viên bi đã thêm
                        }
                    }
                    color2Index = nextColorIndex;
                }
                else if (color3.getGlobalBounds().contains(mousePos)) {
                    Color oldColor = Mau[color3Index].color;
                    int nextColorIndex = (color3Index + 1) % Mau.size(); // Tìm màu tiếp theo
                    while (nextColorIndex == color1Index || nextColorIndex == color2Index || nextColorIndex == color3Index) {
                        nextColorIndex = (nextColorIndex + 1) % Mau.size(); // Tìm màu tiếp theo chưa được chọn
                    }
                    color3.setFillColor(Mau[nextColorIndex].color); // Đổi màu khi chọn
                    addcolor3.setFillColor(Mau[nextColorIndex].color); // Đồng bộ màu add viên bi
                    
                    for (auto& circle : circles) {
                        if (circle.getFillColor() == oldColor) {
                            circle.setFillColor(Mau[nextColorIndex].color); // Đồng bộ màu viên bi đã thêm
                        }
                    }
                    color3Index = nextColorIndex;
                } else if (addcolor1.getGlobalBounds().contains(mousePos)) {
                    CircleShape newCircle(50.f); // Tạo viên bi mới với bán kính 50
                    newCircle.setFillColor(color1.getFillColor()); // Đặt màu cho viên bi mới
                    newCircle.setOrigin({50.f, 50.f}); // Đặt origin để viên bi xoay quanh tâm
                    int currentMarbleCount = circles.size();
                    float nextX = 200.f + (currentMarbleCount * 120.f);
                    newCircle.setPosition({nextX, 825.f}); // Đặt vị trí ban đầu cho viên bi mới
                    circles.push_back(newCircle); // Thêm viên bi mới vào vector
                }
                else if (addcolor2.getGlobalBounds().contains(mousePos)) {
                    CircleShape newCircle(50.f); // Tạo viên bi mới với bán kính 50
                    newCircle.setFillColor(color2.getFillColor()); // Đặt màu cho viên bi mới
                    newCircle.setOrigin({50.f, 50.f}); // Đặt origin để viên bi xoay quanh tâm
                    int currentMarbleCount = circles.size();
                    float nextX = 200.f + (currentMarbleCount * 120.f);
                    newCircle.setPosition({nextX, 825.f}); // Đặt vị trí ban đầu cho viên bi mới
                    circles.push_back(newCircle); // Thêm viên bi mới vào vector
                }
                else if (addcolor3.getGlobalBounds().contains(mousePos)) {
                    CircleShape newCircle(50.f); // Tạo viên bi mới với bán kính 50
                    newCircle.setFillColor(color3.getFillColor()); // Đặt màu cho viên bi mới
                    newCircle.setOrigin({50.f, 50.f}); // Đặt origin để viên bi xoay quanh tâm
                    int currentMarbleCount = circles.size();
                    float nextX = 200.f + (currentMarbleCount * 120.f);
                    newCircle.setPosition({nextX, 825.f}); // Đặt vị trí ban đầu cho viên bi mới
                    circles.push_back(newCircle); // Thêm viên bi mới vào vector
                }
                wasPressed = true;
            }
        } else {
            wasPressed = false;
            selectedIndex = -1;
        }

        
        
        window.clear(Color(10, 17, 33));
        window.draw(roundedSquare);
        window.draw(roundedSquare2);
        window.draw(roundedSquare3);
        window.draw(roundedSquare4);
        window.draw(roundedSquare5);
        window.draw(roundedSquarecolor1);
        window.draw(roundedSquarecolor2);
        window.draw(roundedSquarecolor3);
        window.draw(color1);
        window.draw(color2);    
        window.draw(color3);
        window.draw(addcolor1);
        window.draw(addcolor2);
        window.draw(addcolor3);
        window.draw(startButton);
        window.draw(pauseButton);
        window.draw(stopButton);    
        window.draw(resetButton);
        for (const auto& circle : circles) {
            window.draw(circle); // Vẽ tất cả viên bi trong vector
        }
        window.draw(text);
        window.draw(text2);
        window.draw(text3);
        window.draw(text4);
        window.draw(textadd);
        window.display();
    }
    
    return 0;
}