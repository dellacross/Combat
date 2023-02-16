#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>
#define MAX_AMMO 1000
//-------------- Variaveis Globais --------------
const float FPS = 100;  

const int SCREEN_W = 960; //largura (pixels na horizontal) da tela
const int SCREEN_H = 540; //altura (pixels na vertical) da tela

const float THETA =  M_PI/4;
const float RAIO_CAMPO_FORCA = 25; //raio do "escudo" do tanque"
const float VEL_TANQUE = 1; //velocidade de tanque (em pixels)
const float PASSO_ANGULO = M_PI/90; //angulo de giro (2 graus)
const float x_tiro = 0;
const float y_tiro = 0;

int playing = 1;
int contLinha = 0;

ALLEGRO_FONT *size_32;
ALLEGRO_FONT *size_24;

ALLEGRO_BITMAP *menu;
ALLEGRO_BITMAP *vencedor;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;

ALLEGRO_SAMPLE *abertura = NULL;
ALLEGRO_SAMPLE *tiro = NULL;
ALLEGRO_SAMPLE *tiro_acerto = NULL;
ALLEGRO_SAMPLE *final = NULL;
ALLEGRO_SAMPLE *tiro_obst = NULL;
ALLEGRO_SAMPLE *tanque_obst = NULL;
ALLEGRO_SAMPLE *colisao_tanques = NULL;
ALLEGRO_SAMPLE *motor1 = NULL;
ALLEGRO_SAMPLE *motor2 = NULL;

//"instance" para evitar conflitos de som e permitir empregar funcoes no som
ALLEGRO_SAMPLE_INSTANCE *inst_abertura = NULL; 
ALLEGRO_SAMPLE_INSTANCE *inst_tiro = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_tiro_acerto = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_final = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_tiro_obst = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_tanque_obst = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_colisao_tanques = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_motor1 = NULL;
ALLEGRO_SAMPLE_INSTANCE *inst_motor2 = NULL;

bool game_over = false;
bool freeze = false;
bool tanque1_anda = false;
bool tanque2_anda = false;
//-------------- Structs --------------
typedef struct Ponto{
	float x, y; //x e y do ponto
}Ponto;
typedef struct Tanque{
	Ponto centro; //ponto no qual tem x e y do centro do tanque
	Ponto A, B, C; //vertices do tanque
	ALLEGRO_COLOR cor; //cor do tanque
	
	float vel; //velocidade do tanque
	float angulo; //angulo da trajetoria do tanque
	float x_comp, y_comp; //componentes do vetor de deslcamento do tanque
	float vel_angular; //velocidade angular do tanque (varia de acordo com o angulo)
}Tanque;

typedef struct Tiro{
	float x, y; //posicao de origem do tiro
	float vel_tiro; //velocidade do tiro
	float angulo; //angulo de saida do tiro
	bool ativo; //variavel para verificar se ha mais de 1 tiro do mesmo tanque na tela
	int ponto;
}Tiro;
//-------------- Inicializacao --------------
void initTanque(Tanque *t, float x, float y){ 
	
	t->centro.x = x; //x do centro do tanque
	t->centro.y = y; //y do centro do tanque
	t->cor = al_map_rgb(rand()%256, rand()%256, rand()%256); //cor do tanque
	
	t->A.x = 0; 
	t->A.y = -RAIO_CAMPO_FORCA;
	
	float ALPHA = M_PI/2 - THETA;
	float h = RAIO_CAMPO_FORCA * sin(ALPHA);
	float w = RAIO_CAMPO_FORCA * sin(THETA);
	
	t->B.x = -w;
	t->B.y = h;
	
	t->C.x = w;
	t->C.y = h;
	
	t->vel = 0;
	t->angulo = M_PI/2;
	t->x_comp = cos(t->angulo);
	t->y_comp = sin(t->angulo);
	
	t->vel_angular = 0;
}

void initTiro(Tiro tiro[], int tam){ //funcao para inicializar o tiro
	
	for(int i = 0; i < tam; i++) //for utilizada como auxilio na deteccao da quantidade de tiros na tela 
	{ 
		tiro[i].vel_tiro = 5; //velocidade do tiro
		tiro[i].ativo = false; //tiro comeca como falso ("desativado"/nao existente na tela)
		tiro[i].ponto = 0;
	}
}

