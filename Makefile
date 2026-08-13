# 1. Khai báo các biến cấu hình
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O3 -mwindows
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
TARGET = game.exe
# 2. Tự động quét tất cả các file .cpp trong thư mục src
SRCS = $(wildcard src/*.cpp)
# Tự động tạo tên các file .o tương ứng
OBJS = $(SRCS:.cpp=.o)

# 3. Lệnh build chính (Gom các file .o thành file .exe)
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# 4. Lệnh dịch từng file .cpp thành .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 5. Lệnh dọn dẹp file rác
clean:
	-del /Q src\*.o $(TARGET)