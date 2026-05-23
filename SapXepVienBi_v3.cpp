#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>
#include <optional>

using namespace std;
using namespace sf;

unsigned int x_Screen = 1920;
unsigned int y_Screen = 1080;

struct HCN_Bo_Goc : public sf::Shape
{
public:
    HCN_Bo_Goc() : m_size({0, 0}), m_radius(0.f), m_pointsPerCorner(10) {}

    HCN_Bo_Goc(sf::Vector2f size, float radius, unsigned int pointsPerCorner = 10)
        : m_size(size), m_radius(radius), m_pointsPerCorner(pointsPerCorner)
    {
        update();
    }

    HCN_Bo_Goc &operator=(const HCN_Bo_Goc &other)
    {
        m_size = other.m_size;
        m_radius = other.m_radius;
        m_pointsPerCorner = other.m_pointsPerCorner;
        update();
        return *this;
    }

    std::size_t getPointCount() const override
    {
        return m_pointsPerCorner * 4; // 4 góc, mỗi góc có m_pointsPerCorner điểm
    }

    sf::Vector2f getPoint(std::size_t index) const override
    {
        // Xác định điểm thuộc góc nào và vị trí thứ mấy trong góc đó
        unsigned int goc = index / m_pointsPerCorner;   // 0=Dưới Phải, 1=Dưới Trái, 2=Trên Trái, 3=Trên Phải
        unsigned int thuTu = index % m_pointsPerCorner; // Điểm thứ mấy trong góc

        // Mỗi góc là 1/4 đường tròn (90 độ), chia đều cho số điểm
        float gocBatDau = goc * 90.f;                                             // Góc bắt đầu của cung (độ)
        float buocGoc = 90.f / (m_pointsPerCorner - 1);                           // Khoảng cách giữa các điểm (độ)
        float gocHienTai = (gocBatDau + thuTu * buocGoc) * (3.14159265f / 180.f); // Đổi sang radian

        // Tâm của góc bo tròn (cách mép một khoảng bằng radius)
        sf::Vector2f tamGoc;
        tamGoc.x = (goc == 0 || goc == 3) ? m_size.x - m_radius : m_radius;
        tamGoc.y = (goc == 0 || goc == 1) ? m_size.y - m_radius : m_radius;

        // Điểm trên cung tròn = tâm + radius theo hướng gocHienTai
        return {
            tamGoc.x + m_radius * std::cos(gocHienTai),
            tamGoc.y + m_radius * std::sin(gocHienTai)};
    }

private:
    sf::Vector2f m_size;
    float m_radius;
    unsigned int m_pointsPerCorner;
};

void taotext(Font &VNfont, Text &text_ResetAll, Text &text_ClearBi, Text &text_Start,
             Text &text_Exit, Text &text_Back, Text &text_Next, Text &text_Pause, Text &text_Continue, Text &text1, Text &text2);

void taotextthongbao(Font &VNfont, Text &text_Pause1, Text &text_Next1, Text &text_Back1, Text &text_Maxbi, 
    Text &text_Complete, Text &text_Trung_mau);

void taobutton(HCN_Bo_Goc &Start, HCN_Bo_Goc &ClearBi, HCN_Bo_Goc &ResetAll,
               HCN_Bo_Goc &Exit, HCN_Bo_Goc &Back, HCN_Bo_Goc &Next, HCN_Bo_Goc &Pause, HCN_Bo_Goc &Continue);

void taohinh(RectangleShape &nen, HCN_Bo_Goc &nen1, HCN_Bo_Goc &nen2, HCN_Bo_Goc &nen3);

void doimau(vector<CircleShape> &vienbi1, vector<CircleShape> &vienbi2, Color mau, int &indexMau);

void XuLiHover(vector<CircleShape> &VienBi, Vector2f mousePos);

void XuLyHoverButton(HCN_Bo_Goc &button, Vector2f mousePos);

void XuLyKeoTha(const optional<Event> &event, const RenderWindow &window, vector<CircleShape> &VienBi1,
                vector<int> &VienBi123, bool &KTra, int &VienBiDangChon, int &VienBiDangChon2, Vector2f &offset, Vector2f &VTriBanDau, bool &ktraClick, int d);

void XuLyClick(const optional<Event> &event, const RenderWindow &window, vector<CircleShape> &VienBi, bool &ktraClick);

void ThemBi(const optional<Event> &event, const RenderWindow &window, vector<CircleShape> &VienBi,
            vector<CircleShape> &DSachVienBi, vector<int> &VienBi123, bool &hienThongBao, Clock &dongHoThongBao);

void dongboVienBi(vector<CircleShape> &vienbi1, vector<CircleShape> &vienbi2, vector<CircleShape> &DSachVienBi, vector<int> &VienBi123);

void SapXepVienBi(vector<int> VienBi123, vector<pair<int, int>> &ViTriSwap);

void AnimationGapKhuc(int &cheDoAnimation, int &indexHanhDong, float &timeAccumulated,
                      vector<CircleShape> &DSachVienBi, vector<int> &VienBi123, vector<pair<int, int>> &ViTriSwap,
                      int &vt1, int &vt2, Vector2f &posStart1, Vector2f &posStart2,
                      float speed, float deltaTime);

void ChonMau(RenderWindow &window, vector<CircleShape> &vienbi1, vector<CircleShape> &vienbi2, 
    vector<CircleShape> &DSachVienBi, int &indexMau, Color &mauChon, vector<int> &VienBi123, bool &hienThongBaoTrungMau, Clock &dongHoTrungMau);

