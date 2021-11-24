#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup") 
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<fstream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include<utility>
#include<algorithm>
#include<string>
#include<vector>
#include<stdio.h>

using namespace std;

void menu();
void game();
void gameover();
void scoreboard();

#define PI 3.14159265359

typedef uint8_t byte;

byte grid[20][12];
byte colliders[20][12];


struct Vec2
{
	byte y, x;
	Vec2() {}
	Vec2(byte dy, byte dx) : x(dx), y(dy) {}
};

struct Piece
{
	Vec2 a, b, c, d;
	Piece() {}
	Piece(byte ax, byte ay, byte bx, byte by, byte cx, byte cy, byte dx, byte dy)
		: a(Vec2(ax, ay)), b(Vec2(bx, by)), c(Vec2(cx, cy)), d(Vec2(dx, dy)) {}
	Piece(Vec2 da, Vec2 db, Vec2 dc, Vec2 dd)
		: a(da), b(db), c(dc), d(dd) {}
};

struct Piece2
{
	Vec2 a, b, c, d;
	Piece2() {}
	Piece2(byte ax, byte ay, byte bx, byte by, byte cx, byte cy, byte dx, byte dy)
		: a(Vec2(ax, ay)), b(Vec2(bx, by)), c(Vec2(cx, cy)), d(Vec2(dx, dy)) {}
	Piece2(Vec2 da, Vec2 db, Vec2 dc, Vec2 dd)
		: a(da), b(db), c(dc), d(dd) {}
};

enum PIECE_TYPE
{
	S, Z, L, J, SQR, I, T
};

Piece CreatePiece(PIECE_TYPE type)
{
	switch (type)
	{
	case S: return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(0, 4), Vec2(2, 5));
		break;
	case Z: return Piece(Vec2(1, 4), Vec2(1, 5), Vec2(0, 5), Vec2(2, 4));
		break;
	case L: return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 6));
		break;
	case J: return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 4));
		break;
	case SQR: return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(0, 5), Vec2(0, 4));
		break;
	case I: return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(1, 7));
		break;
	case T: return Piece(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 5));
		break;
	default:
		break;
	}
}

Piece2 CreatePiece2(PIECE_TYPE type)
{
	switch (type)
	{
	case S: return Piece2(Vec2(1, 5), Vec2(1, 4), Vec2(0, 4), Vec2(2, 5));
		break;
	case Z: return Piece2(Vec2(1, 4), Vec2(1, 5), Vec2(0, 5), Vec2(2, 4));
		break;
	case L: return Piece2(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 6));
		break;
	case J: return Piece2(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 4));
		break;
	case SQR: return Piece2(Vec2(1, 5), Vec2(1, 4), Vec2(0, 5), Vec2(0, 4));
		break;
	case I: return Piece2(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(1, 7));
		break;
	case T: return Piece2(Vec2(1, 5), Vec2(1, 4), Vec2(1, 6), Vec2(0, 5));
		break;
	default:
		break;
	}
}

void rotate(Piece& piece)
{
	float angle = 90 * (PI / 180);

	float b_offset_x = piece.b.x - piece.a.x;
	float c_offset_x = piece.c.x - piece.a.x;
	float d_offset_x = piece.d.x - piece.a.x;

	float b_offset_y = piece.b.y - piece.a.y;
	float c_offset_y = piece.c.y - piece.a.y;
	float d_offset_y = piece.d.y - piece.a.y;

	float pbx = piece.a.x + (b_offset_x * cosf(angle) - b_offset_y * sinf(angle));
	float pby = piece.a.y + (b_offset_x * sinf(angle) + b_offset_y * cosf(angle));

	float pcx = piece.a.x + (c_offset_x * cosf(angle) - c_offset_y * sinf(angle));
	float pcy = piece.a.y + (c_offset_x * sinf(angle) + c_offset_y * cosf(angle));

	float pdx = piece.a.x + (d_offset_x * cosf(angle) - d_offset_y * sinf(angle));
	float pdy = piece.a.y + (d_offset_x * sinf(angle) + d_offset_y * cosf(angle));

	
	if (colliders[(byte)pby][(byte)pbx] != 2 &&
		colliders[(byte)pcy][(byte)pcx] != 2 &&
		colliders[(byte)pdy][(byte)pdx] != 2 &&
		piece.a.x != 1 && piece.a.y != 1)
	{
		piece.b.x = pbx;
		piece.b.y = pby;

		piece.c.x = pcx;
		piece.c.y = pcy;

		piece.d.x = pdx;
		piece.d.y = pdy;
	}
}

