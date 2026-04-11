CXX = g++

CXXFLAGS = -I"C:\SFML-3.0.2-windows-gcc-14.2.0-mingw-64-bit\SFML-3.0.2\include" -Iimgui -Iimgui-sfml -DSFML_STATIC
LDFLAGS = -L"C:\SFML-3.0.2-windows-gcc-14.2.0-mingw-64-bit\SFML-3.0.2\lib"

LIBS = -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-main -lopengl32 -lfreetype -lwinmm -lgdi32

# Thay main.cpp bằng SapXepVienBi.cpp ở đây
SRC = SapXepVienBi_v2.cpp \
imgui/imgui.cpp \
imgui/imgui_draw.cpp \
imgui/imgui_widgets.cpp \
imgui/imgui_tables.cpp \
imgui-sfml/imgui-SFML.cpp

# Bạn có thể đổi tên file đầu ra từ main thành SapXepVienBi cho đồng bộ
all:
	$(CXX) $(SRC) -o SapXepVienBi_v2 $(CXXFLAGS) $(LDFLAGS) $(LIBS)

clean:
	del /f SapXepVienBi_v2.exe