Color Mau(float x, float y, float radius);

void ve_luc_giac_nho(RenderWindow &Window, float posx, float posy, float size, Color color);

Color ve_luc_giac_to(RenderWindow &Window, Vector2f center, float size, int radius, bool isClicked);

int main()
{
    RenderWindow window(VideoMode({x_Screen, y_Screen}), "Sap xep vien bi", Style::Default, State::Windowed);
    window.setFramerateLimit(60);
    // tạo nền
    RectangleShape nen;
    HCN_Bo_Goc nen1, nen2, nen3;
    taohinh(nen, nen1, nen2, nen3);

    // tạo bi
    vector<CircleShape> vienbi1, vienbi2;
    float X_BanDau = 780.f;
    float Y_BanDau = 185.f;
    float khoangCachX = 170.f;
    for (int i = 0; i < 3; i++)
    {
        CircleShape bi(75.f);
        bi.setOrigin({75.f, 75.f});
        bi.setFillColor(Color::White);
        bi.setPosition({X_BanDau + i * khoangCachX, Y_BanDau});
        bi.setOutlineColor(Color::White);
        vienbi1.push_back(bi);
        bi.setPosition({(X_BanDau + 620.f) + i * khoangCachX, Y_BanDau});
        vienbi2.push_back(bi);
    }

    // tạo button
    HCN_Bo_Goc Start, ClearBi, ResetAll, Exit, Back, Next, Pause, Continue;
    taobutton(Start, ClearBi, ResetAll, Exit, Back, Next, Pause, Continue);

    // tạo text
    Font VNfont;
    if (!VNfont.openFromFile("c:\\WINDOWS\\Fonts\\TIMES.TTF"))
    {
        cerr << "Không thấy font" << endl;
        return -1;
    }

    Text text_ResetAll(VNfont);
    Text text_Stop(VNfont);
    Text text_ClearBi(VNfont);
    Text text_Start(VNfont);
    Text text_Exit(VNfont);
    Text text_Back(VNfont);
    Text text_Next(VNfont);
    Text text_Pause(VNfont);
    Text text_Continue(VNfont);
    Text text1(VNfont);
    Text text2(VNfont);
    taotext(VNfont, text_ResetAll, text_ClearBi, text_Start, text_Exit, text_Back, text_Next, text_Pause, text_Continue, text1, text2);

    Text text_Pause1(VNfont);
    Text text_Next1(VNfont);
    Text text_Back1(VNfont);
    Text text_Maxbi(VNfont);
    Text text_Complete(VNfont);
    Text text_Trung_mau(VNfont);
    bool hienThongBaoTrungMau = false;
    Clock dongHoTrungMau;
    taotextthongbao(VNfont, text_Pause1, text_Next1, text_Back1, text_Maxbi, text_Complete, text_Trung_mau);

    Text text_X(VNfont);
    text_X.setString("X");
    text_X.setCharacterSize(30);
    text_X.setFillColor(Color::Red);
    text_X.setStyle(Text::Bold);

    // Khai báo 2 biến để quản lý thời gian hiển thị thông báo
    bool hienThongBao = false;
    Clock dongHoThongBao;

    bool hienThongBaoPause = false;
    Clock dongHoThongBaoPause;

    vector<pair<int, int>> ViTriVienBi1; // lưu vị trí ban đầu của 3 viên bi để sau này đồng bộ khi kéo thả
    vector<pair<int, int>> ViTriSwap;
    vector<CircleShape> DSachVienBi;
    vector<int> VienBi123; // lưu 1, 2, 3 để dễ xử lí hơn
    int indexMau = 0;
    int Maxbi = 40;
    // Biến kiểm tra đã click vào bi nào trong DSachVienBi chưa, để tránh xử lí kéo thả khi chỉ hover

    bool isDragging = false;
    int indexBiDangKeo = -1;
    int VienBiDangChon2 = -1;
    Vector2f offset;
    Vector2f viTriBanDauKhiKeo;

    bool isDraggingDS = false;
    int indexBiDangKeoDS = -1;
    Vector2f offsetDS;
    Vector2f viTriBanDauKhiKeoDS;

    // --- BIẾN QUẢN LÝ ANIMATION ---
    int cheDoAnimation = 0; // 0 = chưa chạy, 1 = đang chạy, 2 = tiến 1 bước, 3 = lùi 1 bước, 4 = hoàn thành, 5 = tạm dừng, 6 = tiếp tục sau khi tạm dừng
    int indexHanhDong = 0;  // Đang chạy tới bước swap thứ mấy
    float timeAccumulated = 0;
    float speed = 400.f;
    Clock clock;
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
        window.draw(ClearBi);
        window.draw(ResetAll);
        window.draw(Exit);
        window.draw(Back);
        window.draw(Next);
        window.draw(text_ResetAll);
        window.draw(text_ClearBi);
        window.draw(text_Exit);
        window.draw(text_Back);
        window.draw(text_Next);
        window.draw(text1);
        window.draw(text2);
        // Vẽ thông báo trùng màu nếu cờ đang bật
        if (hienThongBaoTrungMau)
        {
            if (dongHoTrungMau.getElapsedTime().asSeconds() < 2.0f) // Hiện trong 2 giây
            {
                window.draw(text_Trung_mau);
            }
            else
            {
                hienThongBaoTrungMau = false; // Tắt thông báo sau 2 giây
            }
        }

        // Đã sửa 2 thành 6: Chỉ hiện khi thực sự Tạm dừng
        if (hienThongBaoPause)
        {
            if (dongHoThongBaoPause.getElapsedTime().asSeconds() < 3.0f)
            {
                window.draw(text_Pause1);
            }
            else
            {
                hienThongBaoPause = false; // Tự động tắt sau 3 giây
            }
        }

        if (cheDoAnimation == 4)
            window.draw(text_Complete);

        // Vẽ nút Start hoặc Pause/Continue tùy theo trạng thái
        if (ViTriSwap.empty())
        {
            window.draw(Start);
            window.draw(text_Start);
        }
        else
        {
            if (cheDoAnimation == 0 || cheDoAnimation == 6) // Sửa để hiện nút Continue đúng lúc
            {
                window.draw(Continue);
                window.draw(text_Continue);
            }
            else if (cheDoAnimation == 1 || cheDoAnimation == 5) // Sửa để hiện nút Pause đúng lúc
            {
                window.draw(Pause);
                window.draw(text_Pause);
            }
        }

        if (cheDoAnimation == 2)
        {
            window.draw(text_Next1);
        }
        else if (cheDoAnimation == 3)
        {
            window.draw(text_Back1);
        }

        for (auto bi : vienbi1)
            window.draw(bi);
        for (auto bi : vienbi2)
            window.draw(bi);
        for (auto bi : DSachVienBi)
            window.draw(bi);



        bool clickBangMau = false;
        float deltaTime = clock.restart().asSeconds();
        bool ktraAdd = false; // Biến kiểm tra đã click vào bi nào chưa, để tránh xử lí kéo thả khi chỉ hover
        bool ktraClick = false; // Biến kiểm tra đã click vào bi nào trong vienbi1 chưa, để xử lí hover và kéo thả đúng cách
        bool ktraClickDS = false;

        while (const optional<Event> event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
                window.close();
            
            XuLyKeoTha(event, window, vienbi2, VienBi123, isDragging, indexBiDangKeo, VienBiDangChon2, offset, viTriBanDauKhiKeo, ktraAdd, 1);
            if (ktraAdd == true) // Nếu không đang kéo bi nào, mới xử lí thêm bi
            {
                ThemBi(event, window, vienbi2, DSachVienBi, VienBi123, hienThongBao, dongHoThongBao);
                ktraAdd = false; // Reset lại biến sau khi xử lí xong
            }
            
            //XuLyClick(vienbi1, VienBiDangChon2, viTriBanDauKhiKeo, 0);
            XuLyClick(event, window, vienbi1, ktraClick);
            
            // Kiểm tra nếu thả chuột trái sau khi kéo bi 1, thì đồng bộ bi 2 theo:
            if (const auto *mouseButtonReleased = event->getIf<Event::MouseButtonReleased>())
            {
                if (mouseButtonReleased->button == Mouse::Button::Left)
                {
                    dongboVienBi(vienbi2, vienbi1, DSachVienBi, VienBi123);
                }
            }

            // xử lí các nút bấm
            // xử lí các nút bấm
            if (const auto *mouseButtonPressed = event->getIf<Event::MouseButtonPressed>())
            {
                Vector2f mousePos = window.mapPixelToCoords(mouseButtonPressed->position);
                if (mouseButtonPressed->button == Mouse::Button::Left)
                {
                    clickBangMau = true;

                    // 1. NÚT START / PAUSE / CONTINUE (Nằm cùng một vị trí tọa độ)
                    // 1. NÚT START / PAUSE / CONTINUE
                    if (Start.getGlobalBounds().contains(mousePos))
                    {
                        if (ViTriSwap.empty())
                        {
                            SapXepVienBi(VienBi123, ViTriSwap);
                            
                            if (!ViTriSwap.empty())
                            {
                                cheDoAnimation = 1;
                                indexHanhDong = 0;
                                timeAccumulated = 0.f;
                            }
                        }
                        else
                        {
                            if (cheDoAnimation == 1)
                            {
                                cheDoAnimation = 5; // Đang chạy -> Bấm Tạm dừng (để chạy nốt bước hiện tại rồi sang 6)
                                hienThongBaoPause = true;
                                dongHoThongBaoPause.restart();
                            }
                            else if (cheDoAnimation == 0 || cheDoAnimation == 6)
                            {
                                cheDoAnimation = 1; // Đang đứng im -> Chạy tiếp
                                hienThongBaoPause = false;
                            }
                            else if (cheDoAnimation == 4)
                            {
                                indexHanhDong = 0;
                                timeAccumulated = 0.f;
                                cheDoAnimation = 1;
                            }
                        }
                    }

                    // 2. NÚT TIẾP THEO (NEXT)
                    if (Next.getGlobalBounds().contains(mousePos) && indexHanhDong < ViTriSwap.size())
                    {
                        if (cheDoAnimation == 0 || cheDoAnimation == 6 || cheDoAnimation == 4)
                        {
                            cheDoAnimation = 2; // Đặt về chế độ chạy 1 bước (khi chạy xong sẽ về 0, KHÔNG hiện chữ Pause)
                            timeAccumulated = 0.f;
                        }
                    }

                    // 3. NÚT QUAY LẠI (BACK)
                    if (Back.getGlobalBounds().contains(mousePos) && indexHanhDong > 0)
                    {
                        if (cheDoAnimation == 0 || cheDoAnimation == 6 || cheDoAnimation == 4)
                        {
                            cheDoAnimation = 3; // Chuyển sang chế độ chạy lùi 1 bước (chạy xong về 0)
                            timeAccumulated = 0.f;
                            indexHanhDong--;
                        }
                    }
                    // 4. NÚT CLEAR BI
                    if (ClearBi.getGlobalBounds().contains(mousePos))
                    {
                        hienThongBaoPause = false;
                        DSachVienBi.clear();
                        VienBi123.clear();
                        ViTriSwap.clear();
                        cheDoAnimation = 0;
                        indexHanhDong = 0;
                    }

                    // 5. NÚT RESET ALL
                    if (ResetAll.getGlobalBounds().contains(mousePos))
                    {
                        hienThongBaoPause = false;
                        VienBi123.clear();
                        DSachVienBi.clear();
                        for (int i = 0; i < vienbi1.size(); i++)
                        {
                            vienbi1[i].setFillColor(Color::White);
                            vienbi2[i].setFillColor(Color::White);
                        }
                        indexMau = 0;
                        ViTriSwap.clear();
                        cheDoAnimation = 0;
                        indexHanhDong = 0;
                    }

                    // 6. NÚT THOÁT
                    if (Exit.getGlobalBounds().contains(mousePos))
                    {
                        window.close();
                    }
                }
            }

            // xu lí hover nút
            if (const auto *mouseMoved = event->getIf<Event::MouseMoved>())
            {
                Vector2f mousePos = window.mapPixelToCoords(mouseMoved->position);
                XuLyHoverButton(Start, mousePos);
                XuLyHoverButton(ClearBi, mousePos);
                XuLyHoverButton(ResetAll, mousePos);
                XuLyHoverButton(Exit, mousePos);
                XuLyHoverButton(Back, mousePos);
                XuLyHoverButton(Next, mousePos);
                XuLyHoverButton(Pause, mousePos);
                XuLyHoverButton(Continue, mousePos);
                XuLiHover(vienbi1, mousePos);
                XuLiHover(vienbi2, mousePos); 
            }
        }

        // chọn màu
        Color mauChon = ve_luc_giac_to(window, {380.f, 320.f}, 14.f, 10, clickBangMau);
        ChonMau(window, vienbi1, vienbi2, DSachVienBi, indexMau, mauChon, VienBi123, hienThongBaoTrungMau, dongHoTrungMau);

        //chạy animation nếu đang ở chế độ chạy 
        AnimationGapKhuc(cheDoAnimation, indexHanhDong, timeAccumulated, DSachVienBi, VienBi123, ViTriSwap, vt1, vt2, posStart1, posStart2, speed, deltaTime);

        if (hienThongBao == true)
        {
            if (dongHoThongBao.getElapsedTime().asSeconds() < 2.0f)
            {
                window.draw(text_Maxbi);
            }
            else
            {
                hienThongBao = false;
            }
        }

        Text tCount(VNfont, "Danh sach bi: " + to_string(DSachVienBi.size()) + " / " + to_string(Maxbi), 35);
        tCount.setPosition({70, 630});
        window.draw(tCount);
        window.display();
    }

    return 0;
}