void showHighScore(int x, int y, std::string word, sf::RenderWindow& window, sf::Font* font)
{
	sf::Text text;
	text.setFont(*font);
	text.setPosition(x, y);
	text.setString(word);
	text.setCharacterSize(32);
	window.draw(text);
}

void gameover(int score_input) {
	sf::RenderWindow window(sf::VideoMode(960, 960), "Tetris", sf::Style::Titlebar | sf::Style::Close);
	window.setKeyRepeatEnabled(true);
	window.setFramerateLimit(60);

	int score[6];
	
	sf::Texture BG_over;
	BG_over.loadFromFile("images/BG_over.png");

	sf::Sprite BG(BG_over);
	BG.setScale(1.25, 1.25);

	
	//gover.play();

	sf::Text GameOver;
	sf::Text Score;
	sf::Text hightscore;
	sf::Text hightscore1;
	sf::Text hightscore2;
	sf::Text hightscore3;
	sf::Text hightscore4;
	sf::Text hightscore5;
	sf::Text r1;
	sf::Text r2;
	sf::Text r3;
	sf::Text r4;
	sf::Text r5;
	sf::Text Rank;
	sf::Text s;
	sf::Text Back;
	sf::Text exit;

	sf::Font font;
	font.loadFromFile("Forwa_font.TTF");

	Back.setFont(font);
	Back.setCharacterSize(20);
	Back.setFillColor(sf::Color::Green);
	Back.setPosition(100, 375);
	sf::Vector2<float>back_scale(1.0f, 1.0f);
	Back.setScale(back_scale);
	Back.setString("Enter -> Back to menu");

	exit.setFont(font);
	exit.setCharacterSize(20);
	exit.setFillColor(sf::Color::Green);
	exit.setPosition(600, 375);
	exit.setScale(back_scale);
	exit.setString("ESC -> Exit the game");

	GameOver.setFont(font);
	GameOver.setCharacterSize(15);
	GameOver.setFillColor(sf::Color::Red);
	GameOver.setPosition(225, 50);
	sf::Vector2<float> over_scale(5.5f, 5.5f);
	GameOver.setScale(over_scale);
	GameOver.setString("GameOver");

	Score.setFont(font);
	Score.setCharacterSize(15);
	Score.setFillColor(sf::Color::Red);
	Score.setPosition(230, 250);
	sf::Vector2<float> score_scale(3.5f, 3.5f);
	Score.setScale(score_scale);
	Score.setString("YOUR SCORE : ");

	hightscore.setFont(font);
	hightscore.setCharacterSize(15);
	hightscore.setFillColor(sf::Color::Red);
	hightscore.setPosition(150, 460);
	sf::Vector2<float> hightscore_scale(2.0f, 2.0f);
	hightscore.setScale(hightscore_scale);
	hightscore.setString("Leader Board");

	Rank.setFont(font);
	Rank.setCharacterSize(20);
	Rank.setFillColor(sf::Color::Black);
	Rank.setPosition(200, 550);
	sf::Vector2<float> Rank_scale(2.0f, 2.0f);
	Rank.setScale(Rank_scale);
	Rank.setString("Rank");

	s.setFont(font);
	s.setCharacterSize(20);
	s.setFillColor(sf::Color::Black);
	s.setPosition(500, 550);
	s.setScale(Rank_scale);
	s.setString("Score");

	hightscore1.setFont(font);
	hightscore1.setCharacterSize(20);
	hightscore1.setFillColor(sf::Color::Black);
	hightscore1.setPosition(500, 620);
	sf::Vector2<float> hightscore1_scale(2.0f, 2.0f);
	hightscore1.setScale(hightscore1_scale);
	hightscore1.setString(" ");

	r1.setFont(font);
	r1.setCharacterSize(20);
	r1.setFillColor(sf::Color::Black);
	r1.setPosition(220, 620);
	r1.setScale(hightscore1_scale);
	r1.setString("(1)");

	hightscore2.setFont(font);
	hightscore2.setCharacterSize(20);
	hightscore2.setFillColor(sf::Color::Black);
	hightscore2.setPosition(500, 690);
	sf::Vector2<float> hightscore2_scale(2.0f, 2.0f);
	hightscore2.setScale(hightscore2_scale);
	hightscore2.setString(" ");

	r2.setFont(font);
	r2.setCharacterSize(20);
	r2.setFillColor(sf::Color::Black);
	r2.setPosition(220, 690);
	r2.setScale(hightscore1_scale);
	r2.setString("(2)");

	hightscore3.setFont(font);
	hightscore3.setCharacterSize(20);
	hightscore3.setFillColor(sf::Color::Black);
	hightscore3.setPosition(500, 760);
	sf::Vector2<float> hightscore3_scale(2.0f, 2.0f);
	hightscore3.setScale(hightscore3_scale);
	hightscore3.setString(" ");

	r3.setFont(font);
	r3.setCharacterSize(20);
	r3.setFillColor(sf::Color::Black);
	r3.setPosition(220, 760);
	r3.setScale(hightscore1_scale);
	r3.setString("(3)");

	hightscore4.setFont(font);
	hightscore4.setCharacterSize(20);
	hightscore4.setFillColor(sf::Color::Black);
	hightscore4.setPosition(500, 830);
	hightscore4.setScale(hightscore3_scale);
	hightscore4.setString(" ");

	r4.setFont(font);
	r4.setCharacterSize(20);
	r4.setFillColor(sf::Color::Black);
	r4.setPosition(220, 830);
	r4.setScale(hightscore1_scale);
	r4.setString("(4)");

	hightscore5.setFont(font);
	hightscore5.setCharacterSize(20);
	hightscore5.setFillColor(sf::Color::Black);
	hightscore5.setPosition(500, 900);
	hightscore5.setScale(hightscore3_scale);
	hightscore5.setString(" ");

	r5.setFont(font);
	r5.setCharacterSize(20);
	r5.setFillColor(sf::Color::Black);
	r5.setPosition(220, 900);
	r5.setScale(hightscore1_scale);
	r5.setString("(5)");





	int i, j,d;
	
	std::ifstream readFile;
	readFile.open("Highscore.txt");
	if (readFile.is_open())
	{
		while(!readFile.eof())
		{
			for (i = 0; i < 6; i++) {
				readFile >> score[i];
			}
		
		}
	}

	readFile.close();
	
	score[5] = score_input;

	

	for (i = 0; i < 6; i++) {
		for (j = i + 1; j < 6; j++) {
			if (score[i] < score[j]) {
				d = score[i];
				score[i] = score[j];
				score[j] = d;
			}
		}
	}
	std::ofstream writeFile("Highscore.txt");

	if(writeFile.is_open())
	{
		for (i = 0; i < 6; i++) {
			writeFile << score[i]<<" ";
		}
	}
	writeFile.close();
	
	


	while (window.isOpen()) 
	{
		sf::Event gameover;
		while(window.pollEvent(gameover))
		{
			if (gameover.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				window.close();
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
				window.close();
				menu();
			}
			
			char temp[256];
			sprintf_s(temp, "YOUR SCORE: %i", score_input);
			Score.setString(temp);

			char temp2[256];
			sprintf_s(temp2, "%i", score[0]);
			hightscore1.setString(temp2);

			char temp3[256];
			sprintf_s(temp3, "%i", score[1]);
			hightscore2.setString(temp3);

			char temp4[256];
			sprintf_s(temp4, "%i", score[2]);
			hightscore3.setString(temp4);

			char temp5[256];
			sprintf_s(temp5, "%i", score[3]);
			hightscore4.setString(temp5);

			char temp6[256];
			sprintf_s(temp6, "%i", score[4]);
			hightscore5.setString(temp6);
			

			window.clear(sf::Color(0, 0, 0, 0));
			window.draw(BG);
			window.draw(GameOver);
			window.draw(Score);
			window.draw(Rank);
			window.draw(s);
			window.draw(r1);
			window.draw(r2);
			window.draw(r3);
			window.draw(r4);
			window.draw(r5);
			window.draw(hightscore);
			window.draw(hightscore1);
			window.draw(hightscore2);
			window.draw(hightscore3);
			window.draw(hightscore4);
			window.draw(hightscore5);
			window.draw(Back);
			window.draw(exit);
			window.display();

			

		}

	}

}