Tanque tank1;	
Tanque tank2;
Tiro tiro1[MAX_AMMO];	
Tiro tiro2[MAX_AMMO];
//-------------- Cenarios & Obstaculos --------------
void drawCenario(){ //funcao para desenhar cenario
	
	al_clear_to_color(al_map_rgb(10, 101, 7));
	
	//onde colocar o placar ("ceu")
	al_draw_filled_rectangle(0, 0, 
							 SCREEN_W, 40, 
							 al_map_rgb(19, 207, 220));
	
	//obstaculo spawn tank1
	al_draw_filled_rectangle(100, 220, 
							 150, SCREEN_H - 180, 
							 al_map_rgb(90, 67, 14));
	//obstaculo spawn tank2			 
	al_draw_filled_rectangle(SCREEN_W - 150, 220, 
							 SCREEN_W - 100, SCREEN_H - 180, 
							 al_map_rgb(90, 67, 14));
	//obstaculo	meio esquerda		
	al_draw_filled_rectangle(285, 265, 
							 405, 315, 
							 al_map_rgb(90, 67, 14));
	//obstaculo	meio direita				 
	al_draw_filled_rectangle(555, 265, 
							 675, 315, 
							 al_map_rgb(90, 67, 14));
	//borda	esquerda		
	al_draw_filled_rectangle(0, 40,
							 15, SCREEN_H, 
							 al_map_rgb(90, 67, 14));
	//borda	direita						 
	al_draw_filled_rectangle(SCREEN_W - 15, 40, 
							 SCREEN_W, SCREEN_H, 
							 al_map_rgb(90, 67, 14));
	//borda	superior		
	al_draw_filled_rectangle(15, 40, 
							 SCREEN_W - 15, 55, 
							 al_map_rgb(90, 67, 14));
	//borda	inferior						 
	al_draw_filled_rectangle(15, SCREEN_H - 15, 
							 SCREEN_W - 15, SCREEN_H, 
							 al_map_rgb(90, 67, 14));
	
}

void drawFinal(){ //funcao para desenhar a tela final
	
	al_draw_textf(size_24, al_map_rgb(255, 255, 255), SCREEN_W - 200, 15, ALLEGRO_ALIGN_CENTRE, "PLACAR");
	
	al_draw_rectangle(SCREEN_W - 200, 47, 
					SCREEN_W - 170, 87,
					al_map_rgb(255, 255, 255), 1);
	
	al_draw_rectangle(SCREEN_W - 170, 47, 
					SCREEN_W - 25, 87,
					al_map_rgb(255, 255, 255), 1);
	
	al_draw_rectangle(SCREEN_W - 230, 47, 
					SCREEN_W - 200, 87,
					al_map_rgb(255, 255, 255), 1);
	
	al_draw_rectangle(SCREEN_W - 375, 47, 
					SCREEN_W - 230, 87,
					al_map_rgb(255, 255, 255), 1);
}
//-------------- Tanque --------------
void drawTanque(Tanque t){ //desenha o tanque
	
	al_draw_circle(t.centro.x, t.centro.y, RAIO_CAMPO_FORCA, t.cor, 1); //desenha o "escudo" circular do tanque
	
	al_draw_filled_triangle(t.A.x + t.centro.x, t.A.y + t.centro.y, //desenha o tanque em si
							t.B.x + t.centro.x, t.B.y + t.centro.y,
							t.C.x + t.centro.x, t.C.y + t.centro.y,
							t.cor);
}

void Rotate(Ponto *P, float angulo){ //funcao de rotacao do tanque em um ponto especifico
	
	float x = P->x, y= P->y;
	P->x = (x*cos(angulo)) - (y*sin(angulo));
	P->y = (y*cos(angulo)) + (x*sin(angulo));
}

void rotacionaTanque(Tanque *t){ //funcao para rotacionar o tanque
	
	if(t->vel_angular != 0)
	{
		Rotate(&t->A, t->vel_angular); //rotaciona em torno do ponto A
		Rotate(&t->B, t->vel_angular); //rotaciona em torno do ponto B
		Rotate(&t->C, t->vel_angular); //rotaciona em torno do ponto C
	
		t->angulo += t->vel_angular; 
		
		t->x_comp = cos(t->angulo); //da como valor ao componente x do tanque o valor do cos(angulo)
		t->y_comp = sin(t->angulo); //da como valor ao componente y do tanque o valor do sen(angulo)
	}
}