void taotext(Font &VNfont, Text &text_ResetAll, Text &text_ClearBi, Text &text_Start,
                Text &text_Exit, Text &text_Back, Text &text_Next, Text &text_Pause, Text &text_Continue, Text &text1, Text &text2)
    {
        text_ResetAll.setString(L"Đặt lại tất cả");
        text_ResetAll.setCharacterSize(40);
        text_ResetAll.setFillColor(Color::Black);
        text_ResetAll.setStyle(Text::Bold);
        text_ResetAll.setPosition({825.f, 470.f});

        text_ClearBi.setString(L"Xóa tất cả viên bi");
        text_ClearBi.setCharacterSize(40);
        text_ClearBi.setFillColor(Color::White);
        text_ClearBi.setStyle(Text::Bold);
        text_ClearBi.setPosition({1405.f, 470.f});

        text_Start.setString(L"Bắt đầu");
        text_Start.setCharacterSize(40);
        text_Start.setFillColor(Color::White);
        text_Start.setStyle(Text::Bold);
        text_Start.setPosition({1280.f, 970.f});

        text_Exit.setString(L"Thoát");
        text_Exit.setCharacterSize(40);
        text_Exit.setFillColor(Color::White);
        text_Exit.setStyle(Text::Bold);
        text_Exit.setPosition({1785.f, 20.f});

        text_Back.setString(L"Quay lại");
        text_Back.setCharacterSize(40);
        text_Back.setFillColor(Color::White);
        text_Back.setStyle(Text::Bold);
        text_Back.setPosition({1485.f, 970.f});

        text_Next.setString(L"Tiếp theo");
        text_Next.setCharacterSize(40);
        text_Next.setFillColor(Color::White);
        text_Next.setStyle(Text::Bold);
        text_Next.setPosition({1680.f, 970.f});

        text_Pause.setString(L"Tạm dừng");
        text_Pause.setCharacterSize(40);
        text_Pause.setFillColor(Color::White);
        text_Pause.setStyle(Text::Bold);
        text_Pause.setPosition({1260.f, 970.f});

        text_Continue.setString(L"Tiếp tục");
        text_Continue.setCharacterSize(40);
        text_Continue.setFillColor(Color::White);
        text_Continue.setStyle(Text::Bold);
        text_Continue.setPosition({1280.f, 970.f});

        text1.setString(L"Click để thay đổi màu viên bi");
        text1.setCharacterSize(35);
        text1.setFillColor(Color::White);
        text1.setStyle(Text::Bold);
        text1.setPosition({700.f, 290.f});

        text2.setString(L"  Click để thêm viên bi mới\nKéo thả để thay đổi vị trí viên bi");
        text2.setCharacterSize(35);
        text2.setFillColor(Color::White);
        text2.setStyle(Text::Bold);
        text2.setPosition({1350.f, 300.f});
    }

