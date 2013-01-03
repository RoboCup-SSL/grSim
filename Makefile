BUILDDIR=build

#change to Debug for debug mode
BUILDTYPE=Release
#BUILDTYPE=Debug

.PHONY: all build mkbuilddir cmake dist package deb install clean clean-all

all: build

deploy: dist upload

# requires pygithub3:
# pip install pygithub3
upload:
	./upload.py --all

build: mkbuilddir cmake
	$(MAKE) -C $(BUILDDIR)

mkbuilddir:
	[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)

cmake: CMakeLists.txt
	cd $(BUILDDIR) && cmake -DCMAKE_BUILD_TYPE=$(BUILDTYPE) ..

dist: package

package: all
	$(MAKE) -C $(BUILDDIR) package

deb: all
	cd $(BUILDDIR) && cpack -G DEB

install: all
	$(MAKE) -C $(BUILDDIR) install

clean: mkkbuilddir cmake
	$(MAKE) -C $(BUILDDIR) clean
	
clean-all: mkbuilddir
	cd $(BUILDDIR) && rm -rf *

