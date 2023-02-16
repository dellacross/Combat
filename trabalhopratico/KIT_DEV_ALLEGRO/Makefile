ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include

all: bola.exe circles.exe passaro_andante.exe teclado.exe bouncer.exe louco.exe frogger.exe pong.exe passaro_raivoso.exe tela.exe allegro_base.exe combat.exe 

combat.exe: combat.o
	gcc -o combat.exe combat.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

combat.o: combat.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c combat.c	

bola.exe: bola.o
	gcc -o bola.exe bola.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

bola.o: bola.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c bola.c		

circles.exe: circles.o
	gcc -o circles.exe circles.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

circles.o: circles.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c circles.c		

passaro_andante.exe: passaro_andante.o
	gcc -o passaro_andante.exe passaro_andante.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

passaro_andante.o: passaro_andante.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c passaro_andante.c
	
passaro_raivoso.exe: passaro_raivoso.o
	gcc -o passaro_raivoso.exe passaro_raivoso.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

passaro_raivoso.o: passaro_raivoso.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c passaro_raivoso.c	
	
bouncer.exe: bouncer.o
	gcc -o bouncer.exe bouncer.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

bouncer.o: bouncer.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c bouncer.c	
	
teclado.exe: teclado.o
	gcc -o teclado.exe teclado.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

teclado.o: teclado.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c teclado.c	

louco.exe: louco.o
	gcc -o louco.exe louco.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

louco.o: louco.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c louco.c		
	
frogger.exe: frogger.o
	gcc -o frogger.exe frogger.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

frogger.o: frogger.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c frogger.c		
	
pong.exe: pong.o
	gcc -o pong.exe pong.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

pong.o: pong.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c pong.c	
	
tela.exe: tela.o
	gcc -o tela.exe tela.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

tela.o: tela.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c tela.c	
	
allegro_base.exe: allegro_base.o
	gcc -o allegro_base.exe allegro_base.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)

allegro_base.o: allegro_base.c
	gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c allegro_base.c	
	
	
clean:
	del combat.o 
	del combat.exe
	del bola.o 
	del bola.exe
	del circles.o 
	del circles.exe
	del passaro_andante.o 
	del passaro_andante.exe
	del passaro_raivoso.o 
	del passaro_raivoso.exe	
	del louco.o 
	del louco.exe
	del teclado.o 
	del teclado.exe
	del bouncer.o 
	del bouncer.exe	
	del frogger.o
	del frogger.exe
	del pong.o
	del pong.exe
	del tela.o
	del tela.exe
	del allegro_base.o
	del allegro_base.exe

