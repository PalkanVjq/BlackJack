#include <iostream>
#include <SDL.h>
#include <vector>
#include <string>
#include <ctime>
#include <thread>
#include <chrono>
#define RRR 30
#define GGG 255
#define BBB 30
using namespace std;


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 640;

int WC, HC; //ширина и высота текстуры 1 карты
int size =2; 
int speedC = 80; //скорость движения карт

thread *th = nullptr;

SDL_Window *window = nullptr;
SDL_Renderer *render = nullptr;

SDL_Texture *TexAllCard = nullptr ;
SDL_Texture *TexBackCard = nullptr;
SDL_Texture *Background = nullptr;

SDL_Texture *LoadImage(string file);
void DrawBack(SDL_Renderer *rend);
void TextureCutAndRender(SDL_Renderer *rend, SDL_Texture *tex, int x, int y, SDL_Rect temp);

class BlackJack
{
public:

	SDL_Rect Rcard;
	int value = 0;
	int x = 0, y = 0;
	int visible;

	BlackJack(){}
	~BlackJack(){}

	void Render(SDL_Renderer *rend)
	{
		if (visible)
		TextureCutAndRender(rend, TexAllCard, x, y, Rcard);
		else
		{
			SDL_Rect rect;
			rect.x = x;
			rect.y = y;
			rect.h = 157;
			rect.w = 112;

			SDL_RenderCopy(rend, TexBackCard, NULL, &rect);
		}
	}
};
vector<BlackJack*> CardsVector;
void GoToCast(BlackJack *Temp, int xnew, int ynew);

class Buttons
{
public:
	SDL_Rect rect;
	bool visible;
	int colrgb[3];
	Buttons() {}
	virtual ~Buttons() {}

	virtual void Click(int x, int y) = 0;
	virtual void Render(SDL_Renderer *rend) = 0;
};


class HitButtons : public Buttons
{
public:

	HitButtons() 
	{
		colrgb[0] = 0;
		colrgb[1] = 255;
		colrgb[2] = 0;

		visible = false;
		rect.h = 30;
		rect.w = 50;
		rect.x = SCREEN_WIDTH / 2 - rect.w / 2 - 60;
		rect.y = SCREEN_HEIGHT / 2 - rect.h / 2;
	}
	virtual ~HitButtons() {}

	virtual void Click(int x, int y) override
	{

	}
	virtual void Render(SDL_Renderer *rend) override
	{

	}
};
HitButtons *hitbut = new HitButtons();

class StandButtons : public Buttons
{
public:

	StandButtons() 
	{
		colrgb[0] = 255;
		colrgb[1] = 0;
		colrgb[2] = 0;

		visible = false;
		rect.h = 30;
		rect.w = 50;
		rect.x = SCREEN_WIDTH / 2 - rect.w / 2 + 60;
		rect.y = SCREEN_HEIGHT / 2 - rect.h / 2;
	}
	virtual ~StandButtons() {}

	virtual void Click(int x, int y) override
	{

	}
	virtual void Render(SDL_Renderer *rend) override
	{

	}
};

StandButtons *standbut = new StandButtons();

class PlayButtons : public Buttons
{
public:
	PlayButtons()
	{
		colrgb[0] = 255;
		colrgb[1] = 255;
		colrgb[2] = 0;
		visible = true;
		rect.h = 60;
		rect.w = 100;
		rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
		rect.y = SCREEN_HEIGHT / 2 - rect.h / 2;
	}
	virtual ~PlayButtons() {}

	virtual void Click(int x, int y) override
	{
		if (x > rect.x &&x< rect.x + rect.w && y>rect.y &&y < rect.y + rect.h && visible)
		{
			visible = false;
			hitbut->visible = true;
			standbut->visible = true;

			for (int i = 0; i < 52; i++)
			{
				th = new thread(GoToCast, CardsVector[i], i / 2.4, 230 + i / 4);
				CardsVector[i]->visible = false;
			}
		}
		else return;
	}
	virtual void Render(SDL_Renderer *rend) override
	{
		if (!visible)
			return;
		SDL_SetRenderDrawColor(render,colrgb[0], colrgb[1], colrgb[2],0 );
		SDL_RenderFillRect(rend, &rect);

		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		SDL_RenderDrawRect(rend, &rect);
		//DrawBack(rend);
	}
};
PlayButtons *playbut = new PlayButtons();

vector<Buttons*> ButtonsVector = { playbut, hitbut ,standbut };;


void GoToCast(BlackJack *Temp, int xnew, int ynew)
{
	float distanse = sqrt(((xnew - Temp->x)*(xnew - Temp->x)) + ((ynew - Temp->y)*(ynew - Temp->y)));

	while (distanse > 10)
	{
		distanse = sqrt(((xnew - Temp->x)*(xnew - Temp->x)) + ((ynew - Temp->y)*(ynew - Temp->y)));
		Temp->x += 0.1 * speedC * (xnew - Temp->x) / distanse;
		Temp->y += 0.1 * speedC * (ynew - Temp->y) / distanse;
		this_thread::sleep_for(chrono::milliseconds(10));
	}
	Temp->x = xnew;
	Temp->y = ynew;
}


