#include<SFML/Graphics.hpp>
#include<iostream>
#include<cmath>
#include<vector>
#include<algorithm>
#include <utility>

using namespace std;
using namespace sf;

float x_Screen = 1920.f;
float y_Screen = 1080.f;

struct HCN_Bo_Goc : public sf::Shape {
public:
    HCN_Bo_Goc() : m_size({0,0}), m_radius(0.f), m_pointsPerCorner(10) {}

    HCN_Bo_Goc(sf::Vector2f size, float radius, unsigned int pointsPerCorner = 10)
        : m_size(size), m_radius(radius), m_pointsPerCorner(pointsPerCorner)
    {
        update();
    }

    HCN_Bo_Goc& operator=(const HCN_Bo_Goc& other) {
        m_size           = other.m_size;
        m_radius         = other.m_radius;
        m_pointsPerCorner = other.m_pointsPerCorner;
        update();
        return *this;
    }

    std::size_t getPointCount() const override {
        return m_pointsPerCorner * 4; // 4 góc, mỗi góc có m_pointsPerCorner điểm
    }

    sf::Vector2f getPoint(std::size_t index) const override {
        // Xác định điểm thuộc góc nào và vị trí thứ mấy trong góc đó
        unsigned int goc      = index / m_pointsPerCorner; // 0=Dưới Phải, 1=Dưới Trái, 2=Trên Trái, 3=Trên Phải
        unsigned int thuTu    = index % m_pointsPerCorner; // Điểm thứ mấy trong góc

        // Mỗi góc là 1/4 đường tròn (90 độ), chia đều cho số điểm
        float gocBatDau = goc * 90.f;                             // Góc bắt đầu của cung (độ)
        float buocGoc   = 90.f / (m_pointsPerCorner - 1);        // Khoảng cách giữa các điểm (độ)
        float gocHienTai = (gocBatDau + thuTu * buocGoc) * (3.14159265f / 180.f); // Đổi sang radian

        // Tâm của góc bo tròn (cách mép một khoảng bằng radius)
        sf::Vector2f tamGoc;
        tamGoc.x = (goc == 0 || goc == 3) ? m_size.x - m_radius : m_radius;
        tamGoc.y = (goc == 0 || goc == 1) ? m_size.y - m_radius : m_radius;

        // Điểm trên cung tròn = tâm + radius theo hướng gocHienTai
        return {
            tamGoc.x + m_radius * std::cos(gocHienTai),
            tamGoc.y + m_radius * std::sin(gocHienTai)
        };
    }

private:
    sf::Vector2f m_size;
    float        m_radius;
    unsigned int m_pointsPerCorner;
};

void taotext(Font& VNfont, Text& text_ResetColor, Text& text_Stop, Text& text_Start, Text& text1, Text& text2);

void taobutton(HCN_Bo_Goc& Start, HCN_Bo_Goc& Stop, HCN_Bo_Goc& ResetColor);

void taohinh(RectangleShape& nen, HCN_Bo_Goc& nen1, HCN_Bo_Goc& nen2, HCN_Bo_Goc& nen3);

void tao3vienbi(vector<CircleShape>& vienbi1, vector<CircleShape>& vienbi2);

void XuLiHover(vector<CircleShape>& VienBi, Vector2f mousePos);

void XuLyKeoTha(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& VienBi1,
    vector<CircleShape>& VienBi2, bool& KTra, int& VienBiDangChon, Vector2f& offset, pair<Vector2f, Vector2f>& VTriBanDau);

void XuLyClick(vector<CircleShape>& VienBi, int VienBiDangChon, Vector2f VTriBanDau, int d);

void ThemBi(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& VienBi, vector<CircleShape>& DSachVienBi);