void taotextthongbao(Font &VNfont, Text &text_Pause1, Text &text_Next1, Text &text_Back1, Text &text_Maxbi, 
    Text &text_Complete, Text &text_Trung_mau)
{
    text_Trung_mau.setString(L"Màu này đã được sử dụng!");
    text_Trung_mau.setCharacterSize(45);
    text_Trung_mau.setFillColor(Color::Yellow);
    text_Trung_mau.setStyle(Text::Bold);
    text_Trung_mau.setPosition({x_Screen / 2.f - 240.f, y_Screen / 2.f - 160.f});

    text_Pause1.setString(L"Đã tạm dừng!");
    text_Pause1.setCharacterSize(50);
    text_Pause1.setFillColor(Color::Red);
    text_Pause1.setStyle(Text::Bold);
    text_Pause1.setPosition({x_Screen / 2.f - 150.f, y_Screen / 2.f + 130.f});

    text_Next1.setString(L"Đang tiến 1 bước");
    text_Next1.setCharacterSize(45);
    text_Next1.setFillColor(Color::White);
    text_Next1.setStyle(Text::Bold);
    text_Next1.setPosition({x_Screen / 2.f - 150.f, y_Screen / 2.f + 130.f});

    text_Back1.setString(L"Đang lùi 1 bước");
    text_Back1.setCharacterSize(45);
    text_Back1.setFillColor(Color::White);
    text_Back1.setStyle(Text::Bold);
    text_Back1.setPosition({x_Screen / 2.f - 150.f, y_Screen / 2.f + 130.f});

    text_Maxbi.setString(L"Đã thêm tối đa 40 viên bi!");
    text_Maxbi.setCharacterSize(45);
    text_Maxbi.setFillColor(Color::Red);
    text_Maxbi.setStyle(Text::Bold);
    text_Maxbi.setPosition({x_Screen / 2.f - 220.f, y_Screen / 2.f + 130.f});

    text_Complete.setString(L"Đã hoàn thành sắp xếp!");
    text_Complete.setCharacterSize(45);
    text_Complete.setFillColor(Color::Green);
    text_Complete.setStyle(Text::Bold);
    text_Complete.setPosition({x_Screen / 2.f - 220.f, y_Screen / 2.f + 130.f});
}