void atualizaTanque(Tanque *t){ //funcao para atualiozar o tanque de acordo com seu movimento

	rotacionaTanque(t);
	t->centro.y += t->vel*t->y_comp; 
	t->centro.x += t->vel*t->x_comp;
}
//-------------- Tiros --------------
void atira(Tiro tiro[], int tam, Tanque t){ //funcao para executa a acao de atirar
	
	for(int i = 0; i < tam; i++)
	{//for utilizada como auxilio na deteccao da quantidade de tiros na tela 
		if(!tiro[i].ativo)//se nao houver tiro "ativo" na tela
		{ 
			tiro[i].angulo = t.angulo; //o tiro saira com o angulo ao qual o tanque esta direcionado
			tiro[i].x = t.centro.x + t.A.x; //origem x do tiro
			tiro[i].y = t.centro.y + t.A.y; //origem y do tiro
			tiro[i].ativo = true; //coloca o tiro como "ativo" na tela
			al_play_sample_instance(inst_tiro);
		}
	}
}

void atualizaTiro(Tiro tiro[], int tam){ //funcao para atualizar posicao do tiro
	
	for(int i = 0; i < tam; i++)//for utilizada como auxilio na deteccao da quantidade de tiros na tela 
	{
		if(tiro[i].ativo)//se houver tiro "ativo" na tela
		{ 
			tiro[i].x -= tiro[i].vel_tiro*cos(tiro[i].angulo); //para atualizar a coordenada x do tiro de acordo com seu movimento
			tiro[i].y -= tiro[i].vel_tiro*sin(tiro[i].angulo); //para atualizar a coordenada y do tiro de acordo com seu movimento
			if(tiro[i].x > SCREEN_W - 15 || tiro[i].x < 15 || tiro[i].y > SCREEN_H - 15 || tiro[i].y < 15){//verifica se o tiro passou da borda do jogo
				tiro[i].ativo = false; //se verdadeiro, retira o tiro da tela (deixando-o como inativo)
				al_play_sample_instance(inst_tiro_obst);
			}
		}
	}
}
void drawTiro(Tiro tiro[], int tam){ //desenha o tiro
	
	for(int i = 0; i < tam; i++)//desenhar cada tiro
		if(tiro[i].ativo)//se houver tiro "ativo" na tela
			al_draw_circle(tiro[i].x, tiro[i].y, 1, al_map_rgb(0, 0, 0), 4); 
}
//-------------- Colisao entre tanques --------------
void colisaoTanques(Tanque *t1, Tanque *t2){ 
	
	float dx = abs(t1->centro.x - t2->centro.x); //distancia em modulo entre a coordenada x do centro do tanque1 ate a coordenada x do tanque2
	float dy = abs(t1->centro.y - t2->centro.y); //distancia em modulo entre a coordenada y do centro do tanque1 ate a coordenada y do tanque2
	
	//para saber a distancia sempre, utilizei o teorema de pitagoras
	float dxy = pow(dx, 2) +  pow(dy, 2); //soma do quadrado dos catetos (dx e dy)
	
	float hip = pow(dxy, 0.5); //raiz da soma dos catetos (hipotenusa), o que seria a distancia entre os centros dos tanques
	
	//struct para auxiliar na identificacao da posicao de um tanque em relacao ao outro
	Tanque *t_cima; 
	Tanque *t_baixo;
	Tanque *t_dir;
	Tanque *t_esq;
	
	if(hip <= 2*RAIO_CAMPO_FORCA) //se a distancia entre os centros for menor que o tamanho de 2 raios
	{
		//para identificar a posicao de um tanque em relacao ao outro
		if(t1->centro.x > t2->centro.x)
		{
			t_esq = t1;
			t_dir = t2;
		}
		else
		{
			t_esq = t2;
			t_dir = t1;
		}
		
		if(t1->centro.y > t2->centro.y)
		{
			t_cima = t2;
			t_baixo = t1;
		}
		else
		{
			t_cima = t1;
			t_baixo = t2;
		}
		
		//mover o tanque 1 pixel para direcao especifica
		if(t_esq->centro.y == t_dir->centro.y)
		{
			t_esq->centro.x--; //para esquerda
			t_dir->centro.x++; //para direita
			al_play_sample_instance(inst_colisao_tanques);
		}
		else if(t_cima->centro.x == t_baixo->centro.x)
		{
			t_cima->centro.y--; //para cima
			t_baixo->centro.y++; //para baixo
			al_play_sample_instance(inst_colisao_tanques);
		}
		else
		{
		t_esq->centro.x--; //para esquerda
		t_dir->centro.x++; //para direita
		t_cima->centro.y--; //para cima
		t_baixo->centro.y++; //para baixo	
		al_play_sample_instance(inst_colisao_tanques);
		}
	}
}
//-------------- Colisao entre tanque e parede --------------