int main() 
{
    RenderWindow window(VideoMode({x_Screen, y_Screen}), "Sap xep vien bi", Style::Default, State::Windowed);
    window.setFramerateLimit(60);
    
    //tạo nền
    RectangleShape nen;
    HCN_Bo_Goc nen1, nen2, nen3;
    taohinh(nen, nen1, nen2, nen3);

    //tạo bi
    vector<CircleShape> vienbi1, vienbi2;
    tao3vienbi(vienbi1, vienbi2);

    //tạo button
    HCN_Bo_Goc Start, Stop, ResetColor;
    taobutton(Start, Stop, ResetColor);

    //tạo text
    Font VNfont;
    if (!VNfont.openFromFile("c:\\WINDOWS\\Fonts\\TIMES.TTF")) {
        cerr << "Không thấy font" << endl;
        return -1;
    }
    Text text_ResetColor(VNfont);
    Text text_Stop(VNfont); 
    Text text_Start(VNfont); 
    Text text1(VNfont); 
    Text text2(VNfont); 
    taotext(VNfont, text_ResetColor, text_Stop, text_Start, text1, text2); 

    vector<CircleShape> DSachVienBi;
    bool isDragging = false; 
    int indexBiDangKeo = -1; 
    Vector2f offset;
    pair<Vector2f, Vector2f> viTriBanDauKhiKeo;

    while (window.isOpen())
    {
        while (const optional<Event> event = window.pollEvent()) 
        {
            if (event->is<Event::Closed>()) window.close();
            XuLyKeoTha(event, window, vienbi1, vienbi2, isDragging, indexBiDangKeo, offset, viTriBanDauKhiKeo);
            ThemBi(event, window, vienbi2, DSachVienBi);
        }
            
        
        // xử lí kéo thả chuột


        window.clear();
        window.draw(nen);
        window.draw(nen1);
        window.draw(nen2);
        window.draw(nen3);  
        window.draw(Start);
        window.draw(Stop);
        window.draw(ResetColor);
        window.draw(text_ResetColor);
        window.draw(text_Stop); 
        window.draw(text_Start);
        window.draw(text1);
        window.draw(text2);
        for (auto bi : vienbi1) 
        {
            window.draw(bi);
        }
        for (auto bi : vienbi2) 
        {
            window.draw(bi);
        }
        window.display();
    }
    
    return 0;
}





/*
struct lucgiacmau 
{
    ConvexShape shape;
    float scaleHienTai = 1.0f;
    float scaleMongMuon = 1.0f;
};
vector<lucgiacmau> hexs;

void taolucgiac() 
{
    const int numLayers = 10;          // Số lớp từ tâm ra ngoài
    vector<int> rows;   // Số ô mỗi hàng   
    int maxCols = 2 * numLayers - 1; // Số ô lớn nhất ở hàng giữa

    // Tính số ô mỗi hàng để tạo hình lục giác đối xứng
    for (int i = 0; i < maxCols; ++i) {
        int distFromCenter = abs(i - (numLayers - 1));
        rows.push_back(maxCols - distFromCenter);
    }    
    
    float R = 25.f; // Bán kính của mỗi ô lục giác
    float dx = sqrt(3.0f) * R; // Khoảng cách ngang giữa các tâm
    float dy = 1.5f * R;        // Khoảng cách dọc giữa các tâm

    for (int i = 0; i < (int)rows.size(); ++i) {
        int n = rows[i];
        for (int j = 0; j < n; ++j) {
            // Tính toán vị trí tương đối
            float rx = (j - (n - 1) / 2.0f) * dx;
            float ry = (i - (numLayers - 1)) * dy;

            // Tạo ConvexShape lục giác chuẩn (flat-top)
            lucgiacmau hex;
            hex.shape.setPointCount(6);
            for (int k = 0; k < 6; ++k) {
                float a = (30.f + 60.f * k) * (3.14159265f / 180.f); // Xoay 30 độ để có cạnh nằm ngang
                hex.shape.setPoint(k, { R * cos(a), R * sin(a) });
            }
            // Tạo màu ngẫu nhiên cho mỗi lục giác
            Color mauNgauNhien(
                static_cast<Uint8>(r * 255.0f),
                static_cast<Uint8>(g * 255.0f),
                static_cast<Uint8>(b * 255.0f)
            );
            hex.shape.setFillColor(mauNgauNhien);
            hex.shape.setPosition({960.f + rx, 540.f + ry}); // Đặt vị trí dựa trên
            hexs.push_back(hex);
        }
    }
}
*/

// Hàm xử lý hoán đổi vị trí cố định

