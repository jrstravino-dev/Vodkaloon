# Vodkaloon — public build entry (wraps MSXgl build.bat)

.PHONY: all build clean help rom

all: build

help:
	@echo Targets: build clean rom help
	@echo Output ROM: emul\rom\Vodkaloon.rom

build:
	@build.bat

rom: build
	@if not exist build mkdir build
	@if exist emul\rom\Vodkaloon.rom copy /Y emul\rom\Vodkaloon.rom build\Vodkaloon.rom

clean:
	@if exist out rmdir /S /Q out
	@if exist emul rmdir /S /Q emul
	@if exist build\Vodkaloon.rom del /Q build\Vodkaloon.rom
