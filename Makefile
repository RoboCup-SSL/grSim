BUILDDIR=build

#change to Debug for debug mode
BUILDTYPE=Release
#BUILDTYPE=Debug

all: build

mkbuilddir:
	[ -d $(BUILDDIR) ] || mkdir $(BUILDDIR)

cmake: mkbuilddir CMakeLists.txt
	cd $(BUILDDIR) && cmake -DCMAKE_BUILD_TYPE=$(BUILDTYPE) ..

build: cmake
	$(MAKE) -C $(BUILDDIR)

clean: mkbuilddir
	$(MAKE) -C $(BUILDDIR) clean

package: cmake
	$(MAKE) -C $(BUILDDIR) package

deb: cmake
	cd $(BUILDDIR) && cpack -G DEB

install: mkbuilddir
	$(MAKE) -C $(BUILDDIR) install
	
cleanup_cache: mkbuilddir
	cd $(BUILDDIR) && rm -rf *
	
