#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>
#include <sstream>

using namespace sf;
using namespace std;

// ==================== CẤU TRÚC DỮ LIỆU ====================

// Hình chữ nhật bo góc tự tạo
struct HinhChuNhatBoGoc : public ConvexShape {
    HinhChuNhatBoGoc() {}
    
    HinhChuNhatBoGoc(Vector2f kichThuoc, float banKinhGoc) {
        taoHinh(kichThuoc, banKinhGoc);
    }

    void taoHinh(Vector2f kichThuoc, float banKinhGoc) {
        const int diemMoiGoc = 10;
        setPointCount(diemMoiGoc * 4);
        
        float w = kichThuoc.x;
        float h = kichThuoc.y;
        float r = banKinhGoc;
        const float PI = 3.141592654f;
        
        // 4 góc bo tròn
        for (int goc = 0; goc < 4; goc++) {
            for (int i = 0; i < diemMoiGoc; i++) {
                float gocRad = (goc * 90.f + i * (90.f / diemMoiGoc)) * PI / 180.f;
                Vector2f tam;
                if (goc == 0) tam = {w - r, h - r};
                else if (goc == 1) tam = {r, h - r};
                else if (goc == 2) tam = {r, r};
                else tam = {w - r, r};
                
                setPoint(goc * diemMoiGoc + i, 
                    Vector2f(tam.x + r * cos(gocRad), tam.y + r * sin(gocRad)));
            }
        }
    }
};

struct HinhLucGiac {
    ConvexShape shape;
    Color mau;
    float scaleHienTai = 1.0f;
    float scaleMucTieu = 1.0f;
    int index;
};

struct VienBi {
    CircleShape shape;
    Color mau;
    float scaleHienTai = 1.0f;
    float scaleMucTieu = 1.0f;
    Vector2f viTriGoc;
    Vector2f viTriMucTieu;
    bool daXep = false;
};

struct NutBam {
    HinhChuNhatBoGoc hinh;
    Text* chu = nullptr;
    float scaleHienTai = 1.0f;
    float scaleMucTieu = 1.0f;
    Color mauNen = Color(70, 130, 180);
    Color mauChu = Color::White;

    ~NutBam() { delete chu; }
    NutBam() {}
    NutBam(const NutBam& other) : hinh(other.hinh), chu(other.chu ? new Text(*other.chu) : nullptr),
        scaleHienTai(other.scaleHienTai), scaleMucTieu(other.scaleMucTieu),
        mauNen(other.mauNen), mauChu(other.mauChu) {}

    NutBam(const Font& font, string noiDung, Vector2f kichThuoc, Vector2f viTri) {
        hinh.taoHinh(kichThuoc, 10.f);
        hinh.setPosition(viTri);
        hinh.setFillColor(mauNen);

        chu = new Text(font, noiDung, 24);
        chu->setFillColor(mauChu);
        chu->setStyle(Text::Bold);

        FloatRect bounds = chu->getLocalBounds();
        Vector2f origin = bounds.position + bounds.size / 2.f;
        chu->setOrigin(origin);
        chu->setPosition(viTri + kichThuoc / 2.f);
    }

    void capNhat(Vector2f mousePos) {
        scaleMucTieu = hinh.getGlobalBounds().contains(mousePos) ? 1.08f : 1.0f;
        scaleHienTai += (scaleMucTieu - scaleHienTai) * 0.2f;
        hinh.setScale({scaleHienTai, scaleHienTai});
    }

    bool contains(Vector2f pos) const {
        return hinh.getGlobalBounds().contains(pos);
    }

    void ve(RenderWindow& window) {
        window.draw(hinh);
        if (chu) window.draw(*chu);
    }
};

struct KhungMau {
    HinhChuNhatBoGoc hinh;
    Color mau;
    float scaleHienTai = 1.0f;
    float scaleMucTieu = 1.0f;
    int viTriIndex;

    KhungMau() {}

    KhungMau(Color c, Vector2f viTri, int index) : mau(c), viTriIndex(index) {
        hinh.taoHinh({100.f, 100.f}, 15.f);
        hinh.setPosition(viTri);
        hinh.setFillColor(c);
    }

    void capNhat(Vector2f mousePos) {
        scaleMucTieu = hinh.getGlobalBounds().contains(mousePos) ? 1.1f : 1.0f;
        scaleHienTai += (scaleMucTieu - scaleHienTai) * 0.2f;
        hinh.setScale({scaleHienTai, scaleHienTai});
    }