void taobutton(HCN_Bo_Goc &Start, HCN_Bo_Goc &ClearBi, HCN_Bo_Goc &ResetAll,
               HCN_Bo_Goc &Exit, HCN_Bo_Goc &Back, HCN_Bo_Goc &Next, HCN_Bo_Goc &Pause, HCN_Bo_Goc &Continue)
{
    Vector2f KinhThuoc({480.f, 90.f});
    Start = HCN_Bo_Goc(Vector2f({200.f, 80.f}), 20.f);
    Start.setPosition({1250.f, 960.f});
    Start.setFillColor(Color::Green);
    Start.setOutlineColor(Color::White);

    ClearBi = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    ClearBi.setPosition({1330.f, 448.f});
    ClearBi.setFillColor(Color::Red);
    ClearBi.setOutlineColor(Color::White);

    ResetAll = HCN_Bo_Goc(Vector2f(KinhThuoc), 20.f);
    ResetAll.setPosition({710.f, 448.f});
    ResetAll.setFillColor(Color::White);
    ResetAll.setOutlineColor(Color::White);

    Exit = HCN_Bo_Goc(Vector2f({200.f, 80.f}), 10.f);
    Exit.setPosition({1750.f, 0.f});
    Exit.setFillColor(Color::Blue);
    Exit.setOutlineColor(Color::White);

    Back = HCN_Bo_Goc(Vector2f({200.f, 80.f}), 20.f);
    Back.setPosition({1455.f, 960.f});
    Back.setFillColor(Color::Blue);
    Back.setOutlineColor(Color::White);

    Next = HCN_Bo_Goc(Vector2f({200.f, 80.f}), 20.f);
    Next.setPosition({1660.f, 960.f});
    Next.setFillColor(Color::Blue);
    Next.setOutlineColor(Color::White);

    Pause = HCN_Bo_Goc(Vector2f({200.f, 80.f}), 20.f);
    Pause.setPosition({1250.f, 960.f});
    Pause.setFillColor(Color::Green);
    Pause.setOutlineColor(Color::White);

    Continue = HCN_Bo_Goc(Vector2f({200.f, 80.f}), 20.f);
    Continue.setPosition({1250.f, 960.f});
    Continue.setFillColor(Color::Blue);
    Continue.setOutlineColor(Color::White);
}

void taohinh(RectangleShape &nen, HCN_Bo_Goc &nen1, HCN_Bo_Goc &nen2, HCN_Bo_Goc &nen3)
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

