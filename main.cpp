#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace sf;
using namespace std;

// --- CẤU TRÚC DỮ LIỆU ---
vector<Color> mauDuocChonTuMH2;  // Lưu 3 màu của hoán vị đã chọn
vector<CircleShape> dsBiHienThi; // Chứa các viên bi được tạo thêm ở MH3
struct Manhghep
{
    ConvexShape Manh;
    Color maugoc;
    float scale_hientai = 1.0f;
    float scale_Muctieu = 1.0f;
};

struct ButtonUI
{
    ConvexShape hinh;
    Text chu;
    float scale_hientai = 1.0f;
    float scale_Muctieu = 1.0f;

    ButtonUI(const Font &font, string noi_dung, unsigned int size)
        : chu(font, noi_dung, size) {}

    void update(Vector2f mouse)
    {
        scale_Muctieu = (hinh.getGlobalBounds().contains(mouse)) ? 1.05f : 1.0f;
        scale_hientai += (scale_Muctieu - scale_hientai) * 0.15f;
        hinh.setScale({scale_hientai, scale_hientai});
    }

    void draw(RenderWindow &window)
    {
        window.draw(hinh);
        window.draw(chu);
    }
};

// --- HÀM HỖ TRỢ VẼ HÌNH ---
ConvexShape Tam_giac(Vector2f Vi_tri_dat, float radius, float g1, float g2, Color color)
{
    ConvexShape shape(3);
    shape.setPoint(0, {0, 0});
    shape.setPoint(1, {radius * cos(g1), radius * sin(g1)});
    shape.setPoint(2, {radius * cos(g2), radius * sin(g2)});
    shape.setFillColor(color);
    shape.setPosition(Vi_tri_dat);
    return shape;
}

ConvexShape HCN_bo_goc(Vector2f size, float radius, Color color, Vector2f position)
{
    const int Diem_ve = 20;
    ConvexShape shape(Diem_ve * 4);
    float pi = 3.141592654f;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < Diem_ve; j++)
        {
            float goc = (i * 90.f + j * (90.f / (Diem_ve - 1))) * pi / 180.f;
            Vector2f tam;
            if (i == 0)
                tam = {size.x - radius, size.y - radius};
            else if (i == 1)
                tam = {radius, size.y - radius};
            else if (i == 2)
                tam = {radius, radius};
            else
                tam = {size.x - radius, radius};
            shape.setPoint(i * Diem_ve + j, {tam.x + radius * cos(goc), tam.y + radius * sin(goc)});
        }
    }
    shape.setOrigin(size / 2.f);
    shape.setFillColor(color);
    shape.setPosition(position + size / 2.f);
    return shape;
}

// --- CÁC HÀM XỬ LÝ LOGIC ---

void khoiTaoHoanVi(const vector<Color> &maudachon, vector<vector<Color>> &dsHoanVi, vector<ButtonUI> &dsKhung, const Font &font)
{
    dsHoanVi.clear();
    vector<int> p = {0, 1, 2};
    do
    {
        dsHoanVi.push_back({maudachon[p[0]], maudachon[p[1]], maudachon[p[2]]});
    } while (next_permutation(p.begin(), p.end()));

    dsKhung.clear();
    for (int i = 0; i < (int)dsHoanVi.size(); i++)
    {
        dsKhung.emplace_back(font, "Cach " + to_string(i + 1), 22);
        dsKhung[i].hinh = HCN_bo_goc({700, 140}, 15, Color(50, 50, 50), {250, 180.f + i * 180.f});
        dsKhung[i].chu.setPosition(dsKhung[i].hinh.getPosition() - Vector2f(300, 15));
    }
}

