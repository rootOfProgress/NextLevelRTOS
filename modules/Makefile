PACKAGES= weatherNodeV2
.PHONY: all clean
all: 
	for p in $(PACKAGES); do \
		(cd $$p && $(MAKE) $@); \
	done