//
//void DrawBack(SDL_Renderer *rend)
//{
//	SDL_SetRenderDrawColor(render, RRR, GGG, BBB, 0);
//}


void Init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	//создание окна
	window = SDL_CreateWindow("BlackJack", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDL_DestroyTexture(TexAllCard);
	TexAllCard = LoadImage("Cards/pack1.bmp");
	TexBackCard = LoadImage("Cards/back1.bmp");
	Background = LoadImage("Cards/background.bmp");

	SDL_QueryTexture(TexAllCard,NULL,NULL,&WC,&HC);
	WC /= 13;
	HC /= 4;

	BlackJack *TempCard;

	// добавление карт
	for (int j = 0; j < 4; j++) {
		for (int i = 0; i < 1; i++)
		{
			TempCard = new BlackJack();
			TempCard->value = 11;

			TempCard->Rcard.x = i * WC;
			TempCard->Rcard.y = j * HC;
			TempCard->Rcard.h = HC;
			TempCard->Rcard.w = WC;
			CardsVector.push_back(TempCard);
		}

		for (int i = 1; i < 10; i++)
		{
			TempCard = new BlackJack();
			TempCard->value = i + 1;

			TempCard->Rcard.x = i * WC;
			TempCard->Rcard.y = j * HC;
			TempCard->Rcard.h = HC;
			TempCard->Rcard.w = WC;
			CardsVector.push_back(TempCard);
		}

		for (int i = 10; i < 13; i++)
		{
			TempCard = new BlackJack();
			TempCard->value = 10;

			TempCard->Rcard.x = i * WC;
			TempCard->Rcard.y = j * HC;
			TempCard->Rcard.h = HC;
			TempCard->Rcard.w = WC;
			CardsVector.push_back(TempCard);
		}
	}

	srand(time(0));
	//размещение карт по всему окну пред началом игры
	for (int i = 0; i < CardsVector.size(); i++)
	{
		CardsVector[i]->x = rand() % (SCREEN_WIDTH - CardsVector[i]->Rcard.w / 2);
		CardsVector[i]->y = rand() % (SCREEN_HEIGHT - CardsVector[i]->Rcard.h / 2);
	}
	//лицом вверх\вниз (рандом) карт пред началом игры
	for (int i = 0; i < CardsVector.size(); i++)
	{
		if (rand()% 10 == 1)
		CardsVector[i]->visible = false;
		else CardsVector[i]->visible = true;
		
	}
}
SDL_Texture *LoadImage(string file) {
	SDL_Surface *tSurf = nullptr;
	SDL_Texture* tText = nullptr;
	tSurf = SDL_LoadBMP(file.c_str());

	tText = SDL_CreateTextureFromSurface(render,tSurf);
	SDL_FreeSurface(tSurf);
	return tText;
}

void TextureCutAndRender(SDL_Renderer *rend, SDL_Texture *tex, int x, int y, SDL_Rect temp)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.h = temp.h/2;
	rect.w = temp.w/2;

	SDL_RenderCopy(rend, tex, &temp, &rect);
}

int main(int argc, char* argv[])
{
	Init();
	srand(time(0));


	SDL_Event e;

	// Флаг выхода
	bool quit = false;
	
	int xMouse, yMouse;

	SDL_SetRenderDrawColor(render,0,255,0,255);
	
	
	

	while (!quit)
	{
		// Обработка событий
		while (SDL_PollEvent(&e) != NULL)
		{
			switch (e.type)
			{
			case SDL_MOUSEMOTION:
				xMouse = e.motion.x;
				yMouse = e.motion.y;

				break;

			case SDL_MOUSEBUTTONDOWN:
			{
				
				for (int i = 0; i < ButtonsVector.size(); i++)
				{
					ButtonsVector[i]->Click(xMouse, yMouse);
					
				}
			}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) 
				{
				case  SDLK_1:
					SDL_DestroyTexture(TexAllCard);
					TexAllCard = LoadImage("Cards/pack2.bmp");
					break;
				case  SDLK_2:
					SDL_DestroyTexture(TexAllCard);
					TexAllCard = LoadImage("Cards/pack1.bmp");
					break;
				case  SDLK_3:
					SDL_DestroyTexture(TexBackCard);
					TexBackCard = LoadImage("Cards/back1.bmp");
					break;
				case  SDLK_4:
					SDL_DestroyTexture(TexBackCard);
					TexBackCard = LoadImage("Cards/back2.bmp");
					break;
				case  SDLK_5:
					for (int i = 0; i < CardsVector.size(); i++)
						CardsVector[i]->visible = !CardsVector[i]->visible;
					break;

				}
					break;	
			default:
				break;
			}
			
			
		}
		// Отображение сцены
		SDL_RenderClear(render);
		SDL_RenderCopy(render, Background,NULL,NULL);
		

		
		

		for (int i = 0; i < CardsVector.size(); i++)
		{
			CardsVector[i]->Render(render);
		}
		for (int i = 0; i < ButtonsVector.size(); i++)
		{
			ButtonsVector[i]->Render(render);
		}

		//SDL_RenderCopy(render, TexAllCard, NULL, NULL);

		SDL_RenderPresent(render);
		SDL_Delay(10);
	}
	
	
	
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}