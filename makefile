all: assembler processor

assembler:
	$(MAKE) -C assembler

processor:
	$(MAKE) -C processor

clean:
	$(MAKE) -C assembler clean
	$(MAKE) -C processor clean

.PHONY: assembler processor

