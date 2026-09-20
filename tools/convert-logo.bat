:: Convert Zelina Games Konami-style logo PNG to Screen 5 bitmap header
@echo off
setlocal
cd /d "%~dp0.."
python -c "from PIL import Image; im=Image.open(r'assets\\zelina-games-konami-logo.png').convert('RGB'); px=im.load(); w,h=im.size; ink=lambda x,y: sum(px[x,y])>40; xs=[x for x in range(w) for y in range(h) if ink(x,y)]; ys=[y for y in range(h) for x in range(w) if ink(x,y)]; im=im.crop((min(xs),min(ys),max(xs)+1,max(ys)+1)); tw=240; th=max(2, int(round(im.height*tw/im.width))); th=th+(th&1); im=im.resize((tw,th), Image.Resampling.LANCZOS); im.save(r'assets\\zelina-games-logo-msx.png')"
if not exist generated mkdir generated
MSXgl\tools\MSXtk\bin\MSXimg.exe assets\zelina-games-logo-msx.png -out generated\logo_zelina.h -mode bmp -name g_LogoZelina -bpc 4 -pal custom --palcount 8 --paloff 1 -trans 0x000000 -format c -notitle -nodate
