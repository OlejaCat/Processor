SRC_DIRS := ../Stack ../MyMiniLib ../command_processing
BUILD_DIR := ../build_processor

CC := gcc
CFLAGS := -Wall -Wextra -Og

ifeq ($(CC), clang)
CFLAGS += -Wconversion -Wdangling -Wdeprecated -Wdocumentation -Wformat -Wfortify-source -Wgcc-compat -Wgnu -Wignored-attributes -Wignored-pragmas -Wimplicit -Wmost -Wshadow-all -Wthread-safety -Wuninitialized -Wunused -Wformat
CFLAGS += -Wargument-outside-range -Wassign-enum -Wbitwise-instead-of-logical -Wc23-extensions -Wc11-extensions -Wcast-align -Wcast-function-type -Wcast-qual -Wcomma -Wcomment -Wcompound-token-split -Wconditional-uninitialized -Wduplicate-decl-specifier -Wduplicate-enum -Wduplicate-method-arg -Wduplicate-method-match -Wempty-body -Wempty-init-stmt -Wenum-compare -Wenum-constexpr-conversion -Wextra-tokens -Wfixed-enum-extension -Wfloat-equal -Wloop-analysis -Wframe-address -Wheader-guard -Winfinite-recursion -Wno-gnu-binary-literal -Wint-conversion -Wint-in-bool-context -Wmain -Wmisleading-indentation -Wmissing-braces -Wmissing-prototypes -Wover-aligned -Wundef -Wvla
endif
ifeq ($(CC), cc)
CFLAGS += -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
endif
ifeq ($(CC), gcc)
CFLAGS += -Waggressive-loop-optimizations -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wopenmp-simd -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-varargs -Wstack-usage=8192 -Wstack-protector
endif

INCLUDES := -Iinclude $(foreach dir, $(SRC_DIRS), -I$(dir)/include)
SRCS := $(wildcard $(foreach dir, $(SRC_DIRS), $(dir)/source/*.cpp)) source/main.cpp source/processor.cpp
OBJS := $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

ASAN_FLAGS := -fsanitize=address,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DEBUG_FLAGS := $(ASAN_FLAGS) -D_CANARY_PROTECT -D_HASH_PROTECT -DDEBUG -D_FORTIFY_SOURCES=3 -ggdb -g3

RELEASE_FLAGS := -O2 -march=native -g3 -fomit-frame-pointer -DNDEBUG -flto

CFLAGS += $(INCLUDES) $(RELEASE_FLAGS) -lm

TARGET := ../processor

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: source/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: ../command_processing/source/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: ../Stack/source/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: ../MyMiniLib/source/%.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