void game() {
	// sprite
	sf::Texture tile_tex;
	tile_tex.loadFromFile("images/tetris_tile.png");
	sf::Texture BG_games;
	BG_games.loadFromFile("images/BG_game.png");

	sf::Sprite BG(BG_games);
	BG.setScale(1.25, 1.25);
	

	sf::Sprite tile(tile_tex);
	tile.setScale(2.83, 2.83);

	//sound
	sf::SoundBuffer line;
	line.loadFromFile("sounds/sounds_line.wav");

	sf::Sound sound_line;
	sound_line.setBuffer(line);
	sound_line.setVolume(60);

	sf::SoundBuffer line4;
	line4.loadFromFile("sounds/sounds_tetris.wav");

	sf::Sound sound_4line;
	sound_4line.setBuffer(line4);
	sound_4line.setVolume(60);


	sf::Music music;
	music.openFromFile("sounds/Tetris.ogg");
	music.setVolume(10);
	music.play();
	music.setLoop(true);

	sf::SoundBuffer over;
	over.loadFromFile("sounds/Game Over Sound Effect.wav");
	sf::Sound gover;
	gover.setBuffer(over);
	gover.setVolume(40);
	
	float tile_size = tile_tex.getSize().x * tile.getScale().x;
	size_t width = tile_size * 12, height = tile_size * 20;



	//Setup score counter
	sf::Text score;
	sf::Font font;
	sf::Text Back;

	Back.setFont(font);
	Back.setCharacterSize(20);
	Back.setFillColor(sf::Color::Green);
	Back.setPosition(650, 800);
	sf::Vector2<float>back_scale(1.0f, 1.0f);
	Back.setScale(back_scale);
	Back.setString("Enter -> Surrender");

	font.loadFromFile("Forwa_font.TTF");
	score.setFont(font);
	score.setCharacterSize(15);
	score.setFillColor(sf::Color::Green);
	score.setPosition(650, 550);
	sf::Vector2<float> score_scale(2.5f, 2.5f);
	score.setScale(score_scale);
	score.setString("SCORE: 0");

	sf::Text level;
	level.setFont(font);
	level.setCharacterSize(15);
	level.setFillColor(sf::Color::Green);
	level.setPosition(650, 650);
	sf::Vector2<float> level_scale(2.5f, 2.5f);
	level.setScale(level_scale);
	level.setString("LEVEL : 1");

	sf::Text Next;
	Next.setFont(font);
	Next.setCharacterSize(15);
	Next.setFillColor(sf::Color::Green);
	Next.setPosition(650, 150);
	sf::Vector2<float> next_scale(1.5f, 1.5f);
	Next.setScale(next_scale);
	Next.setString("NEXT");

	//Setup window & create first piece
	sf::RenderWindow window(sf::VideoMode(960, 960), "Tetris", sf::Style::Titlebar | sf::Style::Close);
	window.setKeyRepeatEnabled(true);
	window.setFramerateLimit(60);
	srand(time(NULL));
	int type;

	type = static_cast<PIECE_TYPE>((rand() % 7));
	
	
	Piece2 piece2 = CreatePiece2(static_cast<PIECE_TYPE>(type));
	Piece piece = CreatePiece(static_cast<PIECE_TYPE>((rand() % 7)));


	unsigned int timer = 0, gamespeed = 10, scoreCounter = 0, speedlevel = 10;

	while (window.isOpen())
	{
		srand(time(NULL));

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				window.close();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
				window.close();
				music.stop();
				gover.play();
				gameover(scoreCounter);
			}
			if (event.type == sf::Event::KeyPressed)
			{
				//Movement
				if (event.key.code == sf::Keyboard::Up)
				{
					rotate(piece);
				}
				else if (event.key.code == sf::Keyboard::Left &&
					piece.a.x != 0 && piece.b.x != 0 && piece.c.x != 0 && piece.d.x != 0 &&
					(colliders[piece.a.y][piece.a.x - 1]) != 2 && (colliders[piece.b.y][piece.b.x - 1]) != 2 &&
					(colliders[piece.c.y][piece.c.x - 1]) != 2 && (colliders[piece.d.y][piece.d.x - 1]) != 2)
				{
					piece.a.x--;
					piece.b.x--;
					piece.c.x--;
					piece.d.x--;
				}
				else if (event.key.code == sf::Keyboard::Right &&
					piece.a.x != 11 && piece.b.x != 11 && piece.c.x != 11 && piece.d.x != 11 &&
					(colliders[piece.a.y][piece.a.x + 1]) != 2 && (colliders[piece.b.y][piece.b.x + 1]) != 2 &&
					(colliders[piece.c.y][piece.c.x + 1]) != 2 && (colliders[piece.d.y][piece.d.x + 1]) != 2)
				{
					piece.a.x++;
					piece.b.x++;
					piece.c.x++;
					piece.d.x++;
				}
			}
			if (event.type == sf::Event::KeyReleased)
			{
				gamespeed = speedlevel;

			}

			if (scoreCounter >= 30 && scoreCounter < 50) {
				speedlevel = 6;
				level.setString("LEVEL : 2");
			}
			else if (scoreCounter >= 50) {
				speedlevel = 4;
				level.setString("LEVEL : 3");
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
		{
			gamespeed = 1;
		}

		window.clear(sf::Color(0, 0, 0, 0));
		window.draw(BG);


		//Draw the current falling piece
		sf::Sprite piece_tile = tile;

		piece_tile.setPosition(tile_size * piece.a.x, tile_size * piece.a.y);
		window.draw(piece_tile);

		piece_tile.setPosition(tile_size * piece.b.x, tile_size * piece.b.y);
		window.draw(piece_tile);

		piece_tile.setPosition(tile_size * piece.c.x, tile_size * piece.c.y);
		window.draw(piece_tile);

		piece_tile.setPosition(tile_size * piece.d.x, tile_size * piece.d.y);
		window.draw(piece_tile);

		//Refresh the grid-array
		for (size_t i = 0; i < 20; i++)
		{
			for (size_t j = 0; j < 12; j++)
			{
				if (colliders[i][j] == 2)
				{
					grid[i][j] = 2;
				}
				else
				{
					grid[i][j] = 0;
				}
			}
		}

		//Clock
		if (timer > gamespeed)
		{
			//Collission checks
			if (grid[piece.a.y + 1][piece.a.x] == 2 ||
				grid[piece.b.y + 1][piece.b.x] == 2 ||
				grid[piece.c.y + 1][piece.c.x] == 2 ||
				grid[piece.d.y + 1][piece.d.x] == 2 ||
				piece.a.y == 19 || piece.b.y == 19 || piece.c.y == 19 || piece.d.y == 19
				)
			{


				grid[piece.a.y][piece.a.x] = 2;
				grid[piece.b.y][piece.b.x] = 2;
				grid[piece.c.y][piece.c.x] = 2;
				grid[piece.d.y][piece.d.x] = 2;

				colliders[piece.a.y][piece.a.x] = 2;
				colliders[piece.b.y][piece.b.x] = 2;
				colliders[piece.c.y][piece.c.x] = 2;
				colliders[piece.d.y][piece.d.x] = 2;

				piece = CreatePiece(static_cast<PIECE_TYPE>(type));

				type = static_cast<PIECE_TYPE>((rand() % 7));


				piece2 = CreatePiece2(static_cast<PIECE_TYPE>(type));

				

				scoreCounter++;
				char temp[256];
				sprintf_s(temp, "SCORE: %i", scoreCounter);
				score.setString(temp);
			}
			else
			{
				grid[piece.a.y + 1][piece.a.x] = 1;
				grid[piece.b.y + 1][piece.b.x] = 1;
				grid[piece.c.y + 1][piece.c.x] = 1;
				grid[piece.d.y + 1][piece.d.x] = 1;

				piece.a.y++;
				piece.b.y++;
				piece.c.y++;
				piece.d.y++;
			}

			//Check if the player has a line
			byte tetris_row = 0;
			for (size_t i = 0; i < 20; i++)
			{
				byte blocks_in_a_row = 0;
				for (size_t j = 0; j < 12; j++)
				{
					if (colliders[i][j] == 2)
					{
						blocks_in_a_row++;
					}
				}
				if (blocks_in_a_row == 12)
				{
					tetris_row++;
					if (tetris_row >= 4)
					{
						sound_4line.play();
					}
					else
					{
						sound_line.play();
					}
					for (size_t k = i; k > 0; k--)
					{
						for (size_t l = 0; l < 12; l++)
						{
							colliders[k][l] = colliders[k - 1][l];
						}
					}
					scoreCounter += 10;
					char temp[256];
					sprintf_s(temp, "SCORE: %i", scoreCounter);
					score.setString(temp);
				}
			}

			//If game over
			for (size_t i = 0; i < 12; i++)
			{
				if (colliders[0][i] == 2)
				{
					music.stop();
					gover.play();
					window.close();
					gameover(scoreCounter);
				}
			}

			timer = 0;


		}
		else
		{
			timer++;
		}

		//Draws all the tiles
		for (size_t i = 0; i < 20; i++)
		{
			for (size_t j = 0; j < 12; j++)
			{
				if (grid[i][j] == 1)
				{
					sf::Sprite t = tile;
					t.setPosition(tile_size * j, tile_size * i);
					window.draw(t);
				}
				if (colliders[i][j] == 2)
				{
					sf::Sprite t = tile;
					t.setPosition(tile_size * j, tile_size * i);
					window.draw(t);
				}
			}
		}

		//Draw the score and update the window

		sf::Sprite piece2_tile = tile;

		piece2_tile.setPosition(tile_size* piece2.a.x + 530, tile_size* piece2.a.y + 250);
		window.draw(piece2_tile);

		piece2_tile.setPosition(tile_size* piece2.b.x + 530, tile_size* piece2.b.y + 250);
		window.draw(piece2_tile);

		piece2_tile.setPosition(tile_size* piece2.c.x + 530, tile_size* piece2.c.y + 250);
		window.draw(piece2_tile);

		piece2_tile.setPosition(tile_size* piece2.d.x + 530, tile_size* piece2.d.y + 250);
		window.draw(piece2_tile);


		window.draw(Next);
		window.draw(score);
		window.draw(level);
		window.draw(Back);
		window.display();
	}
}

