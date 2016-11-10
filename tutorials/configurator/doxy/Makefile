#
# Definitions
#
DOXYGEN=doxygen
OUTDIR=../html

#
# Targets
#
all:
	rm -rf $(OUTDIR)
	mkdir $(OUTDIR)
	$(DOXYGEN)
	cp *.gif *.png *.svg $(OUTDIR)
clean:
	-rm -rf $(OUTDIR)