    bool contains(Vector2f pos) const {
        return hinh.getGlobalBounds().contains(pos);
    }

    void ve(RenderWindow& window) {
        window.draw(hinh);
    }
};

// ==================== HẰNG SỐ ====================

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const float PI = 3.141592654f;

const Color MAU_LUC_GIAC[6] = {
    Color(231, 76, 60),
    Color(241, 196, 15),
    Color(46, 204, 113),
    Color(52, 152, 219),
    Color(155, 89, 182),
    Color(230, 126, 34)
};

const string TEN_MAU[6] = {"Đỏ", "Vàng", "Xanh Lá", "Xanh Dương", "Tím", "Cam"};

// ==================== BIẾN ====================

// Prototype cho hàm khởi tạo lục giác
void khoiTaoHinhLucGiac();

int manHinh = 1;
vector<int> mauChon;
vector<KhungMau> khungMauChon;
vector<VienBi> danhSachVienBi;
bool daBatDau = false;
int soLuongToiDa = 0;
bool dangKeo = false;
int viTriKeo = -1;
Vector2f viTriMouseBanDau;

// ==================== HÀM ====================

ConvexShape veLucGiac(Vector2f tam, float banKinh, Color mau) {
    ConvexShape hinh(6);
    for (int i = 0; i < 6; i++) {
        float goc = (i * 60.f - 90.f) * PI / 180.f;
        float x = tam.x + banKinh * cos(goc);
        float y = tam.y + banKinh * sin(goc);
        hinh.setPoint(i, Vector2f(x, y));
    }
    hinh.setFillColor(mau);
    return hinh;
}

CircleShape veTron(float banKinh, Color mau, float doDayVien, Color mauVien) {
    CircleShape hinh(banKinh);
    hinh.setFillColor(mau);
    hinh.setOutlineThickness(doDayVien);
    hinh.setOutlineColor(mauVien);
    return hinh;
}

void taoVienBi(Color mau, Vector2f viTri) {
    VienBi vb;
    vb.mau = mau;
    vb.viTriGoc = viTri;
    vb.viTriMucTieu = viTri;
    vb.shape = veTron(30.f, mau, 2.f, Color::White);
    vb.shape.setPosition(viTri);
    danhSachVienBi.push_back(vb);
}

void tinhSoLuongToiDa() {
    float chieuRongKhuVuc = WINDOW_WIDTH - 350.f;
    float chieuCaoKhuVuc = WINDOW_HEIGHT - 150.f;
    float banKinhBi = 30.f;
    float khoangCach = 5.f;
    float tongKichThuoc = (banKinhBi * 2 + khoangCach);
    soLuongToiDa = static_cast<int>((chieuRongKhuVuc / tongKichThuoc) * (chieuCaoKhuVuc / tongKichThuoc));
    soLuongToiDa = min(soLuongToiDa, 50);
}

void sapXepVienBi() {
    int n = danhSachVienBi.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int mau1 = -1, mau2 = -1;
            for (int m = 0; m < 6; m++) {
                if (MAU_LUC_GIAC[m] == danhSachVienBi[j].mau) mau1 = m;
                if (MAU_LUC_GIAC[m] == danhSachVienBi[j + 1].mau) mau2 = m;
            }
            if (mau1 > mau2) {
                swap(danhSachVienBi[j], danhSachVienBi[j + 1]);
            }
        }
    }
}

void xaoTronVienBi() {
    random_device rd;
    mt19937 g(rd());
    shuffle(danhSachVienBi.begin(), danhSachVienBi.end(), g);
}

void taoHinhLucGiac(Vector2f viTri, float banKinh, Color mau) {
    HinhLucGiac hlg;
    hlg.shape = veLucGiac(viTri, banKinh, mau);
    hlg.mau = mau;
    hlg.index = khungMauChon.size();
    khungMauChon.push_back(KhungMau(mau, viTri, hlg.index));
}

void veTatCaHinhLucGiac(RenderWindow& window) {
    for (auto& khung : khungMauChon) {
        khung.ve(window);
    }
}

void capNhatHinhLucGiac(Vector2f mousePos) {
    for (auto& khung : khungMauChon) {
        khung.capNhat(mousePos);
    }
}

// ==================== MAIN ====================

