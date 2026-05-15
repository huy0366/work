#include<SFML/Graphics.hpp>
#include<iostream>
#include<cmath>
#include<vector>
#include<algorithm>
#include<utility>

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

void taotext(Font& VNfont, Text& text_ResetColor, Text& text_Stop, Text& text_ClearBi, Text& text_Start, Text& text1, Text& text2);

void taobutton(HCN_Bo_Goc& Start, HCN_Bo_Goc& Stop, HCN_Bo_Goc& ClearBi, HCN_Bo_Goc& ResetColor);

void taohinh(RectangleShape& nen, HCN_Bo_Goc& nen1, HCN_Bo_Goc& nen2, HCN_Bo_Goc& nen3);

void tao3vienbi(vector<CircleShape>& vienbi1, vector<CircleShape>& vienbi2, Color mau, int& indexMau);

void XuLiHover(vector<CircleShape>& VienBi, Vector2f mousePos);

void XuLyHoverButton(HCN_Bo_Goc& button, Vector2f mousePos);

void XuLyKeoTha(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& VienBi1,
    vector<int>& VienBi123, bool& KTra, int& VienBiDangChon, Vector2f& offset, Vector2f& VTriBanDau, int d);

void XuLyClick(vector<CircleShape>& VienBi, int VienBiDangChon, Vector2f VTriBanDau, int d);

void ThemBi(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& VienBi, 
    vector<CircleShape>& DSachVienBi, vector<int>& VienBi123, bool& hienThongBao, Clock& dongHoThongBao);

void dongboVienBi(vector<CircleShape>& vienbi1, vector<CircleShape>& vienbi2, vector<CircleShape>& DSachVienBi, vector<int>& VienBi123);

bool XoaBi(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& DSachVienBi, vector<int>& VienBi123);

void SapXepVienBi(vector<int>& VienBi123, vector<pair<int, int>>& ViTriSwap);

void ChayAnimationGapKhuc(bool& dangChayAnimation, int& indexHanhDong, int& frameAnimation, const int MAX_FRAME, 
                          vector<CircleShape>& DSachVienBi, vector<pair<int, int>>& ViTriSwap, 
                          int& vt1, int& vt2, Vector2f& posStart1, Vector2f& posStart2);

Color Mau(float x, float y, float radius);

// --- HÀM VẼ HÌNH LỤC GIÁC NHỎ ---
void ve_luc_giac_nho(RenderWindow &Window, float posx, float posy, float size, Color color);

