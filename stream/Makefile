###############################################################################
# NoSpark - OpenEVSE charger firmware
# Copyright (C) 2015 Andre Eisenbach
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# See LICENSE for a copy of the GNU General Public License or see
# it online at <http://www.gnu.org/licenses/>.
###############################################################################

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
