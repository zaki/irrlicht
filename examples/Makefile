DIRS = $(wildcard [012]* Demo)

all: $(DIRS)

$(DIRS):
	-@cd $@ && make clean && make

.PHONY: $(DIRS)