void score_board() {
	sf::RenderWindow window(sf::VideoMode(960, 960), "TETRIS THE GAME!!", sf::Style::Titlebar | sf::Style::Close);
	window.setKeyRepeatEnabled(true);
	window.setFramerateLimit(60);

	int score[6];

	sf::Texture BG_score;
	BG_score.loadFromFile("images/BG_score.png");

	sf::Sprite BG(BG_score);
	BG.setScale(1.25, 1.25);

	sf::Text hightscore;
	sf::Text hightscore1;
	sf::Text hightscore2;
	sf::Text hightscore3;
	sf::Text hightscore4;
	sf::Text hightscore5;
	sf::Text r1;
	sf::Text r2;
	sf::Text r3;
	sf::Text r4;
	sf::Text r5;
	sf::Text Rank;
	sf::Text s;
	sf::Text score_board;
	sf::Text Back;
	sf::Text exit;

	sf::Font font;
	font.loadFromFile("Forwa_font.TTF");

	Back.setFont(font);
	Back.setCharacterSize(20);
	Back.setFillColor(sf::Color::Green);
	Back.setPosition(100, 900);
	sf::Vector2<float>back_scale(1.0f, 1.0f);
	Back.setScale(back_scale);
	Back.setString("Enter -> Back to menu");

	exit.setFont(font);
	exit.setCharacterSize(20);
	exit.setFillColor(sf::Color::Green);
	exit.setPosition(600, 900);
	exit.setScale(back_scale);
	exit.setString("ESC -> Exit the game");

	score_board.setFont(font);
	score_board.setCharacterSize(35);
	score_board.setFillColor(sf::Color::White);
	score_board.setPosition(210, 100);
	sf::Vector2<float> score_scale(2.0f, 2.0f);
	score_board.setScale(score_scale);
	score_board.setString("Score Board");

	Rank.setFont(font);
	Rank.setCharacterSize(20);
	Rank.setFillColor(sf::Color::Black);
	Rank.setPosition(130, 335);
	sf::Vector2<float> Rank_scale(2.0f, 2.0f);
	Rank.setScale(Rank_scale);
	Rank.setString("Rank");

	s.setFont(font);
	s.setCharacterSize(20);
	s.setFillColor(sf::Color::Black);
	s.setPosition(500, 335);
	s.setScale(Rank_scale);
	s.setString("Score");

	hightscore1.setFont(font);
	hightscore1.setCharacterSize(20);
	hightscore1.setFillColor(sf::Color::Black);
	hightscore1.setPosition(540, 425);
	sf::Vector2<float> hightscore1_scale(2.0f, 2.0f);
	hightscore1.setScale(hightscore1_scale);
	hightscore1.setString(" ");

	r1.setFont(font);
	r1.setCharacterSize(20);
	r1.setFillColor(sf::Color::Black);
	r1.setPosition(150, 425);
	r1.setScale(hightscore1_scale);
	r1.setString("(1)");

	hightscore2.setFont(font);
	hightscore2.setCharacterSize(20);
	hightscore2.setFillColor(sf::Color::Black);
	hightscore2.setPosition(540, 505);
	sf::Vector2<float> hightscore2_scale(2.0f, 2.0f);
	hightscore2.setScale(hightscore2_scale);
	hightscore2.setString(" ");

	r2.setFont(font);
	r2.setCharacterSize(20);
	r2.setFillColor(sf::Color::Black);
	r2.setPosition(150, 505);
	r2.setScale(hightscore1_scale);
	r2.setString("(2)");

	hightscore3.setFont(font);
	hightscore3.setCharacterSize(20);
	hightscore3.setFillColor(sf::Color::Black);
	hightscore3.setPosition(540, 595);
	sf::Vector2<float> hightscore3_scale(2.0f, 2.0f);
	hightscore3.setScale(hightscore3_scale);
	hightscore3.setString(" ");

	r3.setFont(font);
	r3.setCharacterSize(20);
	r3.setFillColor(sf::Color::Black);
	r3.setPosition(150, 595);
	r3.setScale(hightscore1_scale);
	r3.setString("(3)");

	hightscore4.setFont(font);
	hightscore4.setCharacterSize(20);
	hightscore4.setFillColor(sf::Color::Black);
	hightscore4.setPosition(540, 675);
	hightscore4.setScale(hightscore3_scale);
	hightscore4.setString(" ");

	r4.setFont(font);
	r4.setCharacterSize(20);
	r4.setFillColor(sf::Color::Black);
	r4.setPosition(150, 675);
	r4.setScale(hightscore1_scale);
	r4.setString("(4)");

	hightscore5.setFont(font);
	hightscore5.setCharacterSize(20);
	hightscore5.setFillColor(sf::Color::Black);
	hightscore5.setPosition(540, 765);
	hightscore5.setScale(hightscore3_scale);
	hightscore5.setString(" ");

	r5.setFont(font);
	r5.setCharacterSize(20);
	r5.setFillColor(sf::Color::Black);
	r5.setPosition(150, 765);
	r5.setScale(hightscore1_scale);
	r5.setString("(5)");





	int i, j, d;

	std::ifstream readFile;
	readFile.open("Highscore.txt");
	if (readFile.is_open())
	{
		while (!readFile.eof())
		{
			for (i = 0; i < 6; i++) {
				readFile >> score[i];
			}

		}
	}

	readFile.close();



	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				window.close();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
				window.close();
				menu();
			}

			char temp2[256];
			sprintf_s(temp2, "%i", score[0]);
			hightscore1.setString(temp2);

			char temp3[256];
			sprintf_s(temp3, "%i", score[1]);
			hightscore2.setString(temp3);

			char temp4[256];
			sprintf_s(temp4, "%i", score[2]);
			hightscore3.setString(temp4);

			char temp5[256];
			sprintf_s(temp5, "%i", score[3]);
			hightscore4.setString(temp5);

			char temp6[256];
			sprintf_s(temp6, "%i", score[4]);
			hightscore5.setString(temp6);

		}

		
		window.clear();
		window.draw(BG);
		window.draw(score_board);
		window.draw(Rank);
		window.draw(s);
		window.draw(r1);
		window.draw(r2);
		window.draw(r3);
		window.draw(r4);
		window.draw(r5);
		window.draw(hightscore);
		window.draw(hightscore1);
		window.draw(hightscore2);
		window.draw(hightscore3);
		window.draw(hightscore4);
		window.draw(hightscore5);
		window.draw(Back);
		window.draw(exit);
		window.display();
	}
}


