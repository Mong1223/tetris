SHELL = /bin/sh

CC       = gcc
CFLAGS   = -Wall -Wextra -Werror -std=c11

LDLIBS   = -lncurses

GCOV_FLAGS = -fprofile-arcs -ftest-coverage
GCOV       = gcov

# -----------------------------------------
#         ОС-ЗАВИСИМЫЕ НАСТРОЙКИ
# -----------------------------------------
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    # macOS
    EXTRA_INCLUDE = -I/opt/homebrew/include
    EXTRA_LIBS    = -L/opt/homebrew/lib
    CHECK_LIBS    = -lcheck
else
    # Linux
    EXTRA_INCLUDE =
    EXTRA_LIBS    =
    CHECK_LIBS    = -lcheck -lsubunit -lrt
endif

TEST_LIBS = $(CHECK_LIBS) -lm -lpthread


# -----------------------------------------
#             ДИРЕКТОРИИ
# -----------------------------------------
PREFIX  = build
OBJDIR  = $(PREFIX)/obj
BINDIR  = $(PREFIX)/bin
LIBDIR  = $(PREFIX)/lib
INCDIR  = $(PREFIX)/include

INSTALL_DIR = install_dir

TARGET  = brickgame
LIBNAME = libtetris.a


# Источники
BACKEND_SRC = \
	brick_game/tetris/backend.c \
	brick_game/tetris/figures.c

GUI_SRC = \
	gui/cli/main.c \
	gui/cli/frontend.c

TEST_SRC = tests/test_tetris.c


# Объектные файлы
BACKEND_OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(BACKEND_SRC))
GUI_OBJ     = $(patsubst %.c,$(OBJDIR)/%.o,$(GUI_SRC))
TEST_OBJ    = $(patsubst tests/%.c,$(OBJDIR)/tests/%.o,$(TEST_SRC))

ALL_OBJ = $(BACKEND_OBJ) $(GUI_OBJ)


# -----------------------------------------
#              ОСНОВНЫЕ ЦЕЛИ
# -----------------------------------------
.PHONY: all clean test gcov_report dvi dist install uninstall

all: $(BINDIR)/$(TARGET)


# Создание структуры каталогов
$(OBJDIR):
	mkdir -p $(OBJDIR)/brick_game/tetris
	mkdir -p $(OBJDIR)/gui/cli
	mkdir -p $(OBJDIR)/tests