void taotext(Font& VNfont, Text& text_ResetColor, Text& text_Stop, Text& text_Start, Text& text1, Text& text2) 
{
    // 1. CHỮ NÚT ĐẶT LẠI MÀU (Nút X: 710, Y: 490)
    
    text_ResetColor.setString(L"Đặt lại màu");
    text_ResetColor.setCharacterSize(40);
    text_ResetColor.setFillColor(Color::Black); // Đã sửa thành màu đen để nổi trên nền trắng
    text_ResetColor.setStyle(Text::Bold);
    text_ResetColor.setPosition({840.f, 510.f});  

    // 2. CHỮ NÚT DỪNG LẠI (Nút X: 1325, Y: 390)
    
    text_Stop.setString(L"Dừng lại");
    text_Stop.setCharacterSize(40);
    text_Stop.setFillColor(Color::White);
    text_Stop.setStyle(Text::Bold);
    text_Stop.setPosition({1485.f, 410.f});  

    // 3. CHỮ NÚT BẮT ĐẦU SẮP XẾP (Nút X: 1325, Y: 490)
   
    text_Start.setString(L"Bắt đầu sắp xếp");
    text_Start.setCharacterSize(40);
    text_Start.setFillColor(Color::White);
    text_Start.setStyle(Text::Bold);
    text_Start.setPosition({1415.f, 510.f});

    text1.setString(L"Click để thay đổi màu viên bi\nKéo thả để thay đổi vị trí viên bi");
    text1.setCharacterSize(35);
    text1.setFillColor(Color::White);
    text1.setStyle(Text::Bold);
    text1.setPosition({730.f, 300.f});

    text2.setString(L"Click để thêm viên bi mới");
    text2.setCharacterSize(35);
    text2.setFillColor(Color::White);
    text2.setStyle(Text::Bold);
    text2.setPosition({1350.f, 300.f});
}

void taobutton(HCN_Bo_Goc& Start, HCN_Bo_Goc& Stop, HCN_Bo_Goc& ResetColor) 
{
    Vector2f KinhThuoc({480.f, 90.f});
    Start = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    Start.setPosition({1325.f, 490.f});
    Start.setFillColor(Color::Green);

    Stop = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    Stop.setPosition({1325.f, 390.f});
    Stop.setFillColor(Color::Red);
    
    ResetColor = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    ResetColor.setPosition({710.f, 490.f});
    ResetColor.setFillColor(Color::White);
}

void taohinh(RectangleShape& nen, HCN_Bo_Goc& nen1, HCN_Bo_Goc& nen2, HCN_Bo_Goc& nen3) 
{
    nen = RectangleShape(Vector2f(x_Screen, y_Screen));
    nen.setFillColor(Color(26, 95, 122));
    nen.setPosition({0.f, 0.f});

    nen1 = HCN_Bo_Goc(Vector2f(x_Screen / 2 + 230.f, y_Screen / 2), 30.f);
    nen1.setFillColor(Color(15, 77, 102));
    nen1.setPosition({50.f, 50.f});

    nen2 = HCN_Bo_Goc(Vector2f(x_Screen / 2 - 360.f, y_Screen / 2), 30.f);
    nen2.setFillColor(Color(15, 77, 102));
    nen2.setPosition({x_Screen / 2 + 310.f, 50.f});

    nen3 = HCN_Bo_Goc(Vector2f(x_Screen - 100.f, y_Screen / 2 - 100.f), 30.f);
    nen3.setFillColor(Color::White);
    nen3.setPosition({50.f, y_Screen / 2 + 70.f});
};
 
void tao3vienbi(vector<CircleShape>& vienbi1, vector<CircleShape>& vienbi2) 
{
    vector<Color> mauSac = {Color::Red, Color::Green, Color::Blue};
    float X_BanDau = 780.f; 
    float Y_BanDau = 185.f;
    float khoangCachX = 170.f;
    for (int i = 0; i < 3; i++) 
    {
        CircleShape bi(75.f);
        bi.setOrigin({75.f, 75.f}); 
        bi.setFillColor(mauSac[i]);
        bi.setPosition({X_BanDau + i * khoangCachX, Y_BanDau});
        bi.setOutlineColor(Color::White);
        vienbi1.push_back(bi);
        bi.setPosition({(X_BanDau + 620.f) + i * khoangCachX, Y_BanDau});
        vienbi2.push_back(bi);
    } 
}

void XuLiHover(vector<CircleShape>& VienBi, Vector2f mousePos)
{
    for (int i = 0; i < VienBi.size(); i++) 
    {
        if (VienBi[i].getOutlineThickness() == -10.f) 
            continue;
        else 
        {
            Vector2f TamBi = VienBi[i].getPosition();
            float KCach = sqrt(pow(mousePos.x - TamBi.x, 2) + pow(mousePos.y - TamBi.y, 2));
            if (KCach <= 75.f)
                VienBi[i].setOutlineThickness(-5.f);
            else 
                VienBi[i].setOutlineThickness(0.f);
        }
    }
}

