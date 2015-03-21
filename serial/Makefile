include ../Makefile.inc

SHAREDINC = -I..

SRCDIR = .
DEPDIR = .dep
OBJDIR = obj
OUTDIR = ../obj

CXXFLAGS = $(PRJCXXFLAGS) $(SHAREDINC)

SOURCE = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)%, $(OBJDIR)%, $(SOURCE:.cpp=.o))
DEPS = $(patsubst $(OBJDIR)%, $(DEPDIR)%, $(OBJS:.o=.d))

TARGET = $(OUTDIR)/lib$(notdir $(CURDIR)).a

all: $(OUTDIR) $(TARGET)

clean:
	-@rm $(TARGET)
	-@rm -rf .dep
	-@rm -rf obj

$(OUTDIR):
	@mkdir $@

$(TARGET): $(OBJS)
	@echo [LIB] $(notdir $@)
	@$(AR) rcs $@ $^

$(DEPDIR)/%.d: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -MM $< > $@
	@sed -i -e "s,$*.o:,$(OBJDIR)/$*.o $@: ,g" $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo [C++] $(notdir $<)
	@$(CXX) -c $(CXXFLAGS) $< -o $@

-include $(DEPS)
