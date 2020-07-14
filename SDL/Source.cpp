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


const int SCREEN_WIDTH = 700;
const int SCREEN_HEIGHT = 640;

int WC, HC; //ширина и высота текстуры 1 карты
int size =2; 
int speedC = 80; //скорость движени€ карт
int Dcoins = 0; //кол-во очков дилера
int Plcoins = 0; //кол-во очков игрока
	// ‘лаг выхода
bool quit = false;

bool DealerActive = false; //’од игрока

thread *th = nullptr;


SDL_Window *window = nullptr;
SDL_Renderer *render = nullptr;

SDL_Texture *TexAllCard = nullptr ;
SDL_Texture *TexBackCard = nullptr;
SDL_Texture *Background = nullptr;
SDL_Texture *PlayBt = nullptr;
SDL_Texture *QuitBt = nullptr;
SDL_Texture *HitBt = nullptr;
SDL_Texture *StandBt = nullptr;
SDL_Texture *ChangeSkinBt = nullptr;


bool PlayerCardsEnd();
bool DealerCardsEnd();
SDL_Texture *LoadImage(string file);
void StartGive();
void TextureCutAndRender(SDL_Renderer *rend, SDL_Texture *tex, int x, int y, SDL_Rect temp);
void GiveCard(string name, bool visib);

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
vector<BlackJack*> DealerCardsVector;
vector<BlackJack*> PlayerCardsVector;
void GoToCast(BlackJack *Temp, int xnew, int ynew);

class Buttons
{
public:
	SDL_Rect rect;
	bool visible;
	int colrgb[3];
	Buttons() {}
	virtual ~Buttons() {}

	virtual bool Click(int x, int y) = 0;
	virtual void Render(SDL_Renderer *rend) = 0;
};


class SkinTitButtons : public Buttons
{
private:
	bool pack1 = true;
public:
	
	SkinTitButtons()
	{
		colrgb[0] = 255;
		colrgb[1] = 203;
		colrgb[2] = 219;

		visible = true;
		rect.h = 30;
		rect.w = 80;
		rect.x = SCREEN_WIDTH / 2 - rect.w / 2 - 45;
		rect.y = SCREEN_HEIGHT / 2 ;
	}
	virtual ~SkinTitButtons() {}

	virtual bool Click(int x, int y) override
	{
		if (x > rect.x &&x< rect.x + rect.w && y>rect.y &&y < rect.y + rect.h)
		{
			if (pack1)
			TexAllCard = LoadImage("Cards/pack2.bmp");
			else
			TexAllCard = LoadImage("Cards/pack1.bmp");

			pack1 = !pack1;
			return true;
		}
		else return false;
	}
	virtual void Render(SDL_Renderer *rend) override
	{
		if (visible)
			SDL_RenderCopy(rend, ChangeSkinBt, NULL, &rect);

	}
};
SkinTitButtons *ScinTitButt = new SkinTitButtons();
class SkinBackButtons : public Buttons
{

private:
	bool back1 = true;
public:

	SkinBackButtons()
	{
		colrgb[0] = 255;
		colrgb[1] = 203;
		colrgb[2] = 219;

		visible = true;
		rect.h = 30;
		rect.w = 80;
		rect.x = SCREEN_WIDTH / 2 - rect.w / 2 + 45;
		rect.y = SCREEN_HEIGHT / 2 ;
	}
	virtual ~SkinBackButtons() {}

	virtual bool Click(int x, int y) override
	{
		if (x > rect.x &&x< rect.x + rect.w && y>rect.y &&y < rect.y + rect.h)
		{
			if (back1)
				TexBackCard = LoadImage("Cards/back2.bmp");
			else
				TexBackCard = LoadImage("Cards/back1.bmp");

			back1 = !back1;
			return true;
		}
		else return false;
	}
	virtual void Render(SDL_Renderer *rend) override
	{
		if (visible)
			SDL_RenderCopy(rend, ChangeSkinBt, NULL, &rect);

	}
};
SkinBackButtons *ScinBackButt = new SkinBackButtons();