void xuLySuKien(RenderWindow &window, int &trangThai, vector<Color> &maudachon, vector<Manhghep> &Lucgiac,
                ButtonUI &btnReset, ButtonUI &btnCont, ButtonUI &btnBack, vector<vector<Color>> &dsHoanVi,
                vector<ButtonUI> &dsKhung, const Font &font, float &scrollTarget, Vector2f Tam, float radius, View &mainView)
{

    Vector2f mouseNormal = window.mapPixelToCoords(Mouse::getPosition(window), window.getDefaultView());

    while (const auto event = window.pollEvent())
    {
        if (event->is<Event::Closed>())
            window.close();

        // Cuộn chuột
        if (trangThai == 1)
        {
            if (const auto *scroll = event->getIf<Event::MouseWheelScrolled>())
            {
                scrollTarget -= scroll->delta * 100.f; // Độ nhạy cuộn
                if (scrollTarget < 0)
                    scrollTarget = 0;
                if (scrollTarget > 450)
                    scrollTarget = 450;
            }
        }

        // Click chuột
        if (auto mouseClick = event->getIf<Event::MouseButtonPressed>())
        {
            if (mouseClick->button == Mouse::Button::Left)
            {
                if (trangThai == 0)
                {
                    float dx = mouseNormal.x - Tam.x, dy = mouseNormal.y - Tam.y;
                    if (sqrt(dx * dx + dy * dy) <= radius && maudachon.size() < 3)
                    {
                        float angle = atan2(dy, dx);
                        if (angle < 0)
                            angle += 2 * 3.14159f;
                        int index = static_cast<int>(angle / (2 * 3.14159f / 6));
                        if (find(maudachon.begin(), maudachon.end(), Lucgiac[index].maugoc) == maudachon.end())
                            maudachon.push_back(Lucgiac[index].maugoc);
                    }
                    if (btnReset.hinh.getGlobalBounds().contains(mouseNormal))
                        maudachon.clear();
                    if (btnCont.hinh.getGlobalBounds().contains(mouseNormal) && maudachon.size() == 3)
                    {
                        khoiTaoHoanVi(maudachon, dsHoanVi, dsKhung, font);
                        trangThai = 1;
                        scrollTarget = 0;
                    }
                }
                // Thêm vào trong hàm xuLySuKien, phần xử lý click chuột
                else if (trangThai == 1) // Đang ở màn hình danh sách hoán vị
                {
                    // Kiểm tra click vào nút Back
                    if (btnBack.hinh.getGlobalBounds().contains(mouseNormal))
                    {
                        trangThai = 0;
                    }
                    // Kiểm tra click vào từng khung hoán vị
                    for (int i = 0; i < (int)dsKhung.size(); i++)
                    {
                        // Lưu ý: phải chuyển tọa độ chuột sang View của MH2 để check chính xác
                        Vector2f mouseInView = window.mapPixelToCoords(Mouse::getPosition(window), mainView);
                        if (dsKhung[i].hinh.getGlobalBounds().contains(mouseInView))
                        {
                            mauDuocChonTuMH2 = dsHoanVi[i]; // Lấy bộ màu tương ứng
                            dsBiHienThi.clear();            // Reset danh sách bi cũ
                            trangThai = 2;                  // Chuyển sang màn hình 3
                            break;
                        }
                    }
                }
                else if (trangThai == 2) // Màn hình 3
                {
                    if (btnBack.hinh.getGlobalBounds().contains(mouseNormal))
                    {
                        trangThai = 1;
                    }
                    else
                    {
                        // Kiểm tra click vào 3 viên bi mẫu ở trên đầu
                        // Tọa độ y của bi mẫu là 50, bán kính 30 (tâm ở 50,80 nếu tính từ vị trí vẽ)
                        // Để chính xác, ta tính khoảng cách từ chuột tới tâm bi mẫu
                        for (int i = 0; i < 3; i++)
                        {
                            Vector2f tamBiMau(450.f + i * 100.f + 30.f, 50.f + 30.f);
                            float dx = mouseNormal.x - tamBiMau.x;
                            float dy = mouseNormal.y - tamBiMau.y;

                            if (sqrt(dx * dx + dy * dy) <= 30.f) // Nếu click trúng bi mẫu thứ i
                            {
                                CircleShape newBi;
                                newBi.setFillColor(mauDuocChonTuMH2[i]);
                                dsBiHienThi.push_back(newBi);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void veManHinh1(RenderWindow &window, Vector2f mouse, Vector2f Tam, float radius, vector<Manhghep> &Lucgiac,
                vector<Color> &maudachon, ButtonUI &btnReset, ButtonUI &btnCont, Text &ThongBao)
{

    for (int i = 0; i < 6; i++)
    {
        float dx = mouse.x - Tam.x, dy = mouse.y - Tam.y;
        float angle = atan2(dy, dx);
        if (angle < 0)
            angle += 2 * 3.14159f;
        Lucgiac[i].scale_Muctieu = (sqrt(dx * dx + dy * dy) <= radius && angle >= i * 2 * 3.14159f / 6 && angle <= (i + 1) * 2 * 3.14159f / 6) ? 1.15f : 1.0f;
        Lucgiac[i].scale_hientai += (Lucgiac[i].scale_Muctieu - Lucgiac[i].scale_hientai) * 0.1f;
        Lucgiac[i].Manh.setScale({Lucgiac[i].scale_hientai, Lucgiac[i].scale_hientai});
        window.draw(Lucgiac[i].Manh);
    }
    btnReset.update(mouse);
    btnReset.draw(window);
    btnCont.update(mouse);
    btnCont.draw(window);
    for (int i = 0; i < (int)maudachon.size(); i++)
        window.draw(HCN_bo_goc({180, 80}, 15, maudachon[i], {900.f, 100.f + i * 120.f}));
    window.draw(ThongBao);
}
void veManHinh2(RenderWindow &window, View &mainView, float &scrollCurrent, float scrollTarget,
                vector<ButtonUI> &dsKhung, const vector<vector<Color>> &dsHoanVi, ButtonUI &btnBack, Text &tieuDe)
{

    // 1. Cập nhật vị trí cuộn mượt
    scrollCurrent += (scrollTarget - scrollCurrent) * 0.1f;
    mainView.setCenter({600.f, 400.f + scrollCurrent});

    // --- BƯỚC 2: VẼ NHỮNG THỨ SẼ BỊ CUỘN (TRÔI ĐI) ---
    window.setView(mainView);

    // Vẽ tiêu đề (Vì vẽ sau khi setView(mainView) nên nó sẽ trôi đi khi cuộn)
    window.draw(tieuDe);

    for (int i = 0; i < (int)dsKhung.size(); i++)
    {
        dsKhung[i].update(window.mapPixelToCoords(Mouse::getPosition(window), mainView));
        dsKhung[i].draw(window);
        for (int j = 0; j < 3; j++)
        {
            CircleShape bi(40);
            bi.setOrigin({40, 40});
            bi.setFillColor(dsHoanVi[i][j]);
            bi.setPosition({dsKhung[i].hinh.getPosition().x - 100 + j * 150, dsKhung[i].hinh.getPosition().y});
            window.draw(bi);
        }
    }

    // --- BƯỚC 3: VẼ NHỮNG THỨ ĐỨNG YÊN (CỐ ĐỊNH TRÊN MÀN HÌNH) ---
    window.setView(window.getDefaultView());

    // Nút Back bây giờ sẽ luôn nằm cố định ở góc màn hình dù bạn có cuộn đi đâu
    btnBack.update(window.mapPixelToCoords(Mouse::getPosition(window), window.getDefaultView()));
    btnBack.draw(window);
}
void veManHinh3(RenderWindow &window, ButtonUI &btnBack, const vector<Color> &mauMau,
                vector<CircleShape> &dsBi, Text huongDan)
{
    window.setView(window.getDefaultView());

    // 1. Vẽ 3 viên bi nguồn (Nút bấm)
    for (int i = 0; i < 3; i++)
    {
        CircleShape biMau(40);
        biMau.setFillColor(mauMau[i]);
        biMau.setOutlineThickness(2);
        biMau.setOutlineColor(Color::White);
        biMau.setPosition({450.f + i * 100.f, 50.f});
        window.draw(biMau);
    }

    // 2. Vẽ hướng dẫn (Không đếm số lượng nữa)
    huongDan.setString("Bam vao cac vien bi phia tren de tao bi tuong ung");
    huongDan.setCharacterSize(22);
    huongDan.setPosition({380, 130});
    window.draw(huongDan);

    // 3. Tính toán vị trí và kích thước cho hàng bi ở giữa
    if (!dsBi.empty())
    {
        float Chieu_rong_man_hinh = 1200.f;
        float screenCenterY = 400.f; // Giữa màn hình theo chiều dọc

        // Tính toán bán kính linh hoạt:
        // Giới hạn tối đa là 50, và nhỏ dần nếu quá nhiều bi để không tràn màn hình
        float Chieu_rong_toi_da_cho_phep = 1100.f; // Chiều rộng tối đa mà hàng bi được chiếm
        float r = 50.f;
        if (dsBi.size() * (r * 2.2f) > Chieu_rong_toi_da_cho_phep)
        {
            r = Chieu_rong_toi_da_cho_phep / (dsBi.size() * 2.2f);
        }
        if (r < 5.f)
            r = 5.f; // Không để bi quá bé

        // Tính tổng chiều rộng của hàng bi để căn lề giữa
        float Khoang_cach_cac_bi = r * 0.2f; // Khoảng cách giữa các bi
        float tong_chieu_rong_hang_bi = dsBi.size() * (2 * r) + (dsBi.size() - 1) * Khoang_cach_cac_bi;
        float startX = (Chieu_rong_man_hinh - tong_chieu_rong_hang_bi) / 2.f;

        for (int i = 0; i < (int)dsBi.size(); i++)
        {
            dsBi[i].setRadius(r);
            dsBi[i].setOrigin({r, r}); // Đặt tâm ở giữa bi
            dsBi[i].setPosition({startX + r + i * (2 * r + Khoang_cach_cac_bi), screenCenterY});
            window.draw(dsBi[i]);
        }
    }

    // 4. Nút Back
    btnBack.update(window.mapPixelToCoords(Mouse::getPosition(window)));
    btnBack.draw(window);
}
// --- MAIN ---
int main()
{
    RenderWindow Window(VideoMode({1200, 800}), "PBL - Hoan Vi Project");
    Window.setFramerateLimit(60);

    Font font;
    if (!font.openFromFile("ARIAL.TTF"))
        return -1;

    // Khai báo tài nguyên
    View mainView = Window.getDefaultView();
    float scrollCurrent = 0, scrollTarget = 0;
    int trangThai = 0;
    Vector2f Tam({400, 400});
    float radius = 250;
    vector<Color> maudachon;
    vector<vector<Color>> dsHoanVi;
    vector<ButtonUI> dsKhung;

    // Khởi tạo Lục giác
    vector<Color> dsmau = {Color::Red, Color::Green, Color::Blue, Color::Yellow, Color::Magenta, Color::Cyan};
    vector<Manhghep> Lucgiac;
    for (int i = 0; i < 6; i++)
    {
        Manhghep tmp;
        tmp.maugoc = dsmau[i];
        tmp.Manh = Tam_giac(Tam, radius, i * 2 * 3.14159f / 6, (i + 1) * 2 * 3.14159f / 6, dsmau[i]);
        Lucgiac.push_back(tmp);
    }

    // Khởi tạo UI
    ButtonUI btnReset(font, "Reset", 20), btnCont(font, "Continue", 20), btnBack(font, "Back", 20);
    btnReset.hinh = HCN_bo_goc({150, 50}, 10, Color(150, 0, 0), {800, 650});
    btnReset.chu.setPosition(btnReset.hinh.getPosition() - Vector2f(30, 15));
    btnCont.hinh = HCN_bo_goc({150, 50}, 10, Color(0, 150, 0), {1000, 650});
    btnCont.chu.setPosition(btnCont.hinh.getPosition() - Vector2f(45, 15));
    btnBack.hinh = HCN_bo_goc({120, 45}, 10, Color(100, 100, 100), {30, 30});
    btnBack.chu.setPosition(btnBack.hinh.getPosition() - Vector2f(25, 15));

    Text ThongBao(font, "Hay chon 3 mau de tiep tuc", 30);
    ThongBao.setPosition({50, 50});
    Text tieuDe(font, "6 CACH HOAN VI CUA 3 MAU", 35);
    tieuDe.setPosition({320, 50});

    // Vòng lặp chính
    while (Window.isOpen())
    {
        xuLySuKien(Window, trangThai, maudachon, Lucgiac, btnReset, btnCont, btnBack, dsHoanVi, dsKhung, font, scrollTarget, Tam, radius, mainView);

        Window.clear(Color(30, 30, 30));

        if (trangThai == 0)
        {
            Window.setView(Window.getDefaultView());
            veManHinh1(Window, Window.mapPixelToCoords(Mouse::getPosition(Window)), Tam, radius, Lucgiac, maudachon, btnReset, btnCont, ThongBao);
            
        }
        else if (trangThai == 1)
        {
            veManHinh2(Window, mainView, scrollCurrent, scrollTarget, dsKhung, dsHoanVi, btnBack, tieuDe);
        }
        else if (trangThai == 2)
        {
            // ThongBao lúc này đóng vai trò là text hướng dẫn ở MH3
            veManHinh3(Window, btnBack, mauDuocChonTuMH2, dsBiHienThi, ThongBao);
        }

        Window.display();
    }
    return 0;
}