void XuLyKeoTha(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& VienBi1, 
    vector<CircleShape>& VienBi2, bool& KTra, int& VienBiDangChon, Vector2f& offset, pair<Vector2f, Vector2f>& VTriBanDau) //offset điểm neo
{
    
    // STEP 1: click chuột trái, giữ chuột trái
    if (const auto* mouseButtonPressed = event->getIf<Event::MouseButtonPressed>()) // Ktra nhấn giữ chuột
    {
        if (mouseButtonPressed->button == Mouse::Button::Left)
        {
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            for (int i = VienBi1.size() - 1; i >= 0; --i) 
            {
                pair<Vector2f, Vector2f> TamBi1 = {VienBi1[i].getPosition(), VienBi2[i].getPosition()};
                float BKinh = VienBi1[i].getRadius();
                float KCach1 = sqrt(pow(mousePos.x - TamBi1.first.x, 2) + pow(mousePos.y - TamBi1.first.y, 2));

                if (KCach1 <= BKinh)
                {
                    KTra = true;
                    VienBiDangChon = i;
                    offset = TamBi1.first - mousePos; 
                    VTriBanDau.first = TamBi1.first;
                    VTriBanDau.second = TamBi1.second;
                    break;
                }
            }
        }
    }

    //STEP 2: thả chuột trái
    if (const auto* mouseButtonRelased = event->getIf<Event::MouseButtonReleased>()) 
    {
        if (mouseButtonRelased->button == Mouse::Button::Left && KTra) 
        {
            bool KTraSwap = false;
            for (int i = 0; i < VienBi1.size(); i++) 
            {
                if (i != VienBiDangChon)
                {
                    pair<Vector2f, Vector2f> TamBi2 = {VienBi1[i].getPosition(), VienBi2[i].getPosition()};
                    Vector2f TamBiDangChon = VienBi1[VienBiDangChon].getPosition();
                    float BKinh = VienBi1[i].getRadius();
                    float KCach2 = sqrt(pow(TamBiDangChon.x - TamBi2.first.x, 2) + pow(TamBiDangChon.y - TamBi2.first.y, 2));
                    // check xem đây là kéo hay click chuột
                    float Check_KCach = sqrt(pow(TamBiDangChon.x - VTriBanDau.first.x, 2) + pow(TamBiDangChon.y - VTriBanDau.first.y, 2));
                    if (Check_KCach < 5.f)
                    {
                        XuLyClick(VienBi1, VienBiDangChon, VTriBanDau.first, 0);
                        break;
                    }
                        
                    if (KCach2 <= 100.f) 
                    {
                        VienBi1[VienBiDangChon].setPosition(TamBi2.first);
                        VienBi1[i].setPosition(VTriBanDau.first);
                        VienBi2[VienBiDangChon].setPosition(TamBi2.second);
                        VienBi2[i].setPosition(VTriBanDau.second);
                        KTraSwap = true;
                        break;
                    }
                }
            }
            if (!KTraSwap)
            {
                VienBi1[VienBiDangChon].setPosition(VTriBanDau.first);
                VienBi2[VienBiDangChon].setPosition(VTriBanDau.second);
            }             
                

            // Reset trạng thái kéo
            KTra = false;
            VienBiDangChon = -1;
        }
    }

    // STEP 3: di chuyển viên bi theo chuột
    if (const auto* mouseMoved = event->getIf<Event::MouseMoved>()) 
    {
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        // Chỉ cập nhật vị trí khi đang giữ chuột (KTra = true)
        if (KTra && VienBiDangChon != -1) 
        {
            VienBi1[VienBiDangChon].setPosition(mousePos + offset);

            XuLyClick(VienBi1, VienBiDangChon, VTriBanDau.first, 1);
        }
        else
            XuLiHover(VienBi1, mousePos);
    }
}

void XuLyClick(vector<CircleShape>& VienBi, int VienBiDangChon, Vector2f VTriBanDau, int d) 
{
    // Kiểm tra xem viên bi này có đang được bật viền hay không 0 là k bật, 1 là đang bật
    if (d == 0) 
    {
        if (VienBi[VienBiDangChon].getOutlineThickness() == -10.f) 
        {
            VienBi[VienBiDangChon].setOutlineThickness(0.f);
        }
        else 
        {
            VienBi[VienBiDangChon].setOutlineThickness(-10.f);

            for (int i = 0; i < VienBi.size(); i++) {
                if (VienBiDangChon != i) {
                    VienBi[i].setOutlineThickness(0.f);
                }
            }
        }
    }
    else 
    {
        Vector2f TamBiDangChon = VienBi[VienBiDangChon].getPosition();
        float Check_KCach = sqrt(pow(TamBiDangChon.x - VTriBanDau.x, 2) + pow(TamBiDangChon.y - VTriBanDau.y, 2));
        if (Check_KCach >= 5.f) 
            for (int i = 0; i < 3; i++)
                VienBi[i].setOutlineThickness(0.f);
    }
    
}

void ThemBi(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& VienBi, vector<CircleShape>& DSachVienBi)
{
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    XuLiHover(VienBi, mousePos); 
    
}