class HitButtons : public Buttons
{

public:
	bool active = false;
	HitButtons() 
	{
		colrgb[0] = 0;
		colrgb[1] = 255;
		colrgb[2] = 0;

		visible = false;
		rect.h = 60;
		rect.w = 100;
		rect.x = SCREEN_WIDTH / 2 - rect.w / 2 - 60;
		rect.y = SCREEN_HEIGHT / 2 - rect.h / 2;
	}
	virtual ~HitButtons() {}

	virtual bool Click(int x, int y) override
	{
		if (x > rect.x &&x< rect.x + rect.w && y>rect.y &&y < rect.y + rect.h && active && PlayerCardsEnd() && DealerCardsEnd())
		{
			GiveCard("Player", true);
			return true;
		}
		else return false;
	}
	virtual void Render(SDL_Renderer *rend) override
	{
		if (!visible)
			return;
		if (active)
		{
				SDL_RenderCopy(rend, HitBt, NULL, &rect);
		}
		else
		{
			SDL_SetRenderDrawColor(render, 100, 100, 100, 0);
			SDL_RenderFillRect(rend, &rect);

			SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
			SDL_RenderDrawRect(rend, &rect);
		}

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
		rect.h = 60;
		rect.w = 100;
		rect.x = SCREEN_WIDTH / 2 - rect.w / 2 + 60;
		rect.y = SCREEN_HEIGHT / 2 - rect.h / 2;
	}
	virtual ~StandButtons() {}

	virtual bool Click(int x, int y) override
	{
		if (x > rect.x &&x< rect.x + rect.w && y>rect.y &&y < rect.y + rect.h && visible && PlayerCardsEnd() &&DealerCardsEnd())
		{
			
			
			hitbut->active = false;
			DealerCardsVector[DealerCardsVector.size() - 1]->visible = true;
			DealerActive = true;
			return true;
		}
		else return false;
		
	}
	virtual void Render(SDL_Renderer *rend) override
	{
		
		if (visible)
			SDL_RenderCopy(rend, StandBt, NULL, &rect);
	
		
	}
};
StandButtons *standbut = new StandButtons();

class QuitButtons : public Buttons
{
public:
	QuitButtons()
	{
		colrgb[0] = 255;
		colrgb[1] = 255;
		colrgb[2] = 255;
		visible = true;
		rect.h = 30;
		rect.w = 80;
		rect.x = SCREEN_WIDTH / 2 - rect.w / 2;
		rect.y = SCREEN_HEIGHT / 2 - rect.h + 140/2;
	}
	virtual ~QuitButtons() {}

	virtual bool Click(int x, int y) override
	{
		if (x > rect.x &&x< rect.x + rect.w && y>rect.y &&y < rect.y + rect.h && visible)
		{
			quit = true;
			return true;
		}
		else return false;;
	}
	virtual void Render(SDL_Renderer *rend) override
	{
		if (visible)
			SDL_RenderCopy(rend, QuitBt, NULL, &rect);

	}
};
QuitButtons *QuitButt = new QuitButtons();
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
		rect.y = SCREEN_HEIGHT / 2 - 140 / 2;
	}
	virtual ~PlayButtons() {}

	virtual bool Click(int x, int y) override
	{
		if (x > rect.x &&x< rect.x + rect.w && y>rect.y &&y < rect.y + rect.h && visible)
		{
			visible = false;
			hitbut->visible = true;
			hitbut->active = true;
			standbut->visible = true;
			

			ScinTitButt->rect.x = SCREEN_WIDTH - ScinTitButt->rect.w;
			ScinTitButt->rect.y = (SCREEN_HEIGHT/2) - 55;

			ScinBackButt->rect.x = SCREEN_WIDTH - ScinBackButt->rect.w;
			ScinBackButt->rect.y = (SCREEN_HEIGHT /2)  - ScinBackButt->rect.h/2;

			QuitButt->rect.x = SCREEN_WIDTH - QuitButt->rect.w;
			QuitButt->rect.y = (SCREEN_HEIGHT / 2) - QuitButt->rect.h/2  + 40;

			for (int i = 0; i < 52; i++)
			{
				th = new thread(GoToCast, CardsVector[i], i / 2.4, 230 + i / 4);
				CardsVector[i]->visible = false;
			}
			th = new thread(StartGive);
			return true;
		}
		else return false;;
	}
	virtual void Render(SDL_Renderer *rend) override
	{
		if (visible)
			SDL_RenderCopy(rend, PlayBt, NULL, &rect);

		/*if (!visible)
			return;
		SDL_SetRenderDrawColor(render,colrgb[0], colrgb[1], colrgb[2],0 );
		SDL_RenderFillRect(rend, &rect);

		SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
		SDL_RenderDrawRect(rend, &rect);*/
		
	}
};
PlayButtons *playbut = new PlayButtons();