// --- HÀM VẼ LƯỚI LỤC GIÁC ---
Color ve_luc_giac_to(RenderWindow &Window, Vector2f center, float size, int radius, bool isClicked);
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
    //tạo button
    HCN_Bo_Goc Start, Stop, ClearBi, ResetColor;
    taobutton(Start, Stop, ClearBi, ResetColor);

    //tạo text
    Font VNfont;
    if (!VNfont.openFromFile("c:\\WINDOWS\\Fonts\\TIMES.TTF")) {
        cerr << "Không thấy font" << endl;
        return -1;
    }
    Text text_ResetColor(VNfont);
    Text text_Stop(VNfont); 
    Text text_ClearBi(VNfont);
    Text text_Start(VNfont); 
    Text text1(VNfont); 
    Text text2(VNfont); 
    taotext(VNfont, text_ResetColor, text_Stop, text_ClearBi, text_Start, text1, text2); 
    Text text_X(VNfont);
    text_X.setString("X");
    text_X.setCharacterSize(30);
    text_X.setFillColor(Color::Red);
    text_X.setStyle(Text::Bold);

    Text text_ThongBao(VNfont);
    text_ThongBao.setString(L"Da them toi da 20 vien bi!"); // Nếu bạn dùng được tiếng Việt có dấu thì ghi "Đã thêm tối đa 20 viên bi!"
    text_ThongBao.setCharacterSize(35);
    text_ThongBao.setFillColor(Color::Red);
    text_ThongBao.setStyle(Text::Bold);
    // Đặt dòng chữ nằm ở dưới cùng, căn giữa màn hình
    text_ThongBao.setPosition({x_Screen / 2.f - 200.f, 950.f}); 

    // Khai báo 2 biến để quản lý thời gian hiển thị thông báo
    bool hienThongBao = false;
    Clock dongHoThongBao;

    vector<pair<int, int> > ViTriVienBi1; // lưu vị trí ban đầu của 3 viên bi để sau này đồng bộ khi kéo thả
    vector<pair<int, int> > ViTriSwap;
    vector<CircleShape> DSachVienBi;
    vector<int> VienBi123; // lưu 1, 2, 3 để dễ xử lí hơn
    int indexMau = 0;

    bool isDragging = false; 
    int indexBiDangKeo = -1; 
    Vector2f offset;
    Vector2f viTriBanDauKhiKeo;

    bool isDraggingDS = false; 
    int indexBiDangKeoDS = -1; 
    Vector2f offsetDS;
    Vector2f viTriBanDauKhiKeoDS;


    // --- BIẾN QUẢN LÝ ANIMATION ---
    bool dangChayAnimation = false;
    int indexHanhDong = 0;      // Đang chạy tới bước swap thứ mấy
    int frameAnimation = 0;     // Đếm số khung hình đã bay
    const int MAX_FRAME = 60;   // Tốc độ bay (40 frame ~ 0.6 giây hoàn thành 1 lần đổi)
    Vector2f posStart1, posStart2;
    int vt1 = 0, vt2 = 0;
    // ------------------------------

    while (window.isOpen())
    {   
        window.clear();
        window.draw(nen);
        window.draw(nen1);
        window.draw(nen2);
        window.draw(nen3);  
        window.draw(Start);
    //    window.draw(Stop);
        window.draw(ClearBi);
        window.draw(ResetColor);
        window.draw(text_ResetColor);
    //    window.draw(text_Stop); 
        window.draw(text_Start);
        window.draw(text_ClearBi);
        window.draw(text1);
        window.draw(text2);
        ve_luc_giac_to(window, {380.f, 320.f}, 22.f, 6, false);

        while (const optional<Event> event = window.pollEvent()) 
        {
            if (event->is<Event::Closed>()) window.close();
            XuLyKeoTha(event, window, vienbi1, VienBi123, isDragging, indexBiDangKeo, offset, viTriBanDauKhiKeo, 1);
            ThemBi(event, window, vienbi2, DSachVienBi, VienBi123, hienThongBao, dongHoThongBao);
            // Kiểm tra nếu thả chuột trái sau khi kéo bi 1, thì đồng bộ bi 2 theo:
            if (const auto* mouseButtonReleased = event->getIf<Event::MouseButtonReleased>()) 
            {
                if (mouseButtonReleased->button == Mouse::Button::Left) 
                {
                    dongboVienBi(vienbi1, vienbi2, DSachVienBi, VienBi123);
                }
            }
            
            //xử lí các nút bấm và màu
            if (const auto* mouseButtonPressed = event->getIf<Event::MouseButtonPressed>()) 
            {
                Vector2f mousePos = window.mapPixelToCoords(mouseButtonPressed->position);
                if (mouseButtonPressed->button == Mouse::Button::Left) 
                {
                    // Chọn màu ban đầu
                    bool clickBangMau = true;
                    Color mauChon = ve_luc_giac_to(window, {380.f, 320.f}, 22.f, 6, clickBangMau);
                    clickBangMau = false; // reset lại sau khi lấy màu
                    if (mauChon.a != 0)
                    {
                        tao3vienbi(vienbi1, vienbi2, mauChon, indexMau);
                    }

                    // Kiểm tra nếu click vào nút Start
                    if (Start.getGlobalBounds().contains(mousePos)) 
                    {
                        SapXepVienBi(VienBi123, ViTriSwap);
                        if (!ViTriSwap.empty()) 
                        {
                            dangChayAnimation = true;
                            indexHanhDong = 0;
                            frameAnimation = 0;
                            
                            vt1 = ViTriSwap[0].first;
                            vt2 = ViTriSwap[0].second;
                            if (vt1 > vt2) swap(vt1, vt2); // Đảm bảo vt1 luôn ở bên trái
                            
                            // Lưu lại tọa độ gốc trước khi cất cánh
                            posStart1 = DSachVienBi[vt1].getPosition();
                            posStart2 = DSachVienBi[vt2].getPosition();
                        }
                        
                    }

                    // Kiểm tra nếu click vào nút ClearBi
                    if (ClearBi.getGlobalBounds().contains(mousePos))
                    {
                        DSachVienBi.clear();
                        VienBi123.clear();
                        ViTriSwap.clear();
                    }

                    if (ResetColor.getGlobalBounds().contains(mousePos))
                    {
                        VienBi123.clear();
                        DSachVienBi.clear();
                        vienbi1.clear();
                        vienbi2.clear();
                        indexMau = 0;
                    }

                    // Kiểm tra nếu click vào nút ResetColor
                }
            }

            bool daXoaBi = XoaBi(event, window, DSachVienBi, VienBi123);

            if (daXoaBi == false) // Nếu không xóa bi nào, mới xử lí kéo thả dãy bi
                XuLyKeoTha(event, window, DSachVienBi, VienBi123, isDraggingDS, indexBiDangKeoDS, offsetDS, viTriBanDauKhiKeoDS, 10);

            //xu lí hover nút
            if (const auto* mouseMoved = event->getIf<Event::MouseMoved>())
            {
                Vector2f mousePos = window.mapPixelToCoords(mouseMoved->position);
                XuLyHoverButton(Start, mousePos);
                XuLyHoverButton(ClearBi, mousePos); 
                XuLyHoverButton(ResetColor, mousePos);

            }
            
        }


        for (auto bi : vienbi1) 
            window.draw(bi);
        for (auto bi : vienbi2) 
            window.draw(bi);
        for (auto bi : DSachVienBi)
            window.draw(bi);

        // --- ĐỘNG CƠ ANIMATION BAY GẤP KHÚC VUÔNG GÓC ---
        ChayAnimationGapKhuc(dangChayAnimation, indexHanhDong, frameAnimation, MAX_FRAME, DSachVienBi, ViTriSwap, vt1, vt2, posStart1, posStart2);
        // ------------------------------------------------

        // (Các vòng lặp for vẽ vienbi1, vienbi2, DSachVienBi của bạn nằm ở dưới đây...)
    


        for (int i = 0; i < DSachVienBi.size(); i++)
        {
            // Kiểm tra: Nếu viên bi này đang được chọn (có viền -10)
            if (DSachVienBi[i].getOutlineThickness() == -10.f)
            {
                // Tính tọa độ góc trên bên phải
                float R = DSachVienBi[i].getRadius() * DSachVienBi[i].getScale().x;
                Vector2f TamBi = DSachVienBi[i].getPosition();
                
                float x_TopRight = TamBi.x + R * 0.707f;
                float y_TopRight = TamBi.y - R * 0.707f;
                
                // Đặt vị trí chữ X lệch vào tâm một chút cho cân đối
                text_X.setPosition({x_TopRight - 10.f, y_TopRight - 20.f});
                
                // Vẽ chữ X ra
                window.draw(text_X);
            }
        }
        // NẾU ĐANG BỊ BẬT CỜ BÁO LỖI
        if (hienThongBao == true) 
        {
            // Kiểm tra xem đồng hồ chạy được mấy giây rồi. Nếu dưới 2 giây thì vẽ chữ ra.
            if (dongHoThongBao.getElapsedTime().asSeconds() < 2.0f) 
            {
                window.draw(text_ThongBao);
            } 
            else 
            {
                // Nếu đã qua 2 giây -> Tự động tắt cờ, giấu chữ đi
                hienThongBao = false;
            }
        }
        
        window.display();
    }
    
    return 0;
}