int main() {
    RenderWindow window(VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), L"Sắp xếp viên bi - SFML 3.0.2", Style::Default, State::Windowed);
    window.setFramerateLimit(60);

    Font font;
    if (!font.openFromFile("C:\\WINDOWS\\Fonts\\arial.ttf")) {
        if (!font.openFromFile("C:\\Windows\\Fonts\\times.ttf")) {
            cerr << "Không load được font!" << endl;
            return -1;
        }
    }

    tinhSoLuongToiDa();

    // 6 hình lục giác màu
    vector<HinhLucGiac> dsLucGiac;
    Vector2f tamLucGiac(250.f, 350.f);
    float banKinhLG = 120.f;

    for (int i = 0; i < 6; i++) {
        HinhLucGiac lg;
        lg.index = i;
        lg.mau = MAU_LUC_GIAC[i];
        lg.shape = veLucGiac(tamLucGiac, banKinhLG, MAU_LUC_GIAC[i]);
        lg.shape.setOutlineThickness(0.f);
        dsLucGiac.push_back(lg);
    }

    // Nút bấm
    NutBam nutBatDau(font, "Bắt Đầu", Vector2f{180.f, 60.f}, Vector2f{1050.f, 620.f});
    nutBatDau.mauNen = Color(39, 174, 96);

    NutBam nutReset(font, "Reset", Vector2f{150.f, 50.f}, Vector2f{1050.f, 550.f});
    nutReset.mauNen = Color(192, 57, 43);

    NutBam nutQuayLai(font, " Quay Lại ", Vector2f{150.f, 50.f}, Vector2f{50.f, 50.f});
    nutQuayLai.mauNen = Color(127, 140, 141);

    // Text tiêu đề
    Text tieuDe(font, "CHỌN MÀU VIÊN BI", 32);
    tieuDe.setFillColor(Color::White);
    tieuDe.setStyle(Text::Bold);
    tieuDe.setPosition({50.f, 100.f});

    Text tieuDe2(font, "TẠO VIÊN BI", 32);
    tieuDe2.setFillColor(Color::White);
    tieuDe2.setStyle(Text::Bold);
    tieuDe2.setPosition({50.f, 50.f});

    Text tieuDe3(font, "SẮP XẾP VIÊN BI", 32);
    tieuDe3.setFillColor(Color::White);
    tieuDe3.setStyle(Text::Bold);
    tieuDe3.setPosition({50.f, 50.f});

    Text thongTin(font, "", 20);
    thongTin.setFillColor(Color(200, 200, 200));

    // Tạo các hình lục giác với các màu sắc khác nhau
    khoiTaoHinhLucGiac();

    while (window.isOpen()) {
        while (const optional<Event> event = window.pollEvent()) {
            if (event->getIf<Event::Closed>()) {
                window.close();
            }

            if (auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                    if (manHinh == 1) {
                        for (auto& lg : dsLucGiac) {
                            if (lg.shape.getGlobalBounds().contains(mousePos)) {
                                if (mauChon.size() < 3) {
                                    bool daCo = false;
                                    for (int m : mauChon) {
                                        if (m == lg.index) daCo = true;
                                    }
                                    if (!daCo) {
                                        mauChon.push_back(lg.index);
                                        Vector2f viTriKhung(350.f + mauChon.size() * 130.f, 280.f);
                                        khungMauChon.emplace_back(MAU_LUC_GIAC[lg.index], viTriKhung, mauChon.size() - 1);
                                    }
                                }
                            }
                        }

                        for (int i = 0; i < khungMauChon.size(); i++) {
                            if (khungMauChon[i].contains(mousePos)) {
                                dangKeo = true;
                                viTriKeo = i;
                                viTriMouseBanDau = mousePos;
                            }
                        }
                    }
                    else if (manHinh == 2) {
                        for (auto& km : khungMauChon) {
                            if (km.contains(mousePos)) {
                                if (danhSachVienBi.size() < soLuongToiDa) {
                                    float x = 350.f + rand() % (WINDOW_WIDTH - 400);
                                    float y = 150.f + rand() % (WINDOW_HEIGHT - 250);
                                    taoVienBi(km.mau, Vector2f(x, y));
                                }
                            }
                        }
                    }
                }
            }

            if (auto* mouseReleased = event->getIf<Event::MouseButtonReleased>()) {
                if (mouseReleased->button == Mouse::Button::Left) {
                    if (dangKeo && manHinh == 1) {
                        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                        for (int i = 0; i < khungMauChon.size(); i++) {
                            if (i != viTriKeo && khungMauChon[i].contains(mousePos)) {
                                swap(mauChon[viTriKeo], mauChon[i]);
                                khungMauChon.clear();
                                for (int j = 0; j < mauChon.size(); j++) {
                                    Vector2f viTriKhung(350.f + j * 130.f, 280.f);
                                    khungMauChon.emplace_back(MAU_LUC_GIAC[mauChon[j]], viTriKhung, j);
                                }
                                break;
                            }
                        }
                        dangKeo = false;
                        viTriKeo = -1;
                    }
                }
            }
        }

        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

        window.clear(Color(30, 30, 50));

        if (manHinh == 1) {
            window.draw(tieuDe);

            Text huongDan(font, "Chọn 3 màu bằng cách click vào hình lục giác", 18);
            huongDan.setFillColor(Color(180, 180, 180));
            huongDan.setPosition({50.f, 140.f});
            window.draw(huongDan);

            for (auto& lg : dsLucGiac) {
                if (lg.shape.getGlobalBounds().contains(mousePos)) {
                    lg.scaleMucTieu = 1.1f;
                    lg.shape.setOutlineThickness(4.f);
                    lg.shape.setOutlineColor(Color::White);
                } else {
                    lg.scaleMucTieu = 1.0f;
                    bool daChon = false;
                    for (int m : mauChon) {
                        if (m == lg.index) daChon = true;
                    }
                    lg.shape.setOutlineThickness(daChon ? 4.f : 0.f);
                    lg.shape.setOutlineColor(daChon ? Color::White : Color::Transparent);
                }

                lg.scaleHienTai += (lg.scaleMucTieu - lg.scaleHienTai) * 0.15f;
                lg.shape.setScale({lg.scaleHienTai, lg.scaleHienTai});

                window.draw(lg.shape);

                Text tenMauText(font, TEN_MAU[lg.index], 16);
                tenMauText.setFillColor(Color::White);
                FloatRect bounds = tenMauText.getLocalBounds();
                Vector2f origin = bounds.position + bounds.size / 2.f;
                tenMauText.setOrigin(origin);
                float goc = (lg.index * 60.f - 90.f) * PI / 180.f;
                tenMauText.setPosition(tamLucGiac + Vector2f((banKinhLG + 30.f) * cos(goc), (banKinhLG + 30.f) * sin(goc)));
                window.draw(tenMauText);
            }

            for (auto& km : khungMauChon) {
                km.capNhat(mousePos);
                if (km.contains(mousePos)) {
                    km.hinh.setOutlineThickness(3.f);
                    km.hinh.setOutlineColor(Color::White);
                } else {
                    km.hinh.setOutlineThickness(0.f);
                }
                km.ve(window);
            }

            if (mauChon.size() == 3) {
                nutBatDau.capNhat(mousePos);
                nutBatDau.ve(window);

                if (nutBatDau.contains(mousePos) && Mouse::isButtonPressed(Mouse::Button::Left)) {
                    manHinh = 2;
                }
            }

            stringstream ss;
            ss << "Đã chọn: " << mauChon.size() << "/3 màu";
            Text soMau(font, ss.str(), 22);
            soMau.setFillColor(Color::White);
            soMau.setPosition({350.f, 240.f});
            window.draw(soMau);
        }
        else if (manHinh == 2) {
            window.draw(tieuDe2);

            Text huongDan(font, "Click vào màu bên trái để tạo viên bi - Kéo thả để hoán đổi màu", 18);
            huongDan.setFillColor(Color(180, 180, 180));
            huongDan.setPosition({50.f, 90.f});
            window.draw(huongDan);

            for (int i = 0; i < khungMauChon.size(); i++) {
                auto& km = khungMauChon[i];
                km.capNhat(mousePos);

                if (km.contains(mousePos)) {
                    km.hinh.setOutlineThickness(4.f);
                    km.hinh.setOutlineColor(Color::White);

                    if (Mouse::isButtonPressed(Mouse::Button::Left)) {
                        if (danhSachVienBi.size() < soLuongToiDa) {
                            float x = 350.f + rand() % (WINDOW_WIDTH - 400);
                            float y = 150.f + rand() % (WINDOW_HEIGHT - 250);
                            taoVienBi(km.mau, Vector2f(x, y));
                        }
                    }
                } else {
                    km.hinh.setOutlineThickness(0.f);
                }
                km.ve(window);
            }

            for (auto& vb : danhSachVienBi) {
                if (vb.shape.getGlobalBounds().contains(mousePos)) {
                    vb.scaleMucTieu = 1.15f;
                    vb.shape.setOutlineThickness(3.f);
                    vb.shape.setOutlineColor(Color::White);
                } else {
                    vb.scaleMucTieu = 1.0f;
                    vb.shape.setOutlineThickness(2.f);
                    vb.shape.setOutlineColor(Color(100, 100, 100));
                }

                vb.scaleHienTai += (vb.scaleMucTieu - vb.scaleHienTai) * 0.2f;
                vb.shape.setScale({vb.scaleHienTai, vb.scaleHienTai});

                window.draw(vb.shape);
            }

            if (!danhSachVienBi.empty()) {
                nutBatDau.capNhat(mousePos);
                nutBatDau.ve(window);

                if (nutBatDau.contains(mousePos) && Mouse::isButtonPressed(Mouse::Button::Left)) {
                    xaoTronVienBi();
                    manHinh = 3;
                    daBatDau = true;
                }
            }

            nutQuayLai.capNhat(mousePos);
            nutQuayLai.ve(window);

            if (nutQuayLai.contains(mousePos) && Mouse::isButtonPressed(Mouse::Button::Left)) {
                manHinh = 1;
            }

            stringstream ss2;
            ss2 << "Số viên bi: " << danhSachVienBi.size() << "/" << soLuongToiDa;
            thongTin.setString(ss2.str());
            thongTin.setPosition({350.f, 120.f});
            window.draw(thongTin);
        }
        else if (manHinh == 3) {
            window.draw(tieuDe3);

            Text huongDan3(font, "Các viên bi đang được sắp xếp theo màu", 18);
            huongDan3.setFillColor(Color(180, 180, 180));
            huongDan3.setPosition({50.f, 90.f});
            window.draw(huongDan3);

            for (auto& km : khungMauChon) {
                window.draw(km.hinh);
            }

            if (daBatDau) {
                sapXepVienBi();
                daBatDau = false;
            }

            float khoangCach = 70.f;
            float xBatDau = 350.f;
            float yBatDau = 150.f;
            int soCot = (WINDOW_WIDTH - 380) / khoangCach;

            for (size_t i = 0; i < danhSachVienBi.size(); i++) {
                int cot = i % soCot;
                int hang = i / soCot;

                float x = xBatDau + cot * khoangCach;
                float y = yBatDau + hang * khoangCach;

                danhSachVienBi[i].shape.setPosition({x, y});

                if (danhSachVienBi[i].shape.getGlobalBounds().contains(mousePos)) {
                    danhSachVienBi[i].scaleMucTieu = 1.2f;
                    danhSachVienBi[i].shape.setOutlineThickness(4.f);
                    danhSachVienBi[i].shape.setOutlineColor(Color::White);
                } else {
                    danhSachVienBi[i].scaleMucTieu = 1.0f;
                    danhSachVienBi[i].shape.setOutlineThickness(2.f);
                    danhSachVienBi[i].shape.setOutlineColor(Color(80, 80, 80));
                }

                danhSachVienBi[i].scaleHienTai += (danhSachVienBi[i].scaleMucTieu - danhSachVienBi[i].scaleHienTai) * 0.2f;
                danhSachVienBi[i].shape.setScale({danhSachVienBi[i].scaleHienTai, danhSachVienBi[i].scaleHienTai});

                window.draw(danhSachVienBi[i].shape);
            }

            nutQuayLai.capNhat(mousePos);
            nutQuayLai.ve(window);

            if (nutQuayLai.contains(mousePos) && Mouse::isButtonPressed(Mouse::Button::Left)) {
                manHinh = 2;
                danhSachVienBi.clear();
            }

            nutReset.capNhat(mousePos);
            nutReset.ve(window);

            if (nutReset.contains(mousePos) && Mouse::isButtonPressed(Mouse::Button::Left)) {
                danhSachVienBi.clear();
                manHinh = 1;
                mauChon.clear();
                khungMauChon.clear();
            }

            stringstream ss3;
            ss3 << "Tổng số viên bi: " << danhSachVienBi.size();
            thongTin.setString(ss3.str());
            thongTin.setPosition({350.f, 120.f});
            window.draw(thongTin);
        }

        window.display();
    }

    return 0;
}