void colisaoTanqueParede(Tanque *t){
	
	//distancia de cada coordenada entre as paredes do jogo
	float dx_cp_dir = abs(t->centro.x - SCREEN_W + 15); 
	float dx_cp_esq = abs(15 - t->centro.x); 
	float dy_cp_sup = abs(55 - t->centro.y);
	float dy_cp_inf = abs(t->centro.y - SCREEN_H + 15);
	
	//no caso de cada parede, mover o tanque para uma direcao especifica
	if(dx_cp_dir <= RAIO_CAMPO_FORCA){
		t->centro.x--; //para esquerda
		al_play_sample_instance(inst_tanque_obst);
	}
	if(dx_cp_esq <= RAIO_CAMPO_FORCA){
		t->centro.x++; //para direita
		al_play_sample_instance(inst_tanque_obst);
	}
	if(dy_cp_sup <= RAIO_CAMPO_FORCA){
		t->centro.y++; //para baixo
		al_play_sample_instance(inst_tanque_obst);
	}
	if(dy_cp_inf <= RAIO_CAMPO_FORCA){
		t->centro.y--; //para cima
		al_play_sample_instance(inst_tanque_obst);
	}
}
//-------------- Colisao entre tiro e tanque --------------

void colisaoTiroTanque(Tanque *t1, Tiro tiro[], int tam){
	
	for(int i = 0; i < MAX_AMMO; i++) 
	{	
		if(tiro[i].ativo)
		{
			float dx = abs(t1->centro.x - tiro[i].x); //distancia entre a coordenada x do tiro e a coordenada x do centro do tanque
			float dy = abs(t1->centro.y - tiro[i].y); //distancia entre a coordenada y do tiro e a coordenada y do centro do tanque
			
			//para saber a distancia sempre, utilizei o teorema de pitagoras
			float dxy = pow(dx, 2) +  pow(dy, 2); //soma do quadrado dos catetos (dx e dy)

			float hip = pow(dxy, 0.5); //raiz da soma dos catetos (hipotenusa), o que seria a distancia entre os centros dos tanques
			
			if(hip <= RAIO_CAMPO_FORCA) //se a distancia entre o tiro e o centro do tanque for menor ou igual o raio do "escudo"
			{
				tiro[i].ativo = false; //desativar o tiro
				tiro[i].ponto++;
				al_play_sample_instance(inst_tiro_acerto);
			}
		}
	}
}
//-------------- Colisao entre tanque e obstaculo --------------

void colisaoTanqueObst(Tanque *t, int sup_esq_x, int sup_esq_y, int inf_dir_x, int inf_dir_y){
	
	float dx_co_esq = abs(t->centro.x - sup_esq_x); //distancia entre a coordenada x do centro do tanque e a coordenada x do ponto superior esquerdo do obstaculo
	float dx_co_dir = abs(inf_dir_x - t->centro.x); //distancia entre a coordenada x do centro do tanque e a coordenada x do ponto inferior direito do obstaculo
	float dy_co_sup = abs(t->centro.y - sup_esq_y); //distancia entre a coordenada y do centro do tanque e a coordenada y do ponto superior esquerdo do obstaculo
	float dy_co_inf = abs(inf_dir_y - t->centro.y); //distancia entre a coordenada y do centro do tanque e a coordenada y do ponto inferior direito do obstaculo
	
	//if's para saber se alguma coordenada do perimetro do escudo do tanque bate em alguma coordenada do obstaculo
	//se verdadeiro, move o tanque 1 pixel na direcao contraria
	if(dx_co_esq <= RAIO_CAMPO_FORCA  && t->centro.y >= sup_esq_y - RAIO_CAMPO_FORCA && t->centro.y <= inf_dir_y + RAIO_CAMPO_FORCA)
	{
		t->centro.x--;
		al_play_sample_instance(inst_tanque_obst);
	}
	
	if(dx_co_dir <= RAIO_CAMPO_FORCA && t->centro.y >= sup_esq_y - RAIO_CAMPO_FORCA && t->centro.y <= inf_dir_y + RAIO_CAMPO_FORCA)
	{
		t->centro.x++;
		al_play_sample_instance(inst_tanque_obst);
	}
	
	if(dy_co_sup <= RAIO_CAMPO_FORCA && t->centro.x >= sup_esq_x - RAIO_CAMPO_FORCA && t->centro.x <= inf_dir_x + RAIO_CAMPO_FORCA)
	{
		t->centro.y--;
		al_play_sample_instance(inst_tanque_obst);
	}
	
	if(dy_co_inf <= RAIO_CAMPO_FORCA && t->centro.x >= sup_esq_x - RAIO_CAMPO_FORCA && t->centro.x <= inf_dir_x + RAIO_CAMPO_FORCA)
	{
		t->centro.y++;
		al_play_sample_instance(inst_tanque_obst);
	}
}
//-------------- Colisao entre tiro e obstaculo --------------

