PACKAGES=
.PHONY: all clean
all: 
	for p in $(PACKAGES); do \
		(cd $$p && $(MAKE) $@); \
	done