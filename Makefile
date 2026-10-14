CC = x86_64-w64-mingw32-gcc
WINDRES = x86_64-w64-mingw32-windres
CFLAGS = -O2 -Wall -mwindows
LDFLAGS = -static -static-libgcc -s -lgdi32 -luser32 -lcomctl32 -lgdiplus -lshell32

all: DevHome.exe DevHome.msi

assets/resources.o: assets/resources.rc assets/app.ico assets/app.manifest
	$(WINDRES) assets/resources.rc -O coff -o assets/resources.o

DevHome.exe: src/main.c assets/resources.o
	$(CC) $(CFLAGS) src/main.c assets/resources.o -o DevHome.exe $(LDFLAGS)

installer/installer.wxs: assets/app.ico DevHome.exe
	@echo "WiX installer file ready"

DevHome.msi: DevHome.exe installer/installer.wxs
	wixl -o DevHome.msi installer/installer.wxs

clean:
	rm -f assets/resources.o DevHome.exe DevHome.msi
