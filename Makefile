# use `make` to compile singly-linked list
# use `make CXXFLAGS=-DDOUBLY_LINKED_LIST` to compile doubly-linked list
# use `make CXXFLAGS=-DDEBUG` to compile debug version
# use `make lib` to generate static library

CXX_DEBUG_FLAGS ?= -D_DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations           \
                   -Wc++14-compat -Wmissing-declarations -Wdeprecated -Wcast-align -Wcast-qual -Wchar-subscripts   \
                   -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal           \
                   -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op      \
                   -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self      \
                   -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel                \
                   -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types  \
                   -Wsuggest-override -Wswitch-default -Wsync-nand -Wundef -Wunreachable-code                      \
                   -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers   \
                   -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new                   \
                   -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging                 \
                   -fno-omit-frame-pointer -pie -fPIE -Werror=vla                                                  \
                   -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

EXECUTABLE_BINARY = list-example.x

SRC_DIR = ./src
INCLUDE_DIR = ./include
O_DIR ?= ./build
LIB_DIR ?= ./lib

CSRC = $(SRC_DIR)/main.c $(SRC_DIR)/linked-list.c
COBJ = $(addprefix $(O_DIR)/, $(CSRC:.c=.o))

SRC_FOR_LIB = $(SRC_DIR)/linked-list.c
OBJ_FOR_LIB = $(LIB_DIR)/linked-list.o
HEADERS_FOR_LIB = $(INCLUDE_DIR)/linked-list.h


LIBNAME = liblist.a

#--------------------------------------------------------------------

override CXXFLAGS += -I$(INCLUDE_DIR)

ifneq (0, $(words $(findstring -DDEBUG, $(CXXFLAGS))))
override CXXFLAGS += $(CXX_DEBUG_FLAGS)
override LDFLAGS += $(CXX_DEBUG_FLAGS)
else
override CXXFLAGS += -DNDEBUG
endif

#--------------------------------------------------------------------

.PHONY: all
all: $(O_DIR)/$(EXECUTABLE_BINARY)

$(O_DIR)/$(EXECUTABLE_BINARY): $(COBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

$(COBJ): $(O_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

#--------------------------------------------------------------------

.PHONY: lib
lib:
	@mkdir -p $(LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $(SRC_FOR_LIB) -o $(OBJ_FOR_LIB)
	ar rs $(LIB_DIR)/$(LIBNAME) $(OBJ_FOR_LIB)
	rm $(OBJ_FOR_LIB)
	cp $(HEADERS_FOR_LIB) $(LIB_DIR)

#--------------------------------------------------------------------

.PHONY: run
run:
	$(O_DIR)/$(EXECUTABLE_BINARY)

#--------------------------------------------------------------------

.PHONY: clean
clean:
	rm -rf $(O_DIR) $(LIB_DIR)

#--------------------------------------------------------------------