$(BINDIR):
	mkdir -p $(BINDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

$(INCDIR):
	mkdir -p $(INCDIR)/brick_game/tetris


# -----------------------------------------
#        СБОРКА СТАТИЧЕСКОЙ БИБЛИОТЕКИ
# -----------------------------------------
$(LIBDIR)/$(LIBNAME): $(OBJDIR) $(BACKEND_OBJ) | $(LIBDIR)
	ar rcs $@ $(BACKEND_OBJ)


# -----------------------------------------
#        ФИНАЛЬНЫЙ БИНАРНИК
# -----------------------------------------
$(BINDIR)/$(TARGET): $(OBJDIR) $(GUI_OBJ) $(LIBDIR)/$(LIBNAME) | $(BINDIR)
	$(CC) $(CFLAGS) $(EXTRA_LIBS) $(GUI_OBJ) $(LIBDIR)/$(LIBNAME) $(LDLIBS) -o $@


# -----------------------------------------
#         ПРАВИЛА КОМПИЛЯЦИИ .o
# -----------------------------------------
$(OBJDIR)/brick_game/tetris/%.o: brick_game/tetris/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(EXTRA_INCLUDE) -c $< -o $@

$(OBJDIR)/gui/cli/%.o: gui/cli/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(EXTRA_INCLUDE) -c $< -o $@

$(OBJDIR)/tests/%.o: tests/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(EXTRA_INCLUDE) -c $< -o $@


# -----------------------------------------
#                 INSTALL
# -----------------------------------------
install: all
	mkdir -p $(INSTALL_DIR)/bin
	mkdir -p $(INSTALL_DIR)/lib
	mkdir -p $(INSTALL_DIR)/include/brick_game/tetris
	cp $(BINDIR)/$(TARGET) $(INSTALL_DIR)/bin/
	cp $(LIBDIR)/$(LIBNAME) $(INSTALL_DIR)/lib/
	cp brick_game/tetris/*.h $(INSTALL_DIR)/include/brick_game/tetris/
	@echo "Установлено в $(INSTALL_DIR)"

uninstall:
	rm -rf $(INSTALL_DIR)
	@echo "Удалено из $(INSTALL_DIR)"


# -----------------------------------------
#                ТЕСТЫ
# -----------------------------------------
test: $(OBJDIR) $(LIBDIR)/$(LIBNAME) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(EXTRA_LIBS) $(TEST_OBJ) $(LIBDIR)/$(LIBNAME) $(TEST_LIBS) -o test_tetris
	./test_tetris


# -----------------------------------------
#           GCOV + LCOV REPORT
# -----------------------------------------
gcov_report: clean
	mkdir -p build/gcov/obj
	mkdir -p build/gcov/bin

	# backend c покрытием
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(EXTRA_INCLUDE) -c brick_game/tetris/backend.c  -o build/gcov/obj/backend.o
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(EXTRA_INCLUDE) -c brick_game/tetris/figures.c -o build/gcov/obj/figures.o

	# тесты
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(EXTRA_INCLUDE) -c tests/test_tetris.c -o build/gcov/obj/test_tetris.o

	# линковка
	$(CC) $(GCOV_FLAGS) \
		build/gcov/obj/backend.o build/gcov/obj/figures.o \
		build/gcov/obj/test_tetris.o \
		$(TEST_LIBS) $(EXTRA_LIBS) \
		-o build/gcov/bin/tests_gcov

	cd build/gcov/bin && ./tests_gcov || true

	lcov --capture --directory build/gcov/obj --output-file build/gcov/coverage.info
	lcov --remove build/gcov/coverage.info 'tests/*' --output-file build/gcov/coverage.info

	genhtml build/gcov/coverage.info --output-directory gcov_report

	@echo "HTML отчёт готов: gcov_report/index.html"


# -----------------------------------------
#       DOCUMENTATION (DVI / LATEX)
# -----------------------------------------
dvi:
	mkdir -p dvi
	xelatex -output-directory=dvi dvi/documentation.tex
	xelatex -output-directory=dvi dvi/documentation.tex


# -----------------------------------------
#                DIST
# -----------------------------------------
dist:
	tar czf brickgame_tetris_dist.tar.gz \
		Makefile \
		brick_game \
		gui \
		tests \
		fsm_diagram.png
	@echo "Создан архив brickgame_tetris_dist.tar.gz"


run:
	./build/bin/brickgame

run_installed:
	./install_dir/bin/brickgame


# -----------------------------------------
#                CLEAN
# -----------------------------------------
clean:
	rm -rf $(PREFIX)
	rm -rf test_tetris
	rm -rf gcov_report
	rm -rf build/gcov
	rm -rf brickgame_tetris_dist.tar.gz
	rm -rf $(INSTALL_DIR)
	rm -f *.gcno *.gcda *.gcov
	rm -f */*.aux */*.log */*.out */*.toc */*.pdf
	rm -f *.dat
	@echo "Очистка завершена"


# -----------------------------------------
#       CODE STYLE / STATIC ANALYSIS
# -----------------------------------------
clang_check:
	@cp ../materials/linters/.clang-format .clang-format
	@clang-format -n brick_game/tetris/*.c gui/cli/*.c
	@clang-format -n brick_game/tetris/*.h gui/cli/*.h
	@rm .clang-format

clang:
	@cp ../materials/linters/.clang-format .clang-format
	@clang-format -i brick_game/tetris/*.c gui/cli/*.c
	@clang-format -i brick_game/tetris/*.h gui/cli/*.h
	@rm .clang-format

cppcheck:
	cppcheck --enable=all --inconclusive --std=c11 \
	--suppress=missingIncludeSystem \
	-I brick_game/tetris \
	-I gui/cli \
	brick_game/tetris/*.c \
	gui/cli/*.c \
	tests/*.c

memcheck: build/bin/brickgame
	valgrind --leak-check=full --show-leак-kinds=all \
		--track-origins=yes ./build/bin/brickgame


#brew install --cask mactex