void doimau(vector<CircleShape> &vienbi1, vector<CircleShape> &vienbi2, Color mau, int &indexMau)
{
    if (indexMau >= 3)
        return; // Chỉ tạo tối đa 3 viên bi mỗi lần gọi hàm
    vienbi1[indexMau].setFillColor(mau);
    vienbi2[indexMau].setFillColor(mau);
}

void XuLiHover(vector<CircleShape> &VienBi, Vector2f mousePos)
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

void XuLyHoverButton(HCN_Bo_Goc &button, Vector2f mousePos)
{
    // Kiểm tra xem chuột có nằm trong phạm vi của nút không
    if (button.getGlobalBounds().contains(mousePos))
    {
        button.setOutlineThickness(-5.f);
        button.setOutlineColor(Color::White); // Bo viền trắng
    }
    else
    {
        button.setOutlineThickness(0.f); // Tắt viền khi chuột đi ra
    }
}

void XuLyKeoTha(const optional<Event> &event, const RenderWindow &window, vector<CircleShape> &VienBi1,
                vector<int> &VienBi123, bool &KTra, int &VienBiDangChon, int &VienBiDangChon2, Vector2f &offset, Vector2f &VTriBanDau, bool &ktraClick, int d)
{
    // STEP 1: click chuột trái, giữ chuột trái
    if (const auto *mouseButtonPressed = event->getIf<Event::MouseButtonPressed>()) // Ktra nhấn giữ chuột
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

    // STEP 2: thả chuột trái
    if (const auto *mouseButtonRelased = event->getIf<Event::MouseButtonReleased>())
    {
        if (mouseButtonRelased->button == Mouse::Button::Left && KTra)
        {
            bool KTraSwap = false;
            Vector2f TamBiDangChon = VienBi1[VienBiDangChon].getPosition();
            // check xem đây là kéo hay click chuột
            float Check_KCach = sqrt(pow(TamBiDangChon.x - VTriBanDau.x, 2) + pow(TamBiDangChon.y - VTriBanDau.y, 2));
            if (Check_KCach < 5.f)
            {
                ktraClick = true;
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
    if (const auto *mouseMoved = event->getIf<Event::MouseMoved>())
    {
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        // Chỉ cập nhật vị trí khi đang giữ chuột (KTra = true)
        if (KTra && VienBiDangChon != -1)
        {
            VienBi1[VienBiDangChon].setPosition(mousePos + offset);
        }
        else
            XuLiHover(VienBi1, mousePos);
    }
}

void XuLyClick(const optional<Event> &event, const RenderWindow &window, vector<CircleShape> &VienBi, bool &ktraClick)
{
    if (const auto *mouseButtonPressed = event->getIf<Event::MouseButtonPressed>())
        {
            Vector2f mousePos = window.mapPixelToCoords(mouseButtonPressed->position);
            if (mouseButtonPressed->button == Mouse::Button::Left)
            {
                for (int i = 0; i < VienBi.size(); i++)
                {
                    Vector2f TamBi = VienBi[i].getPosition();
                    float KCach = sqrt(pow(mousePos.x - TamBi.x, 2) + pow(mousePos.y - TamBi.y, 2));
                    if (KCach <= VienBi[i].getRadius())
                    {
                        ktraClick = true;
                        if (VienBi[i].getOutlineThickness() == -10.f)
                        {
                            VienBi[i].setOutlineThickness(0.f);
                        }
                        else
                        {
                            cout << "Đang click vào viên bi thứ " << i + 1 << endl;
                            VienBi[i].setOutlineThickness(-10.f);

                            for (int j = 0; j < VienBi.size(); j++)
                            {
                                if (i != j)
                                {
                                    VienBi[j].setOutlineThickness(0.f);
                                }
                            }
                        }
                        break; // Thoát vòng lặp sau khi đã xử lý viên bi được click
                    }
                }
            }
        }
    
    // if (d == 0)
    // {
    //     if (VienBi[VienBiDangChon].getOutlineThickness() == -10.f)
    //     {
    //         VienBi[VienBiDangChon].setOutlineThickness(0.f);
    //     }
    //     else
    //     {
    //         cout << "Đang click vào viên bi thứ " << VienBiDangChon + 1 << endl;
    //         VienBi[VienBiDangChon].setOutlineThickness(-10.f);

    //         for (int i = 0; i < VienBi.size(); i++)
    //         {
    //             if (VienBiDangChon != i)
    //             {
    //                 VienBi[i].setOutlineThickness(0.f);
    //             }
    //         }
    //     }
    // }
    // else if (d == 1)
    // {
    //     Vector2f TamBiDangChon = VienBi[VienBiDangChon].getPosition();
    //     float Check_KCach = sqrt(pow(TamBiDangChon.x - VTriBanDau.x, 2) + pow(TamBiDangChon.y - VTriBanDau.y, 2));
    //     if (Check_KCach >= 5.f)
    //         for (int i = 0; i < VienBi.size(); i++)
    //             VienBi[i].setOutlineThickness(0.f);
    // }
    // else if (d == 2)
    // {
    //     Vector2f TamBiDangChon = VienBi[VienBiDangChon].getPosition();
    //     float Check_KCach = sqrt(pow(TamBiDangChon.x - VTriBanDau.x, 2) + pow(TamBiDangChon.y - VTriBanDau.y, 2));
    //     if (Check_KCach >= 5.f)
    //         for (int i = 0; i < VienBi.size(); i++)
    //             VienBi[i].setOutlineThickness(0.f);
    // }
}

void ThemBi(const optional<Event> &event, const RenderWindow &window, vector<CircleShape> &VienBi,
            vector<CircleShape> &DSachVienBi, vector<int> &VienBi123, bool &hienThongBao, Clock &dongHoThongBao)
{
    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
    XuLiHover(VienBi, mousePos);

    for (int i = 0; i < VienBi.size(); i++)
    {
        Vector2f TamBi = VienBi[i].getPosition();
        float KCach = sqrt(pow(mousePos.x - TamBi.x, 2) + pow(mousePos.y - TamBi.y, 2));
        if (KCach <= 75.f)
        {
            // --- CHỐT CHẶN BẢO VỆ Ở ĐÂY ---
            // Nếu đã đủ 40 viên thì bật thông báo, reset đồng hồ và thoát vòng lặp ngay
            if (DSachVienBi.size() >= 40)
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
            // tìm tọa độ x
            // CThuc: Tâm màn hình - (Tổng chiều dài của cả dãy / 2)
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

void dongboVienBi(vector<CircleShape> &vienbi1, vector<CircleShape> &vienbi2, vector<CircleShape> &DSachVienBi, vector<int> &VienBi123)
{
    // Đảm bảo 2 mảng có số lượng bi bằng nhau để không bị lỗi văng game

    // 2. Chốt kiểm tra an toàn: Nếu khay dưới chưa có bi thì thoát
    if (DSachVienBi.empty() || VienBi123.empty() || DSachVienBi.size() != VienBi123.size())
        return;

    // 3. Đọc màu từ khay mẫu
    Color mau0 = vienbi1[0].getFillColor(); // Vị trí đầu quy thành số 0
    Color mau1 = vienbi1[1].getFillColor(); // Vị trí giữa quy thành số 1
    Color mau2 = vienbi1[2].getFillColor(); // Vị trí cuối quy thành số 2

    // 4. Áp dụng quy tắc màu xuống mảng logic VienBi123
    for (int i = 0; i < DSachVienBi.size(); i++)
    {
        Color mauHienTai = DSachVienBi[i].getFillColor();

        if (mauHienTai == mau0)
            VienBi123[i] = 0;
        else if (mauHienTai == mau1)
            VienBi123[i] = 1;
        else if (mauHienTai == mau2)
            VienBi123[i] = 2;
        else
            VienBi123[i] = 1; // Màu lạ cho đứng giữa
    }
}

void SapXepVienBi(vector<int> VienBi123, vector<pair<int, int>> &ViTriSwap)
{
    ViTriSwap.clear();
    if (VienBi123.size() <= 1)
        return;

    // Thuật toán Cờ Hà Lan chạy thẳng trên mảng logic
    int low = 0, mid = 0, high = VienBi123.size() - 1;

    while (mid <= high)
    {
        if (VienBi123[mid] == 0)
        {
            if (low != mid)
            {
                swap(VienBi123[low], VienBi123[mid]);
                ViTriSwap.push_back({low, mid});
            }
            low++;
            mid++;
        }
        else if (VienBi123[mid] == 1)
        {
            mid++;
        }
        else if (VienBi123[mid] == 2)
        {
            if (mid != high && VienBi123[high] != 2)
            {
                swap(VienBi123[mid], VienBi123[high]);
                ViTriSwap.push_back({mid, high});
            }
            high--;
        }
    }
}

void AnimationGapKhuc(int &cheDoAnimation, int &indexHanhDong, float &timeAccumulated,
                      vector<CircleShape> &DSachVienBi, vector<int> &VienBi123, vector<pair<int, int>> &ViTriSwap,
                      int &vt1, int &vt2, Vector2f &posStart1, Vector2f &posStart2,
                      float speed, float deltaTime)
{
    // Đã thêm số 6 vào điều kiện dừng
    if (cheDoAnimation == 0 || cheDoAnimation == 4 || cheDoAnimation == 6 || indexHanhDong >= ViTriSwap.size() || indexHanhDong < 0)
        return;

    if (timeAccumulated == 0.f)
    {
        vt1 = ViTriSwap[indexHanhDong].first;
        vt2 = ViTriSwap[indexHanhDong].second;
        if (vt1 > vt2)
            swap(vt1, vt2);

        posStart1 = DSachVienBi[vt1].getPosition();
        posStart2 = DSachVienBi[vt2].getPosition();
    }

    timeAccumulated += deltaTime;

    float x1 = posStart1.x;
    float y1 = posStart1.y;
    float x2 = posStart2.x;
    float y2 = posStart2.y;

    float doCao = 130.f;
    float doDichX = abs(posStart2.x - posStart1.x);

    float thoiGianGD1 = doCao / speed;
    float thoiGianGD2 = doDichX / speed;
    float thoiGianGD3 = thoiGianGD1;

    float moc1 = thoiGianGD1;
    float moc2 = moc1 + thoiGianGD2;
    float tongThoiGianCapNay = moc2 + thoiGianGD3;

    if (timeAccumulated <= moc1)
    {
        float tiLe = timeAccumulated / thoiGianGD1;
        y1 += doCao * tiLe;
        y2 -= doCao * tiLe;
    }
    else if (timeAccumulated <= moc2)
    {
        y1 += doCao;
        y2 -= doCao;
        float tiLe = (timeAccumulated - moc1) / thoiGianGD2;
        x1 = posStart1.x + (posStart2.x - posStart1.x) * tiLe;
        x2 = posStart2.x + (posStart1.x - posStart2.x) * tiLe;
    }
    else if (timeAccumulated <= tongThoiGianCapNay)
    {
        x1 = posStart2.x;
        x2 = posStart1.x;
        float tiLe = (timeAccumulated - moc2) / thoiGianGD3;
        y1 = (posStart1.y + doCao) - doCao * tiLe;
        y2 = (posStart2.y - doCao) + doCao * tiLe;
    }

    DSachVienBi[vt1].setPosition({x1, y1});
    DSachVienBi[vt2].setPosition({x2, y2});

    if (timeAccumulated >= tongThoiGianCapNay)
    {
        swap(VienBi123[vt1], VienBi123[vt2]);

        DSachVienBi[vt1].setPosition(posStart2);
        DSachVienBi[vt2].setPosition(posStart1);
        swap(DSachVienBi[vt1], DSachVienBi[vt2]);

        timeAccumulated = 0.f;

        // --- BỘ NÃO QUẢN LÝ TRẠNG THÁI MỚI ---
        if (cheDoAnimation == 3)
        {
            cheDoAnimation = 0; // Lùi 1 bước xong -> Đứng im (KHÔNG hiện text)
        }
        else if (cheDoAnimation == 2)
        {
            indexHanhDong++;
            cheDoAnimation = 0; // Tiến 1 bước xong -> Đứng im (KHÔNG hiện text)
        }
        else if (cheDoAnimation == 5)
        {
            indexHanhDong++;
            cheDoAnimation = 6; // Đang chạy mà bấm Tạm dừng -> Hoàn thành bước này rồi chuyển sang 6 (HIỆN text Pause1)
        }
        else if (cheDoAnimation == 1)
        {
            indexHanhDong++;
            if (indexHanhDong >= ViTriSwap.size())
            {
                cheDoAnimation = 4;
            }
        }
    }
}

void ChonMau(RenderWindow &window, vector<CircleShape> &vienbi1, vector<CircleShape> &vienbi2, 
    vector<CircleShape> &DSachVienBi, int &indexMau, Color &mauChon, vector<int> &VienBi123, bool &hienThongBaoTrungMau, Clock &dongHoTrungMau)
{
    

    if (mauChon.a != 0) // Nếu người dùng click chọn một màu hợp lệ
    {
        // 1. KIỂM TRA MÀU ĐÃ TỒN TẠI CHƯA
        bool mauDaTonTai = false;
        for (int i = 0; i < 3; i++)
        {
            if (vienbi1[i].getFillColor() == mauChon)
            {
                mauDaTonTai = true;
                break;
            }
        }
        if (mauDaTonTai)
        {
            hienThongBaoTrungMau = true;
            dongHoTrungMau.restart();
            return;
        }

        // 2. Ktra có click vào viên bi nào chưa
        int viTriBiDuocChon1 = -1;
        for (int i = 0; i < vienbi1.size(); i++)
        {
            if (vienbi1[i].getOutlineThickness() == -10.f)
            {
                viTriBiDuocChon1 = i;
                break;
            }
        }
        // rồi thì cập nhật màu cho viên bi đó
        if (viTriBiDuocChon1 != -1)
        {
            // Cập nhật màu cho khay 1 và khay 2 đồng bộ
            vienbi1[viTriBiDuocChon1].setFillColor(mauChon);
            vienbi2[viTriBiDuocChon1].setFillColor(mauChon);

            // Cập nhật màu cho các viên bi cùng loại trong dãy bi đang sắp xếp
            for (int k = 0; k < VienBi123.size(); k++)
            {
                if (VienBi123[k] == viTriBiDuocChon1)
                {
                    DSachVienBi[k].setFillColor(mauChon);
                }
            }
            
            // Xong việc thì TẮT viền đỏ đi trả lại trạng thái bình thường
            vienbi1[viTriBiDuocChon1].setOutlineThickness(0.f);
            vienbi1[viTriBiDuocChon1].setOutlineColor(Color::White);
        }

        // chưa thì cập nhật 3 viên bi ban đầu
        if (indexMau < 3 && viTriBiDuocChon1 == -1)
        {
            // Thực hiện đổi màu như cũ
            doimau(vienbi1, vienbi2, mauChon, indexMau);
            indexMau++;

            // Cập nhật màu cho bi đang được chọn
            int viTriBiDuocChon1 = -1;
            for (int i = 0; i < vienbi1.size(); i++)
            {
                if (vienbi1[i].getOutlineThickness() == -10.f)
                {
                    viTriBiDuocChon1 = i;
                    break;
                }
            }

            if (viTriBiDuocChon1 != -1)
            {
                // Cập nhật màu cho khay 1 và 2
                vienbi1[viTriBiDuocChon1].setFillColor(mauChon);
                // (Bạn có thể giữ logic tìm viTriBiDuocChon2 ở đây nếu cần đồng bộ)

                // Cập nhật màu cho các viên bi cùng loại trong danh sách sắp xếp
                for (int k = 0; k < VienBi123.size(); k++)
                {
                    if (VienBi123[k] == viTriBiDuocChon1)
                    {
                        DSachVienBi[k].setFillColor(mauChon);
                    }
                }
            }
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
    return selectedColor;
}