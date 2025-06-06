# 컴파일러와 옵션 설정
CC = gcc
CFLAGS = -Iinclude -Wall -Wextra -g

# 소스와 헤더 포함
SRCDIR = src
INCDIR = include
BINDIR = bin

# 소스 파일들
SRCS = $(wildcard $(SRCDIR)/*.c)

# 오브젝트 파일들 (.c → .o)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(SRCDIR)/%.o)

# 최종 실행 파일 이름
TARGET = $(BINDIR)/mini_os

# 기본 빌드 명령어
all: $(TARGET)

# 링크하여 실행 파일 생성
$(TARGET): $(OBJS)
	mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# 개별 .o 파일 빌드 규칙
$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 정리 명령
clean:
	rm -f $(SRCDIR)/*.o $(TARGET)

# 실행
run: all
	./$(TARGET)