void colisaoTiroObst(Tiro tiro[], int sup_esq_x, int sup_esq_y, int inf_dir_x, int inf_dir_y){
	
	for(int i = 0; i < MAX_AMMO; i++)
	{
		//if's para saber se a coordenada do tiro eh a mesma de alguma coordenada do obstaculo
		//se verdadeiro, "desativa" o tiro na tela
		if(tiro[i].ativo)
		{
			if(tiro[i].x >= sup_esq_x && tiro[i].x < inf_dir_x && tiro[i].y >= sup_esq_y && tiro[i].y <= inf_dir_y)
			{
				tiro[i].ativo = false;
				al_play_sample_instance(inst_tiro_obst);
			}
				
			if(tiro[i].x <= inf_dir_x && tiro[i].x > sup_esq_x && tiro[i].y >= sup_esq_y && tiro[i].y <= inf_dir_y)
			{
				tiro[i].ativo = false;
				al_play_sample_instance(inst_tiro_obst);
			}
				
			if(tiro[i].y >= sup_esq_y && tiro[i].y < inf_dir_y && tiro[i].x >= sup_esq_x && tiro[i].x <= inf_dir_x)
			{
				tiro[i].ativo = false;
				al_play_sample_instance(inst_tiro_obst);
			}

			if(tiro[i].y >= inf_dir_y && tiro[i].y < sup_esq_y && tiro[i].x >= sup_esq_x && tiro[i].x <= inf_dir_x)
			{
				tiro[i].ativo = false;
				al_play_sample_instance(inst_tiro_obst);
			}
		}
	}	
}
//-------------- Start Game --------------

void startGame(){ //funcao que realiza todos os procedimentos iniciais do jogo
	
	game_over = false;
	freeze = false;
	
	//cria o tanque
	initTanque(&tank1, 45, SCREEN_H/2 + 20);
	initTanque(&tank2, SCREEN_W - 45, SCREEN_H/2 + 20);
	
	//gera a bala
	initTiro(tiro1, MAX_AMMO);
	initTiro(tiro2, MAX_AMMO);
}
//-------------- Update Game --------------
void updateGame(){//funcao realidade todos os procedimentos responsaveis pela atualizacao ao longo do jogo
	
	drawCenario();
				
	atualizaTanque(&tank1);
	atualizaTanque(&tank2);
				
	drawTanque(tank1);
	drawTanque(tank2);
				
	drawTiro(tiro1, MAX_AMMO);
	drawTiro(tiro2, MAX_AMMO);
	
	atualizaTiro(tiro1, MAX_AMMO);
	atualizaTiro(tiro2, MAX_AMMO);
	
	colisaoTanques(&tank1, &tank2);
	
	colisaoTanqueParede(&tank1);
	colisaoTanqueParede(&tank2);
	
	colisaoTiroTanque(&tank1, tiro2, MAX_AMMO);
	colisaoTiroTanque(&tank2, tiro1, MAX_AMMO);
	
	colisaoTanqueObst(&tank1, 100, 220, 150, SCREEN_H - 180);
	colisaoTanqueObst(&tank2, 100, 220, 150, SCREEN_H - 180);			
	colisaoTanqueObst(&tank1, SCREEN_W - 150, 220, SCREEN_W - 100, SCREEN_H - 180);
	colisaoTanqueObst(&tank2, SCREEN_W - 150, 220, SCREEN_W - 100, SCREEN_H - 180);			
	colisaoTanqueObst(&tank1, 285, 265, 405, 315);
	colisaoTanqueObst(&tank2, 285, 265, 405, 315);			
	colisaoTanqueObst(&tank1, 555, 265, 675, 315);
	colisaoTanqueObst(&tank2, 555, 265, 675, 315);
	
	//colisao com os obstaculos
	colisaoTiroObst(tiro1, 100, 220, 150, SCREEN_H - 180);			
	colisaoTiroObst(tiro2, 100, 220, 150, SCREEN_H - 180);			
	colisaoTiroObst(tiro1, SCREEN_W - 150, 220, SCREEN_W - 100, SCREEN_H - 180);			
	colisaoTiroObst(tiro2, SCREEN_W - 150, 220, SCREEN_W - 100, SCREEN_H - 180);			
	colisaoTiroObst(tiro1, 285, 265, 405, 315);			
	colisaoTiroObst(tiro2, 285, 265, 405, 315);			
	colisaoTiroObst(tiro1, 555, 265, 675, 315);			
	colisaoTiroObst(tiro2, 555, 265, 675, 315);
	
	//Colisao com as paredes
	colisaoTiroObst(tiro1, 0, 40, 15, SCREEN_H); //borda esquerda
	colisaoTiroObst(tiro2, 0, 40, 15, SCREEN_H); //borda esquerda
	colisaoTiroObst(tiro1, SCREEN_W - 15, 40, SCREEN_W, SCREEN_H);
	colisaoTiroObst(tiro2, SCREEN_W - 15, 40, SCREEN_W, SCREEN_H);
	colisaoTiroObst(tiro1, 15, 40, SCREEN_W - 15, 55);
	colisaoTiroObst(tiro2, 15, 40, SCREEN_W - 15, 55);
	colisaoTiroObst(tiro1, 15, SCREEN_H - 15, SCREEN_W - 15, SCREEN_H);
	colisaoTiroObst(tiro2, 15, SCREEN_H - 15, SCREEN_W - 15, SCREEN_H);
	
	al_draw_textf(size_32, al_map_rgb(255, 255, 255), 290, 1, 0, "%d", tiro1->ponto);
	al_draw_textf(size_32, al_map_rgb(255, 255, 255), 680, 1, 0, "%d", tiro2->ponto);
} 

