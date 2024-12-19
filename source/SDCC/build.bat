sdcc -c --opt-code-size --debug -o.\obj\ ..\src\clock.c
sdcc -c --opt-code-size --debug -o.\obj\ ..\src\isp.c 
sdcc -c --opt-code-size --debug -o.\obj\ ..\src\tpi.c
sdcc -c --opt-code-size --debug -o.\obj\ ..\src\usbdrv.c
sdcc -c --opt-code-size --debug -o.\obj\ ..\src\main.c

sdcc --code-loc 0x0000 --no-xinit-opt .\obj\main.rel .\obj\clock.rel .\obj\isp.rel .\obj\tpi.rel .\obj\usbdrv.rel -o.\obj\usbasp.ihx 
packihx .\obj\usbasp.ihx > .\obj\usbasp.hex
pause