vector<Buttons*> ButtonsVector = { playbut, hitbut ,standbut, ScinTitButt, ScinBackButt,QuitButt };


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

void NewGame() //Ќачало новой игры
{
	int sizeboof=0;
	sizeboof = DealerCardsVector.size();
	for (int i = 0; i < sizeboof; i++) //добавление карт дилера назад в колоду
		CardsVector.push_back(DealerCardsVector[i]);
	for (int i = 0; i < sizeboof; i++)//удаление карт дилера
		DealerCardsVector.pop_back();

	sizeboof = PlayerCardsVector.size();
	for (int i = 0; i < sizeboof; i++) //добавление карт игрока назад в колоду
		CardsVector.push_back(PlayerCardsVector[i]);
	for (int i = 0; i < sizeboof; i++)//удаление карт игрока
		PlayerCardsVector.pop_back();

	for (int i = 0; i < CardsVector.size(); i++) //—ложить все карты в колоду
	{
		th = new thread(GoToCast, CardsVector[i], i / 2.4, 230 + i / 4);
		CardsVector[i]->visible = false;
	}
	th = new thread(StartGive); // ¬ыдача 2 карт игроку и дилеру в начале игры

	hitbut->active = true;
	DealerActive = false;
}

void StartGive() // ¬ыдача 2 карт игроку и дилеру в начале игры
{
	this_thread::sleep_for(chrono::milliseconds(1000));
		GiveCard("Dealer", true);
		GiveCard("Player", true);
		GiveCard("Dealer", false);
		GiveCard("Player", true);
	
}
void GiveCard(string name, bool visib )
{
	srand(time(0));
	int irand = rand() % CardsVector.size();
	BlackJack *temp = CardsVector[irand];
	temp->visible = visib;
	int x2boof = CardsVector[CardsVector.size() - 1]->x, y2boof = CardsVector[CardsVector.size() - 1]->y;

	CardsVector[CardsVector.size() - 1]->x = temp->x;
	CardsVector[CardsVector.size() - 1]->y = temp->y;

	CardsVector[irand] = CardsVector[CardsVector.size() - 1];

	CardsVector[CardsVector.size() - 1] = temp;
	temp->x = x2boof;
	temp->y = y2boof;

	if (name == "Player")
	{
		PlayerCardsVector.push_back(CardsVector[CardsVector.size() - 1]);

		th = new thread(GoToCast, PlayerCardsVector[PlayerCardsVector.size() - 1],
			(SCREEN_WIDTH / 2 - PlayerCardsVector[PlayerCardsVector.size() - 1]->Rcard.w / 2) + 30 * PlayerCardsVector.size(),
			450);
	}
	else if (name == "Dealer")
	{
		DealerCardsVector.push_back(CardsVector[CardsVector.size() - 1]);

		th = new thread(GoToCast, DealerCardsVector[DealerCardsVector.size() - 1],
			(SCREEN_WIDTH / 2 - DealerCardsVector[DealerCardsVector.size() - 1]->Rcard.w / 2) + 30 * DealerCardsVector.size(),
			10);
	}


	CardsVector.pop_back();



	
}
bool PlayerCardsEnd()
{
	if (PlayerCardsVector.size() == 0)
		return false;

	if (PlayerCardsVector[PlayerCardsVector.size() - 1]->x == (SCREEN_WIDTH / 2 - PlayerCardsVector[PlayerCardsVector.size() - 1]->Rcard.w / 2) + 30 * PlayerCardsVector.size() &&
		PlayerCardsVector[PlayerCardsVector.size() - 1]->y == 450)
		return true;
	return false;
}
bool DealerCardsEnd()
{
	if (DealerCardsVector.size() == 0)
		return false;

	if ((DealerCardsVector[DealerCardsVector.size() - 1]->x == (SCREEN_WIDTH / 2 - DealerCardsVector[DealerCardsVector.size() - 1]->Rcard.w / 2) + 30 * DealerCardsVector.size()) &&
		(DealerCardsVector[DealerCardsVector.size() - 1]->y == 10))
		return true;
	return false;
}
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

	PlayBt = LoadImage("Cards/PlayBt.bmp");
	QuitBt = LoadImage("Cards/Quit.bmp");
	HitBt = LoadImage("Cards/HitBt.bmp");
	StandBt = LoadImage("Cards/StandBt.bmp");
	ChangeSkinBt = LoadImage("Cards/ChangeSkinBt.bmp");

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


	
	int xMouse, yMouse;
	

	SDL_SetRenderDrawColor(render,0,255,0,255);
	
	
	

	while (!quit)
	{
		// ќбработка событий
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
					if (ButtonsVector[i]->Click(xMouse, yMouse))
						break;
					
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
					
				case  SDLK_6:
					
					break;
				}
					break;	
			default:
				break;
			}
			
			
		}
		// ќтображение сцены
		SDL_RenderClear(render);
		SDL_RenderCopy(render, Background,NULL,NULL);
		

		for (int i = 0; i < CardsVector.size(); i++)
		{
			CardsVector[i]->Render(render);
		}
		for (int i = 0; i < PlayerCardsVector.size(); i++)
		{
			PlayerCardsVector[i]->Render(render);
		}
		for (int i = 0; i < DealerCardsVector.size(); i++)
		{
			DealerCardsVector[i]->Render(render);
		}

		for (int i = 0; i < ButtonsVector.size(); i++)
		{
			ButtonsVector[i]->Render(render);
		}
		SDL_RenderPresent(render);

		Dcoins = 0;
		for (int i = 0; i < DealerCardsVector.size(); i++)
		{
			Dcoins += DealerCardsVector[i]->value;
		}
		Plcoins = 0;
		for (int i = 0; i < PlayerCardsVector.size(); i++)
		{
			Plcoins += PlayerCardsVector[i]->value;
		}

		

		if (Plcoins > 21 && PlayerCardsEnd())
		{
			SDL_ShowSimpleMessageBox(NULL, "Result", "DEALER WINS\nYOU>21", window);
			NewGame();
			continue;
		}

		if (DealerActive)
		if (DealerCardsEnd() && Dcoins < 22 && Dcoins < Plcoins)
		{
			GiveCard("Dealer", true);
		}
		else {
			if (Dcoins == Plcoins && DealerCardsEnd())
			{
				SDL_ShowSimpleMessageBox(NULL, "Result", " A TIE!", window);
				NewGame();
				continue;
			}
			if ((Dcoins > 21 || (Dcoins < Plcoins && Plcoins <= 21 && DealerActive)) && DealerCardsEnd())
			{
				SDL_ShowSimpleMessageBox(NULL, "Result", "YOU WIN!", window);
				NewGame();
				continue;
			}
			if ((Plcoins > 21 || (Plcoins < Dcoins && Dcoins <= 21 && DealerActive))&& DealerCardsEnd())
			{
				SDL_ShowSimpleMessageBox(NULL, "Result", "DEALER WINS", window);
				NewGame();
				continue;
			}
		}

		

		//SDL_RenderCopy(render, TexAllCard, NULL, NULL);

		
		SDL_Delay(10);
	}
	
	
	
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}