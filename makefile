all: assembler processor

assembler:
	$(MAKE) -C assembler_sources

processor:
	$(MAKE) -C processor_sources

clean:
	$(MAKE) -C assembler_sources clean
	$(MAKE) -C processor_sources clean

.PHONY: assembler processor