void taotext(Font& VNfont, Text& text_ResetColor, Text& text_Stop, Text& text_ClearBi, Text& text_Start, Text& text1, Text& text2)
{
    // 1. CHỮ NÚT ĐẶT LẠI MÀU (Nút X: 710, Y: 490)
    
    text_ResetColor.setString(L"Đặt lại màu");
    text_ResetColor.setCharacterSize(40);
    text_ResetColor.setFillColor(Color::Black); // Đã sửa thành màu đen để nổi trên nền trắng
    text_ResetColor.setStyle(Text::Bold);
    text_ResetColor.setPosition({840.f, 510.f});  

    // 2. CHỮ NÚT DỪNG LẠI (Nút X: 1325, Y: 390)
    // text_Stop.setString(L"Dừng lại");
    // text_Stop.setCharacterSize(40);
    // text_Stop.setFillColor(Color::White);
    // text_Stop.setStyle(Text::Bold);
    // text_Stop.setPosition({1485.f, 410.f});  

    text_ClearBi.setString(L"Xóa tất cả viên bi");
    text_ClearBi.setCharacterSize(40);
    text_ClearBi.setFillColor(Color::White);
    text_ClearBi.setStyle(Text::Bold);
    text_ClearBi.setPosition({1405.f, 410.f});  

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

void taobutton(HCN_Bo_Goc& Start, HCN_Bo_Goc& Stop, HCN_Bo_Goc& ClearBi, HCN_Bo_Goc& ResetColor) 
{
    Vector2f KinhThuoc({480.f, 90.f});
    Start = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    Start.setPosition({1325.f, 490.f});
    Start.setFillColor(Color::Green);
    Start.setOutlineColor(Color::White);

    // Stop = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    // Stop.setPosition({1325.f, 390.f});
    // Stop.setFillColor(Color::Red);
    // Stop.setOutlineColor(Color::White);
    
    ClearBi = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    ClearBi.setPosition({1325.f, 390.f});
    ClearBi.setFillColor(Color::Red);
    ClearBi.setOutlineColor(Color::White);

    ResetColor = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    ResetColor.setPosition({710.f, 490.f});
    ResetColor.setFillColor(Color::White);
    ResetColor.setOutlineColor(Color::White);
}

void taohinh(RectangleShape& nen, HCN_Bo_Goc& nen1, HCN_Bo_Goc& nen2, HCN_Bo_Goc& nen3) 
{
    nen = RectangleShape(Vector2f(x_Screen, y_Screen));
    nen.setFillColor(Color(30, 30, 46));
    nen.setPosition({0.f, 0.f});

    nen1 = HCN_Bo_Goc(Vector2f(x_Screen / 2 + 230.f, y_Screen / 2), 30.f);
    nen1.setFillColor(Color(40, 42, 54));
    nen1.setPosition({50.f, 50.f});

    nen2 = HCN_Bo_Goc(Vector2f(x_Screen / 2 - 360.f, y_Screen / 2), 30.f);
    nen2.setFillColor(Color(40, 42, 54));
    nen2.setPosition({x_Screen / 2 + 310.f, 50.f});

    nen3 = HCN_Bo_Goc(Vector2f(x_Screen - 100.f, y_Screen / 2 - 100.f), 30.f);
    nen3.setFillColor(Color(68, 71, 90));
    nen3.setPosition({50.f, y_Screen / 2 + 70.f});
};
 
void tao3vienbi(vector<CircleShape>& vienbi1, vector<CircleShape>& vienbi2, Color mau, int& indexMau) 
{
    if (indexMau >= 3) return; // Chỉ tạo tối đa 3 viên bi mỗi lần gọi hàm
    float X_BanDau = 780.f; 
    float Y_BanDau = 185.f;
    float khoangCachX = 170.f;
    CircleShape bi(75.f);
    bi.setOrigin({75.f, 75.f}); 
    bi.setFillColor(mau);
    bi.setPosition({X_BanDau + indexMau * khoangCachX, Y_BanDau});
    bi.setOutlineColor(Color::White);
    vienbi1.push_back(bi);
    bi.setPosition({(X_BanDau + 620.f) + indexMau * khoangCachX, Y_BanDau});
    vienbi2.push_back(bi);
    indexMau++;
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

void XuLyHoverButton(HCN_Bo_Goc& button, Vector2f mousePos)
{
    // Kiểm tra xem chuột có nằm trong phạm vi của nút không
    if (button.getGlobalBounds().contains(mousePos)) 
    {
        button.setOutlineThickness(-5.f);
        button.setOutlineColor(Color::White); // Bo viền trắng
    } 
    else 
    {
        button.setOutlineThickness(0.f);      // Tắt viền khi chuột đi ra
    }
}

void XuLyKeoTha(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& VienBi1, 
    vector<int>& VienBi123, bool& KTra, int& VienBiDangChon, Vector2f& offset, Vector2f& VTriBanDau, int d) //offset điểm neo // d 1 là kéo thả viên bi, d 10 là kéo thả dãy bi
{
    
    // STEP 1: click chuột trái, giữ chuột trái
    if (const auto* mouseButtonPressed = event->getIf<Event::MouseButtonPressed>()) // Ktra nhấn giữ chuột
    {
        if (mouseButtonPressed->button == Mouse::Button::Left)
        {
            
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            for (int i = VienBi1.size() - 1; i >= 0; --i) 
            {
                Vector2f TamBi1 = VienBi1[i].getPosition();
                float BKinh = VienBi1[i].getRadius();
                float KCach1 = sqrt(pow(mousePos.x - TamBi1.x, 2) + pow(mousePos.y - TamBi1.y, 2));

                if (KCach1 <= BKinh)
                {
                    KTra = true;
                    VienBiDangChon = i;
                    offset = TamBi1 - mousePos; 
                    VTriBanDau = TamBi1;
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
            Vector2f TamBiDangChon = VienBi1[VienBiDangChon].getPosition();
            // check xem đây là kéo hay click chuột
            float Check_KCach = sqrt(pow(TamBiDangChon.x - VTriBanDau.x, 2) + pow(TamBiDangChon.y - VTriBanDau.y, 2));
            if (Check_KCach < 5.f)
            {
                XuLyClick(VienBi1, VienBiDangChon, VTriBanDau, 0);
                
            }
            for (int i = 0; i < VienBi1.size(); i++) 
            {
                if (i != VienBiDangChon)
                {
                    Vector2f TamBi2 = VienBi1[i].getPosition();
                    
                    float BKinh = VienBi1[i].getRadius();
                    float KCach2 = sqrt(pow(TamBiDangChon.x - TamBi2.x, 2) + pow(TamBiDangChon.y - TamBi2.y, 2));
                    
                    if (KCach2 <= VienBi1[i].getRadius() * 1.f)
                    {
                        VienBi1[VienBiDangChon].setPosition(TamBi2);
                        VienBi1[i].setPosition(VTriBanDau);
                        swap(VienBi1[VienBiDangChon], VienBi1[i]);
                        if (d == 10)
                        {
                            // Cập nhật vị trí trong mảng đồng bộ
                            swap(VienBi123[VienBiDangChon], VienBi123[i]);
                        }
                        
                        KTra = true;
                        KTraSwap = true;
                        break;
                    }
                }
            }
            if (!KTraSwap)
            {
                VienBi1[VienBiDangChon].setPosition(VTriBanDau);
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

                //XuLyClick(VienBi1, VienBiDangChon, VTriBanDau, 0);
        }
        else
            XuLiHover(VienBi1, mousePos);
    }

}

void XuLyClick(vector<CircleShape>& VienBi, int VienBiDangChon, Vector2f VTriBanDau, int d) 
{
    //Kiểm tra xem viên bi này có đang được bật viền hay không 0 là k bật, 1 là đang bật VienBi1, 2 là đang bật DSachVienBi
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
    else if (d == 1)
    {
        Vector2f TamBiDangChon = VienBi[VienBiDangChon].getPosition();
        float Check_KCach = sqrt(pow(TamBiDangChon.x - VTriBanDau.x, 2) + pow(TamBiDangChon.y - VTriBanDau.y, 2));
        if (Check_KCach >= 5.f) 
            for (int i = 0; i < VienBi.size(); i++)
                VienBi[i].setOutlineThickness(0.f);
    }
    // else if (d == 2)
    // {
    //     Vector2f TamBiDangChon = VienBi[VienBiDangChon].getPosition();
    //     float Check_KCach = sqrt(pow(TamBiDangChon.x - VTriBanDau.x, 2) + pow(TamBiDangChon.y - VTriBanDau.y, 2));
    //     if (Check_KCach >= 5.f) 
    //         for (int i = 0; i < VienBi.size(); i++)
    //             VienBi[i].setOutlineThickness(0.f);
    // }
    
}

void ThemBi(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& VienBi, 
    vector<CircleShape>& DSachVienBi, vector<int>& VienBi123, bool& hienThongBao, Clock& dongHoThongBao)
{
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    XuLiHover(VienBi, mousePos); 
    
    if (const auto* mouseButtonPressed = event->getIf<Event::MouseButtonPressed>()) // Ktra nhấn giữ chuột
    {
        if (mouseButtonPressed->button == Mouse::Button::Left)
        {
            for (int i = 0; i < VienBi.size(); i++)
            {
                Vector2f TamBi = VienBi[i].getPosition();
                float KCach = sqrt(pow(mousePos.x - TamBi.x, 2) + pow(mousePos.y - TamBi.y, 2));
                if (KCach <= 75.f)
                {
                    // --- CHỐT CHẶN BẢO VỆ Ở ĐÂY ---
                    // Nếu đã đủ 20 viên thì bật thông báo, reset đồng hồ và thoát vòng lặp ngay
                    if (DSachVienBi.size() >= 20)
                    {
                        hienThongBao = true;
                        dongHoThongBao.restart();
                        break; 
                    }
                    CircleShape bi(70.f);
                    bi.setOrigin({70.f, 70.f});
                    bi.setFillColor(VienBi[i].getFillColor());
                    bi.setOutlineThickness(0.f);
                    bi.setOutlineColor(Color::White);
                    DSachVienBi.push_back(bi);
                    VienBi123.push_back(i); // lưu số thứ tự của viên bi được thêm vào

                    float X_Center = x_Screen / 2.f;
                    float Y_BanDau = 830.f;
                    float X_KCach = 160.f;
                    int N = DSachVienBi.size();
                    //tìm tọa độ x
                    //CThuc: Tâm màn hình - (Tổng chiều dài của cả dãy / 2)
                    float X_BanDau = X_Center - ((N - 1) * X_KCach / 2);
                    float Max_Width = x_Screen - 150.f;
                    float DSVB_Width = ((N - 1) * X_KCach) + (75.f * 2); // chiều dài DSachVienBi
                    float Scale = 1.0f;
                    if (DSVB_Width > Max_Width)
                    {
                        Scale = Max_Width / DSVB_Width;
                        X_KCach *= Scale;
                        X_BanDau = X_Center - ((N - 1) * X_KCach) / 2.0f;
                    }
                    
                    for (int j = 0; j < N; j++)
                    {
                        DSachVienBi[j].setPosition({X_BanDau + j * X_KCach, Y_BanDau});
                        DSachVienBi[j].setScale({Scale, Scale});
                    }
                    break;
                }
            }
        }
    }
}

void dongboVienBi(vector<CircleShape>& vienbi1, vector<CircleShape>& vienbi2, vector<CircleShape>& DSachVienBi, vector<int>& VienBi123)
{
    // Đảm bảo 2 mảng có số lượng bi bằng nhau để không bị lỗi văng game
    if (vienbi1.size() != vienbi2.size()) return;

    for (int i = 0; i < vienbi1.size(); i++)
    {
        // 1. Đồng bộ màu: Viết thêm dòng này để lỡ bi 1 đổi màu thì bi 2 cũng đổi theo
        vienbi2[i].setFillColor(vienbi1[i].getFillColor());

        // 2. Đồng bộ vị trí: Lấy tọa độ của bi 1
        Vector2f viTriBi1 = vienbi1[i].getPosition();

        // Ép bi 2 đi theo bi 1, cộng thêm 620 pixel trục X để nó nằm ở bên phải
        vienbi2[i].setPosition({viTriBi1.x + 620.f, viTriBi1.y});
    }   
    // 2. Chốt kiểm tra an toàn: Nếu khay dưới chưa có bi thì thoát
    if (DSachVienBi.empty() || VienBi123.empty() || DSachVienBi.size() != VienBi123.size()) return;

    // 3. Đọc màu từ khay mẫu
    Color mau0 = vienbi1[0].getFillColor(); // Vị trí đầu quy thành số 0
    Color mau1 = vienbi1[1].getFillColor(); // Vị trí giữa quy thành số 1
    Color mau2 = vienbi1[2].getFillColor(); // Vị trí cuối quy thành số 2

    // 4. Áp dụng quy tắc màu xuống mảng logic VienBi123
    for (int i = 0; i < DSachVienBi.size(); i++) {
        Color mauHienTai = DSachVienBi[i].getFillColor();
        
        if (mauHienTai == mau0) VienBi123[i] = 0;
        else if (mauHienTai == mau1) VienBi123[i] = 1;
        else if (mauHienTai == mau2) VienBi123[i] = 2;
        else VienBi123[i] = 1; // Màu lạ cho đứng giữa
    }
}

bool XoaBi(const optional<Event>& event, const RenderWindow& window, vector<CircleShape>& DSachVienBi, vector<int>& VienBi123)
{
    if (const auto* mouseButtonPressed = event->getIf<Event::MouseButtonPressed>())
    {
        if (mouseButtonPressed->button == Mouse::Button::Left)
        {
            Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

            for (int i = 0; i < DSachVienBi.size(); i++)
            {
                // Chỉ xét viên bi đang được bật viền đỏ (-10.f)
                if (DSachVienBi[i].getOutlineThickness() == -10.f)
                {
                    // Toán học: Lấy tọa độ góc trên bên phải (chỗ đặt dấu X)
                    float R = DSachVienBi[i].getRadius() * DSachVienBi[i].getScale().x;
                    Vector2f TamBi = DSachVienBi[i].getPosition();
                    float x_TopRight = TamBi.x + R * cos(45 * 3.14159265f / 180.f);
                    float y_TopRight = TamBi.y - R * sin(45 * 3.14159265f / 180.f);

                    // Tính khoảng cách từ chuột tới chữ X
                    float KCach = sqrt(pow(mousePos.x - x_TopRight, 2) + pow(mousePos.y - y_TopRight, 2));

                    // Nếu click trúng (phạm vi 25px) -> BẮN BỎ!
                    if (KCach <= 25.f) 
                    {
                        DSachVienBi.erase(DSachVienBi.begin() + i);
                        VienBi123.erase(VienBi123.begin() + i); // Nhớ xóa luôn mã màu

                        // --- CĂN GIỮA LẠI DÃY BI ---
                        int N = DSachVienBi.size();
                        if (N > 0) 
                        {
                            float X_Center = x_Screen / 2.f;
                            float Y_BanDau = 830.f;
                            float X_KCach = 160.f;
                            float Max_Width = x_Screen - 150.f;
                            float DSVB_Width = ((N - 1) * X_KCach) + (70.f * 2); 
                            float Scale = 1.0f;

                            if (DSVB_Width > Max_Width) {
                                Scale = Max_Width / DSVB_Width;
                                X_KCach *= Scale;
                            }

                            float X_BanDau = X_Center - ((N - 1) * X_KCach) / 2.0f;
                            for (int j = 0; j < N; j++) {
                                DSachVienBi[j].setPosition({X_BanDau + j * X_KCach, Y_BanDau});
                                DSachVienBi[j].setScale({Scale, Scale});
                            }
                        }
                        
                        return true; // Trả về true nếu đã xóa bi
                    }
                }
            }
        }
    }
    return false;
}

void SapXepVienBi(vector<int>& VienBi123, vector<pair<int, int>>& ViTriSwap) 
{
    // Hàm này sẽ chứa thuật toán sắp xếp viên bi, được gọi khi nhấn nút "Bắt đầu sắp xếp"
    // Thuật toán có thể là Bubble Sort, Selection Sort, hoặc bất kỳ thuật toán nào bạn muốn
    // Trong quá trình sắp xếp, bạn có thể thêm hiệu ứng đổi màu hoặc di chuyển viên bi để trực quan hơn
    for (int i = 0; i < VienBi123.size() - 1; i++)
    {
        for (int j = i + 1; j < VienBi123.size(); j++)
        {
            if (VienBi123[i] > VienBi123[j])
            {
                // Swap trong mảng số thứ tự
                swap(VienBi123[i], VienBi123[j]);
                // Swap trong mảng hình ảnh
                // Lưu lại vị trí swap để sau này có thể tạo hiệu ứng di chuyển nếu muốn
                ViTriSwap.push_back({i, j});
            }
        }
    }
}

void ChayAnimationGapKhuc(bool& dangChayAnimation, int& indexHanhDong, int& frameAnimation, const int MAX_FRAME, 
                          vector<CircleShape>& DSachVienBi, vector<pair<int, int>>& ViTriSwap, 
                          int& vt1, int& vt2, Vector2f& posStart1, Vector2f& posStart2)
{
    // Nếu không được bật cờ chạy hoặc đã chạy hết kịch bản thì thoát luôn
    if (!dangChayAnimation || indexHanhDong >= ViTriSwap.size()) return;

    frameAnimation++;
    
    float x1 = posStart1.x;
    float y1 = posStart1.y;
    float x2 = posStart2.x;
    float y2 = posStart2.y;
    float doCao = 130.f; // Quãng đường chạy lên/xuống

    // GIAI ĐOẠN 1 (Frame 1-20): Tách ra theo trục Y
    if (frameAnimation <= 20) 
    {
        float tiLe = (float)frameAnimation / 20.f; // chạy từ 0 đến 1 theo thời gian
        y1 += doCao * tiLe; // Bi trái chạy XUỐNG (+Y)
        y2 -= doCao * tiLe; // Bi phải chạy LÊN (-Y)
    }
    // GIAI ĐOẠN 2 (Frame 21-40): Chạy ngang qua nhau theo trục X
    else if (frameAnimation <= 40) 
    {
        y1 += doCao;
        y2 -= doCao;
        float tiLe = (float)(frameAnimation - 20) / 20.f;
        x1 = posStart1.x + (posStart2.x - posStart1.x) * tiLe;
        x2 = posStart2.x + (posStart1.x - posStart2.x) * tiLe;
    }
    // GIAI ĐOẠN 3 (Frame 41-60): Chạy gập vào vị trí đích theo trục Y
    else if (frameAnimation <= 60) 
    {
        x1 = posStart2.x;
        x2 = posStart1.x;
        float tiLe = (float)(frameAnimation - 40) / 20.f;
        y1 = (posStart1.y + doCao) - doCao * tiLe;
        y2 = (posStart2.y - doCao) + doCao * tiLe;
    }

    // Áp dụng tọa độ vật lý cho 2 viên bi
    DSachVienBi[vt1].setPosition({x1, y1});
    DSachVienBi[vt2].setPosition({x2, y2});

    // KHI HOÀN THÀNH ĐỦ 60 FRAME (ĐÃ VỀ ĐÍCH AN TOÀN)
    if (frameAnimation >= MAX_FRAME)
    {
        // Ghim chặt tọa độ đích
        DSachVienBi[vt1].setPosition(posStart2);
        DSachVienBi[vt2].setPosition(posStart1);

        // TRÁO ĐỔI THỰC SỰ TRONG MẢNG HÌNH ẢNH
        swap(DSachVienBi[vt1], DSachVienBi[vt2]);

        // Qua bước tiếp theo
        indexHanhDong++;
        frameAnimation = 0;

        // Nạp thông số cho cặp tiếp theo (nếu còn)
        if (indexHanhDong < ViTriSwap.size()) 
        {
            vt1 = ViTriSwap[indexHanhDong].first;
            vt2 = ViTriSwap[indexHanhDong].second;
            if (vt1 > vt2) swap(vt1, vt2);
            
            posStart1 = DSachVienBi[vt1].getPosition();
            posStart2 = DSachVienBi[vt2].getPosition();
        }
        else 
        {
            dangChayAnimation = false; // Tắt máy chiếu
            ViTriSwap.clear(); // Xóa hết kịch bản đã chạy xong
        }
    }
}


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
    for (auto &h : normal)
        ve_luc_giac_nho(Window, h.p.x, h.p.y, h.s, h.c);
    for (auto &h : hovered)
        ve_luc_giac_nho(Window, h.p.x, h.p.y, h.s, h.c);
    return selectedColor;}
