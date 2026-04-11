#include <iostream>
#include <vector>
#include <cstdlib> 
#include <ctime>   
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <algorithm>

using namespace std;
using namespace sf;

// 1. TẠO CLASS HÌNH BO GÓC


struct RoundedRectangleShape : public sf::Shape {
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

struct ManhGhep {
    ConvexShape shape;
    float scaleHienTai = 1.0f;
    float scaleMongMuon = 1.0f;
};

Color mauTamGiac[6] = {
        Color::Red,
        Color::Yellow,
        Color::Blue,
        Color::Green,
        Color(255, 165, 0),
        Color(199, 21, 133)
    };

int main() {

    RenderWindow window(VideoMode({1920, 1080}), L"Sắp xếp viên bi", Style::Default, State::Windowed);
    window.setFramerateLimit(60);

    // tạo hình chữ nhật bo góc làm nền
    RoundedRectangleShape nen(Vector2f(1820.f, 980.f), 30.f); // chiều dài, rộng, bán kính bo góc
    nen.setFillColor(Color(17, 29, 59));
    nen.setPosition({50.f, 50.f});
    
    Vector2f Size_Color(220.f, 200.f);
    float x_BanDau = 1000.f;
    float y_BanDau = 170.f;
    vector <RoundedRectangleShape> color[6];
    for (int i = 0; i < 6; i++) {
        RoundedRectangleShape newShape(Vector2f{Size_Color}, 20.f);
        if (i < 3) 
            newShape.setPosition({x_BanDau + i * 250, y_BanDau});
        else 
            newShape.setPosition({x_BanDau - 750.f + i * 250, y_BanDau + 270.f});
        color[i].push_back(newShape);
    }

    RoundedRectangleShape Reset_Button(Vector2f{330.f, 100.f}, 10.f);
    Reset_Button.setFillColor(Color(255, 197, 1));
    Reset_Button.setPosition({x_BanDau, 800.f});

    RoundedRectangleShape Continue_Button(Vector2f{330.f, 100.f}, 10.f);
    Continue_Button.setFillColor(Color(100, 219, 225));
    Continue_Button.setPosition({x_BanDau + 390, 800.f});

    Vector2f tam(540.f, 540.f);
    float BK = 350.f;
    const float PI = 3.141592654f;
    
    vector<ManhGhep> DSTamGiac;
    for (int i = 0; i < 6; i++) {
        Vector2f p0 = tam;
        float gocBanDau = (i * 60) * PI / 180.f;
        float gocKetThuc = ((i + 1) * 60.f) * PI / 180.f;

        Vector2f p1;
        p1.x = BK * cos(gocBanDau);
        p1.y = BK * sin(gocBanDau);

        Vector2f p2;
        p2.x = BK * cos(gocKetThuc);
        p2.y = BK * sin(gocKetThuc);

        ManhGhep mg;
        mg.shape.setPointCount(3);
        mg.shape.setPosition(tam);

        mg.shape.setPoint(0, Vector2f(0, 0));
        mg.shape.setPoint(1, Vector2f(p1.x, p1.y));
        mg.shape.setPoint(2, Vector2f(p2.x, p2.y));
        
        mg.shape.setFillColor(mauTamGiac[i]);

        DSTamGiac.push_back(mg);
    }

    int maxTG = 0;
        vector<int> MauDuocChon;

    while (window.isOpen()) {
        while (const optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) window.close();
        }
        
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));   
        
        float dx = mousePos.x - tam.x;
        float dy = mousePos.y - tam.y;
        float KhoangCachChuot = sqrt(pow(dx, 2) + pow(dy, 2)); //khoảng cách chuột đến tâm 

        float GocChuotRad = atan2(dy, dx);
        if (GocChuotRad < 0) GocChuotRad += 2 * PI;
        
        for (int i = 0; i < 6; i++) {
            float gocBanDau = (i * 60) * PI / 180.f;
            float gocKetThuc = ((i + 1) * 60 * PI / 180.f);

            if (KhoangCachChuot < BK && GocChuotRad >= gocBanDau && GocChuotRad < gocKetThuc) {
                DSTamGiac[i].scaleMongMuon = 1.1f;
                if (Mouse::isButtonPressed(Mouse::Button::Left)) {
                    if (maxTG < 3) {
                        if (find(MauDuocChon.begin(), MauDuocChon.end(), i) == MauDuocChon.end()) {
                            MauDuocChon.push_back(i); 
                            DSTamGiac[i].shape.setOutlineThickness(-3.0f);
                            DSTamGiac[i].shape.setOutlineColor(Color::White);
                            // if (maxTG == 0) color1.setFillColor(mauTamGiac[i]);
                            // if (maxTG == 1) color2.setFillColor(mauTamGiac[i]);
                            // if (maxTG == 2) color3.setFillColor(mauTamGiac[i]);
                            maxTG++;
                        }
                    }
                    
                }
                
            } else {
                DSTamGiac[i].scaleMongMuon = 1.0f;
            }
            
            
            DSTamGiac[i].scaleHienTai += (DSTamGiac[i].scaleMongMuon - DSTamGiac[i].scaleHienTai) * 0.15f;
            DSTamGiac[i].shape.setScale(Vector2f(DSTamGiac[i].scaleHienTai, DSTamGiac[i].scaleHienTai));

            window.draw(DSTamGiac[i].shape);
        }


        // window.draw(color1);
        // window.draw(color2);
        // window.draw(color3);
        window.draw(nen);
        window.draw(Continue_Button);
        window.draw(Reset_Button);
        for (int i = 0; i < 6; i++) {
            for (const auto& shape : color[i]) {
                window.draw(shape);
            }
        }
        window.display();
    }
    
    return 0;
}