

all :  mr16sim.exe lesson0.csv lesson.csv loop1.csv loop2.csv

lesson0.csv : lesson0.c
	gcc lesson0.c -o lesson0.exe
	./lesson0.exe



lesson.csv : lesson.c
	gcc lesson.c -o lesson.exe
	./lesson.exe


loop1.csv : loop1.c
	gcc loop1.c -o loop1.exe
	./loop1.exe

loop2.csv : loop2.c
	gcc loop2.c -o loop2.exe
	./loop2.exe

mr16sim.exe : mr16sim.c
	gcc -O2 -Wall mr16sim.c -o mr16sim.exe


clean :
	rm -f *.csv
	rm -f *.exe