void reinicia(){ //funcao para recomecar o jogo
	game_over = false;
	freeze = false;
	startGame();
	updateGame();
}

void abreMenu(){ //funcao para gerar o menu do jogo
	
	menu = al_load_bitmap("menu2.jpg");
	al_draw_bitmap(menu, 0, 0, 0);
	al_play_sample_instance(inst_abertura);
	
	int p = 1;
	while(p)
	{
		al_flip_display();
		
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev); //fila de eventos para o menu
		
		//se o tipo de evento for um pressionar de uma tecla
		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
				
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_X: //se clicar X, inicia-se o jogo
					startGame();
					p = 0;
				break;
			}
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			game_over = 1;
			playing = 0; 
			break;
		}
	}
	al_destroy_bitmap(menu);
}
//-------------- Historico --------------

void registraHistorico(Tiro *t1, Tiro *t2){ //para criar e registrar as informacoes no historico
	
	int vitoriasT1 = 0, vitoriasT2 = 0;
	FILE *arq, *temp; //criar arquivo para armazenar os dados e outro para trocar os dados 
	
	arq = fopen("historico.txt", "r"); //abre o arquivo "arq" para leitura
	fscanf(arq, "%d %d", &vitoriasT1, &vitoriasT2); 
	fclose(arq); //fecha o arquivo
	
	temp = fopen("historico.txt", "w"); //usa o filme "temp" para abrir o historico.txt para escrita
	
	if(tiro1->ponto >= 5) //se o tanque 1 chegar a 5 pontos, soma-se uma vitoria ao tanque 1
		vitoriasT1++;
		
	else if(tiro2->ponto >= 5) //se o tanque 2 chegar a 5 pontos, soma-se uma vitoria ao tanque 2
		vitoriasT2++;
	
	fprintf(temp, "%d %d", vitoriasT1, vitoriasT2); //escreve no arquivo o numero de vitorias de cada um 
	
	fclose(temp); //fecha o arquivo temp
}
//-------------- Main --------------
int main(int argc, char **argv){
	
	//inicializa o Allegro
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}
	
	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		return -1;
	}
	
	//inicializa o módulo de primitivas do Allegro
    if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
        return -1;
    }	
	
	//inicializa o modulo que permite carregar imagens no jogo
	if(!al_init_image_addon()){
		fprintf(stderr, "failed to initialize image module!\n");
		return -1;
	}
	
	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}
	
	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		return -1;
	}
	
	//instala o teclado
	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}
	
	//instala o mouse
	if(!al_install_mouse()) {
		fprintf(stderr, "failed to initialize mouse!\n");
		return -1;
	}
	
	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();

	//inicializa o modulo allegro que entende arquivos tff de fontes
	if(!al_init_ttf_addon()) {
		fprintf(stderr, "failed to load tff font module!\n");
		return -1;
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 32 (segundo parametro)
	size_32 = al_load_font("segoesc.ttf", 32, 1);
	if(size_32 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	
	//carrega o arquivo arial.ttf da fonte Arial e define que sera usado o tamanho 24 (segundo parametro) 
	size_24 = al_load_font("segoesc.ttf", 24, 1); 
	if(size_24 == NULL) {
		fprintf(stderr, "font file does not exist or cannot be accessed!\n");
	}
	
	//-------------------------- instalar e inicializar os efeitos sonoros --------------------------
	{
	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(15);
	
	tiro_acerto = al_load_sample("tiro_acerto.wav");
	inst_tiro_acerto = al_create_sample_instance(tiro_acerto);
	al_attach_sample_instance_to_mixer(inst_tiro_acerto, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_tiro_acerto, 0);
	al_set_sample_instance_gain(inst_tiro_acerto, 0.6);
	
	abertura = al_load_sample("abertura.wav");
	inst_abertura = al_create_sample_instance(abertura);
	al_attach_sample_instance_to_mixer(inst_abertura, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_abertura, 0);
	al_set_sample_instance_gain(inst_abertura, 0.7);
	
	final = al_load_sample("final.wav");
	inst_final = al_create_sample_instance(final);
	al_attach_sample_instance_to_mixer(inst_final, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_final, 0);
	al_set_sample_instance_gain(inst_final, 0.8);
	
	tiro_obst = al_load_sample("tiro_obst.wav");
	inst_tiro_obst = al_create_sample_instance(tiro_obst);
	al_attach_sample_instance_to_mixer(inst_tiro_obst, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_tiro_obst, 0);
	al_set_sample_instance_gain(inst_tiro_obst, 0.5);
	
	tiro = al_load_sample("tiro.wav");
	inst_tiro = al_create_sample_instance(tiro);
	al_attach_sample_instance_to_mixer(inst_tiro, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_tiro, 0);
	al_set_sample_instance_gain(inst_tiro, 0.5);
	
	tanque_obst = al_load_sample("tanque_obst.wav");
	inst_tanque_obst = al_create_sample_instance(tanque_obst);
	al_attach_sample_instance_to_mixer(inst_tanque_obst, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_tanque_obst, 0);
	al_set_sample_instance_gain(inst_tanque_obst, 0.5);
	
	colisao_tanques = al_load_sample("colisao_tanques.wav");
	inst_colisao_tanques = al_create_sample_instance(colisao_tanques);
	al_attach_sample_instance_to_mixer(inst_colisao_tanques, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_colisao_tanques, 0);
	al_set_sample_instance_gain(inst_colisao_tanques, 0.5);
	
	motor1 = al_load_sample("motor1.wav");
	inst_motor1 = al_create_sample_instance(motor1);
	al_attach_sample_instance_to_mixer(inst_motor1, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_motor1, 0);
	al_set_sample_instance_gain(inst_motor1, 0.5);
	
	motor2 = al_load_sample("motor1.wav");
	inst_motor2 = al_create_sample_instance(motor2);
	al_attach_sample_instance_to_mixer(inst_motor2, al_get_default_mixer());
	al_set_sample_instance_playmode(inst_motor2, 0);
	al_set_sample_instance_gain(inst_motor2, 0.5);
	}
	
	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	//registra na fila os eventos de mouse (ex: clicar em um botao do mouse)
	al_register_event_source(event_queue, al_get_mouse_event_source());
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	
	//inicia o temporizador
	al_start_timer(timer);
	
	abreMenu();
	
	while(playing){
		
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if(ev.type == ALLEGRO_EVENT_TIMER && (!freeze)) 
		{	
			
			if(tanque1_anda)
				al_play_sample_instance(inst_motor1);
			else if(!tanque1_anda)
				al_stop_sample_instance(inst_motor1);
				
			if(tanque2_anda)
				al_play_sample_instance(inst_motor2);
			else if(!tanque2_anda)
				al_stop_sample_instance(inst_motor2);
		
			if(tiro1->ponto >= 5 || tiro2->ponto >= 5)
			{						
				char c[80];
				FILE *arq;
				
				freeze = true;
				game_over = true;
				
				vencedor = al_load_bitmap("vencedor.jpg");
				al_draw_bitmap(vencedor, 0, 0, 0);
				al_play_sample_instance(inst_final);
			
				if(tiro1->ponto >= 5)
					al_draw_textf(size_24, al_map_rgb(255, 255, 255), 220, 20, ALLEGRO_ALIGN_CENTRE, "O tanque 1 venceu a batalha!");

				else if(tiro2->ponto >= 5)
					al_draw_textf(size_24, al_map_rgb(255, 255, 255), 220, 20, ALLEGRO_ALIGN_CENTRE, "O tanque 2 venceu a batalha!");
				
				al_draw_textf(size_24, al_map_rgb(255, 255, 255),SCREEN_W - 180, 150, ALLEGRO_ALIGN_CENTRE, "Vitorias Tanque 1: ");
				al_draw_textf(size_24, al_map_rgb(255, 255, 255),SCREEN_W - 180, 180, ALLEGRO_ALIGN_CENTRE, "Vitorias Tanque 2: ");
				
				drawFinal();
				registraHistorico(tiro1, tiro2);
				
				al_draw_textf(size_24, al_map_rgb(255, 255, 255), SCREEN_W - 200, 50, ALLEGRO_ALIGN_CENTRE, "Tanque 1  %d  %d  Tanque 2", tiro1->ponto, tiro2->ponto);
				
				arq = fopen("historico.txt", "r");
				
				contLinha = 0;
				
				while(!feof(arq))
				{
					fscanf(arq, "%s", c);
					al_draw_textf(size_24, al_map_rgb(255, 255, 255), SCREEN_W - 30, 150 + contLinha, ALLEGRO_ALIGN_CENTRE, c);
					contLinha+= 30;
				}
				
				al_draw_textf(size_24, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_W/2, ALLEGRO_ALIGN_CENTRE, "Para jogar denovo, tecle X");
				al_draw_textf(size_24, al_map_rgb(255, 255, 255), SCREEN_W/2, SCREEN_H - 30, ALLEGRO_ALIGN_CENTRE, "Para sair, tecle ESC");
				
				al_destroy_bitmap(vencedor);
				fclose(arq);
			}
			else if(!game_over)
				updateGame();
			
			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();
			
			if(al_get_timer_count(timer)%(int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer)/FPS));
		}
		
		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		
		//se o tipo de evento for um pressionar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_W:
					tank1.vel -= VEL_TANQUE;
					tanque1_anda = true;
				break;
				
				case ALLEGRO_KEY_S:
					tank1.vel += VEL_TANQUE;
					tanque1_anda = true;
				break;
				
				case ALLEGRO_KEY_D:
					tank1.vel_angular += PASSO_ANGULO;
				break;
				
				case ALLEGRO_KEY_A:
					tank1.vel_angular -= PASSO_ANGULO;
				break;
			}
			
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP:
					tank2.vel -= VEL_TANQUE;
					tanque2_anda = true;
				break;
				
				case ALLEGRO_KEY_DOWN:
					tank2.vel += VEL_TANQUE;
					tanque2_anda = true;
				break;
				
				case ALLEGRO_KEY_RIGHT:
					tank2.vel_angular += PASSO_ANGULO;
				break;
				
				case ALLEGRO_KEY_LEFT:
					tank2.vel_angular -= PASSO_ANGULO;
				break;
			}
			
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_Q:
					atira(tiro1, MAX_AMMO, tank1);
				break;
				
				case ALLEGRO_KEY_ENTER:
					atira(tiro2, MAX_AMMO, tank2);
			}
			
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_ESCAPE:
					playing = 0;
				break;
					
				case ALLEGRO_KEY_X:	
					startGame();
					al_flip_display();
				break;
			}	
		}
		
		//se o tipo de evento for um soltar de uma tecla
		else if(ev.type == ALLEGRO_EVENT_KEY_UP) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_W:
					tank1.vel += VEL_TANQUE;
					tanque1_anda = false;
				break;
				
				case ALLEGRO_KEY_S:
					tank1.vel -= VEL_TANQUE;
					tanque1_anda = false;
				break;
				
				case ALLEGRO_KEY_D:
					tank1.vel_angular -= PASSO_ANGULO;
				break;
				
				case ALLEGRO_KEY_A:
					tank1.vel_angular += PASSO_ANGULO;
				break;
			}
			
			switch(ev.keyboard.keycode)
			{
				case ALLEGRO_KEY_UP:
					tank2.vel += VEL_TANQUE;
					tanque2_anda = false;
				break;
				
				case ALLEGRO_KEY_DOWN:
					tank2.vel -= VEL_TANQUE;
					tanque2_anda = false;
				break;
				
				case ALLEGRO_KEY_RIGHT:
					tank2.vel_angular -= PASSO_ANGULO;
				break;
				
				case ALLEGRO_KEY_LEFT:
					tank2.vel_angular += PASSO_ANGULO;
				break;
			}
		}
	}//FIM DO WHILE
	
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_font(size_32);
	al_destroy_font(size_24);
	al_destroy_sample(tiro_acerto);
	al_destroy_sample(abertura);
	al_destroy_sample(final);
	al_destroy_sample(tiro_obst);
	al_destroy_sample(tiro);
	al_destroy_sample(tanque_obst);
	al_destroy_sample(colisao_tanques);
	
	return 0;
}