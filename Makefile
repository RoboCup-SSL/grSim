BUILDDIR=build

#change to Debug for debug mode
BUILDTYPE=Release
#BUILDTYPE=Debug

.PHONY: all build mkbuilddir cmake package deb install clean clean-all

all: build

build: mkbuilddir cmake
	$(MAKE) -C $(BUILDDIR)

mkbuilddir:
	[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)

cmake: CMakeLists.txt
	cd $(BUILDDIR) && cmake -DCMAKE_BUILD_TYPE=$(BUILDTYPE) ..

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

