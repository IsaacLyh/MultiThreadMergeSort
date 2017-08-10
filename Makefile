include $(PUB)/Arch/arch.gnu-c++11.generic
WARNINGS += -Wall -pedantic

OPTIMIZATION += -O3

# Defines the macro "SERIAL"
# To enable selective compilaton of a serial version of the code
ifeq ($(serial), 1)
	C++FLAGS += -DSERIAL
	CFLAGS += -DSERIAL
endif
 
MSORT_OBJ	= mSortDriver.o MergeSort.o Merge.o Timer.o cmdLine.o verify.o


msort:	        $(MSORT_OBJ)
		$(C++LINK) $(LDFLAGS) -o $@ $(MSORT_OBJ)  $(LDLIBS)

.PHONY: clean

clean:
	$(RM) *.o msort
	$(RM) core
