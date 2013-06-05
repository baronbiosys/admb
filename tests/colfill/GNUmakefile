TARGET=colfill
GTEST_DIR=../googletest-read-only

all: clean $(TARGET) run gtest

gtest: gtest-all.o gtest.cpp
	CXXFLAGS=-I$(GTEST_DIR)/include ../../admb gtest.cpp gtest-all.o
	./gtest

gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc
	$(CXX) -c -I$(GTEST_DIR)/include -I$(GTEST_DIR) -o $@ $<


$(TARGET): $(TARGET).tpl $(TARGET).dat
	../../admb -r -s $(TARGET)

run:
	@printf "Started run:\n"
	./$(TARGET) -nohess
	@printf "Finished run.\n"

clean:
	@rm -vf a
	@rm -vf $(TARGET).p02
	@rm -vf $(TARGET).par
	@rm -vf $(TARGET).b02
	@rm -vf $(TARGET).r02
	@rm -vf b1 b2 s1 s2
	@rm -vf $(TARGET)
	@rm -vf admodel.*
	@rm -vf variance
	@rm -vf fmin.log
	@rm -vf $(TARGET).eva
	@rm -vf $(TARGET).htp
	@rm -vf $(TARGET).bar
	@rm -vf $(TARGET).bgs
	@rm -vf $(TARGET).cor
	@rm -vf $(TARGET).cpp
	@rm -vf $(TARGET).log
	@rm -vf $(TARGET).luu
	@rm -vf $(TARGET).b01
	@rm -vf $(TARGET).psv
	@rm -vf $(TARGET).r01
	@rm -vf $(TARGET).o
	@rm -vf $(TARGET).p01
	@rm -vf $(TARGET).std
	@rm -vf $(TARGET).rep
	@rm -vf $(TARGET).rhes
	@rm -vf f1b2list13
	@rm -vf hesscheck
	@rm -vf data
	@rm -vf sims
	@rm -vf eigv.rpt
	@rm -vf hessian.bin
	@rm -vf f1b2list1 gradfil1.tmp nf1b2list12
	@rm -vf f1b2list12 gradfil2.tmp nf1b2list13
	@rm -vf cmpdiff.tmp f1b2list1 nf1b2list1 varssave.tmp