void menu() {
	sf::RenderWindow window(sf::VideoMode(960, 960), "TETRIS THE GAME!!", sf::Style::Titlebar | sf::Style::Close);
	window.setKeyRepeatEnabled(true);
	window.setFramerateLimit(60);

	sf::Texture BG_menu;
	BG_menu.loadFromFile("images/BG_menu.png");

	sf::Sprite BG(BG_menu);
	BG.setScale(1.25, 1.25);

	sf::Font font;
	font.loadFromFile("Forwa_font.TTF");

	sf::Text Start("START",font,60);
	sf::Text Leaderboard("Leader Board", font, 60);
	sf::Text Exit("EXIT", font, 60);
	sf::Text t389("389", font, 40);
	
	t389.setPosition(700, 350);
	Start.setPosition(250, 450);
	Leaderboard.setPosition(250, 600);
	Exit.setPosition(250, 750);

	while (window.isOpen()) {
		sf::Event menu;
		while (window.pollEvent(menu)) {
			if (menu.type == sf::Event::Closed|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
				window.close();
			}
			
			int n = sf::Mouse::getPosition(window).x;

			if (Start.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
				Start.setFillColor(sf::Color::Red);
				Start.setStyle(sf::Text::Underlined);
			}
			else if (Leaderboard.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
				Leaderboard.setFillColor(sf::Color::Red);
				Leaderboard.setStyle(sf::Text::Underlined);
			}
			else if (Exit.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
				Exit.setFillColor(sf::Color::Red);
				Exit.setStyle(sf::Text::Underlined);
			}
			else {
				Start.setFillColor(sf::Color::White);
				Start.setStyle(sf::Text::Regular);
				Leaderboard.setFillColor(sf::Color::White);
				Leaderboard.setStyle(sf::Text::Regular);
				Exit.setFillColor(sf::Color::White);
				Exit.setStyle(sf::Text::Regular);
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				if (Start.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
					window.close();
					game();
				}
				else if (Leaderboard.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
					window.close();
					score_board();
				}
				else if (Exit.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y)) {
					window.close();
				}
			}

			window.clear();
			window.draw(BG);
			window.draw(Start);
			window.draw(Leaderboard);
			window.draw(Exit);
			window.draw(t389);
			window.display();

		}
	}
}

int main()
{
	menu();

	return 0;
}