#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <Windows.h>

using namespace std;

#define ROWS 8
#define COLS 8    
#define TOTAL_PIECES 32
#define BOARD_SIZE 64
#define WHITE_PIECES 16
#define BLACK_PIECES 16
#define TOTAL_WHITE_KNIGHTS 2
#define TOTAL_BLACK_KNIGHTS 2
#define TOTAL_WHITE_BISHOPS 2
#define TOTAL_BLACK_BISHOPS 2
#define TOTAL_WHITE_ROOKS 2
#define TOTAL_BLACK_ROOKS 2
#define TOTAL_WHITE_PAWNS 8
#define TOTAL_BLACK_PAWNS 8
#define TOTAL_WHITE_QUEENS 1
#define TOTAL_BLACK_QUEENS 1
#define TOTAL_WHITE_KINGS 1
#define TOTAL_BLACK_KINGS 1

enum Pieces {
	WHITE_KING,
	WHITE_QUEEN,
	WHITE_ROOK,
	WHITE_BISHOP,
	WHITE_KNIGHT,
	WHITE_PAWN,
	BLACK_KING,
	BLACK_QUEEN,
	BLACK_ROOK,
	BLACK_BISHOP,
	BLACK_KNIGHT,
	BLACK_PAWN,
	EMPTY
};

enum Pieces boardArray[ROWS][COLS] = {
	{BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK },
	{BLACK_PAWN, BLACK_PAWN  , BLACK_PAWN  , BLACK_PAWN , BLACK_PAWN, BLACK_PAWN  , BLACK_PAWN ,  BLACK_PAWN, },
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
	{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,EMPTY , EMPTY, EMPTY},
	{WHITE_PAWN, WHITE_PAWN  , WHITE_PAWN  , WHITE_PAWN , WHITE_PAWN, WHITE_PAWN  , WHITE_PAWN ,  WHITE_PAWN, },
	{WHITE_ROOK, WHITE_KNIGHT,WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK} };



vector<string> boardState;
bool whiteKingCanCastleLeft = false;
bool whiteKingCanCastleRight = false;
bool blackKingCanCastleLeft = false;
bool blackKingCanCastleRight = false;
bool enPassantPossible = false;


bool movesArray[ROWS][COLS] = { false };
bool resolveCheckArray[ROWS][COLS] = { false };
bool inCheck[2] = { false,false };

int lastPlayedMove[4] = { -1,-1,-1,-1 };

int fiftyCount = 0;
int fullMoveCount = 0;

bool turn = true;

bool whiteKingMoved = false;
bool blackKingMoved = false;
bool rightWhiteRookMoved = false;
bool leftWhiteRookMoved = false;
bool rightBlackRookMoved = false;
bool leftBlackRookMoved = false;
int enPassCoord[2] = { -1,-1 };
static vector<pair<int, int>> attackerSquares;
static vector<pair<int, int>> blockingSquares;

bool isSelected = false;
int selectedRow, selectedCol;

sf::Texture title;
sf::Texture whiteKing;
sf::Texture whiteQueen;
sf::Texture whiteRook;
sf::Texture whiteBishop;
sf::Texture whiteKnight;
sf::Texture whitePawn;
sf::Texture blackKing;
sf::Texture blackQueen;
sf::Texture blackRook;
sf::Texture blackBishop;
sf::Texture blackKnight;
sf::Texture blackPawn;
sf::Texture board;
sf::Texture moveIndicator;
sf::Texture checkIndicator;

sf::Font font;
pair<int, int> getKingPos(enum Pieces king);

bool isMoveLegal(int fromRow, int fromCol, int toRow, int toCol, enum Pieces piece, enum Pieces king);

void renderGame(sf::RenderWindow &window) {
	
		window.clear(sf::Color::White);
		sf::Sprite boardSprite(board);
		window.draw(boardSprite);

		


		for (int i = 0; i < ROWS; ++i) {
			for (int j = 0; j < COLS; ++j) {
				if (boardArray[i][j] != EMPTY) {
					sf::Sprite piece;
					switch (boardArray[i][j]) {
					case WHITE_KING: piece.setTexture(whiteKing); break;
					case WHITE_QUEEN: piece.setTexture(whiteQueen); break;
					case WHITE_ROOK: piece.setTexture(whiteRook); break;
					case WHITE_BISHOP: piece.setTexture(whiteBishop); break;
					case WHITE_KNIGHT: piece.setTexture(whiteKnight); break;
					case WHITE_PAWN: piece.setTexture(whitePawn); break;
					case BLACK_KING: piece.setTexture(blackKing); break;
					case BLACK_QUEEN: piece.setTexture(blackQueen); break;
					case BLACK_ROOK: piece.setTexture(blackRook); break;
					case BLACK_BISHOP: piece.setTexture(blackBishop); break;
					case BLACK_KNIGHT: piece.setTexture(blackKnight); break;
					case BLACK_PAWN: piece.setTexture(blackPawn); break;
					default: continue;
					}
					piece.setPosition(j * 123.0f, i * 123.0f);
					window.draw(piece);
				}
				if (inCheck[turn ? 0 : 1]) {

					if (resolveCheckArray[i][j]) {
						sf::Sprite indicator(moveIndicator);
						indicator.setPosition(j * 123.0f, i * 123.0f);
						indicator.setColor(sf::Color(0, 255, 0, 100));
						window.draw(indicator);
					}
				}
				else {
					if (movesArray[i][j]) {
						sf::Sprite indicator(moveIndicator);
						indicator.setPosition(j * 123.0f, i * 123.0f);
						indicator.setColor(sf::Color(0, 255, 0, 100));
						window.draw(indicator);
					}
				}
			}
		}

		if (inCheck[0] || inCheck[1]) {
			pair<int, int> kingPos = getKingPos(turn ? WHITE_KING : BLACK_KING);
			sf::Sprite checkIndicatorSprite(checkIndicator);
			checkIndicatorSprite.setPosition(kingPos.second * 123.0f, kingPos.first * 123.0f);
			checkIndicatorSprite.setColor(sf::Color(255, 255, 0, 100));
			window.draw(checkIndicatorSprite);
		}
				
}

bool takeAblePieces(int row, int col) {
	vector<enum Pieces> takingPieces;

	if (!turn) {//black turn
		takingPieces.push_back(WHITE_KING);
		takingPieces.push_back(WHITE_QUEEN);
		takingPieces.push_back(WHITE_ROOK);
		takingPieces.push_back(WHITE_BISHOP);
		takingPieces.push_back(WHITE_KNIGHT);
		takingPieces.push_back(WHITE_PAWN);
	}
	else {//white turn

		takingPieces.push_back(BLACK_KING);
		takingPieces.push_back(BLACK_QUEEN);
		takingPieces.push_back(BLACK_ROOK);
		takingPieces.push_back(BLACK_BISHOP);
		takingPieces.push_back(BLACK_KNIGHT);
		takingPieces.push_back(BLACK_PAWN);
	}

	for (int i = 0; i < takingPieces.size(); i++) {
		if (boardArray[row][col] == takingPieces[i]) {
			return true;
		}
	}
	return false;

}


void checkMateScreen(sf::RenderWindow& window) {



	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2,text3;
	text.setFont(font);
	text.setString("CHECKMATE");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(360, 400);

	text2.setFont(font);

	if (turn) {
		text2.setString("BLACK WINS");

	}
	else {
		text2.setString("WHITE WINS");

	}
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(360, 500);

	text3.setFont(font);
	text3.setString("PRESS ENTER TO CONTINUE");
	text3.setCharacterSize(35);
	text3.setFillColor(sf::Color(131, 156, 143));
	text3.setPosition(210, 700);

	while (true) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.draw(text3);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Enter:
					return;
				default:
					break;
				}
			}
		}
	}
}



void staleMateScreen(sf::RenderWindow& window) {


	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2;
	text.setFont(font);
	text.setString("DRAW BY STALEMATE");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(250, 400);


	text2.setFont(font);
	text2.setString("PRESS ENTER TO CONTINUE");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(210, 700);

	while (true) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Enter:
					return;
				default:
					break;
				}
			}
		}
	}
}


void threeFoldScreen(sf::RenderWindow& window) {


	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2;
	text.setFont(font);
	text.setString("DRAW BY THREE FOLD REPETITION");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(180, 400);


	text2.setFont(font);
	text2.setString("PRESS ENTER TO CONTINUE");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(210, 700);

	while (true) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Enter:
					return;
				default:
					break;
				}
			}
		}
	}
}

void fiftyMoveScreen(sf::RenderWindow & window) {

	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2;
	text.setFont(font);
	text.setString("DRAW BY FIFTY MOVE RULE");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(220, 400);


	text2.setFont(font);
	text2.setString("PRESS ENTER TO CONTINUE");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(210, 700);

	while (true) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Enter:
					return;
				default:
					break;
				}
			}
		}
	}
}



void resignScreen(sf::RenderWindow& window) {


	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text2, text3;

	text2.setFont(font);

	if (turn) {
		text2.setString("BLACK WINS");

	}
	else {
		text2.setString("WHITE WINS");

	}
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(360, 500);

	text3.setFont(font);
	text3.setString("PRESS ENTER TO CONTINUE");
	text3.setCharacterSize(35);
	text3.setFillColor(sf::Color(131, 156, 143));
	text3.setPosition(210, 700);

	while (true) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text2);
		window.draw(text3);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Enter:
					return;
				default:
					break;
				}
			}
		}
	}
}

bool pauseScreen(sf::RenderWindow & window) {
	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2,text3;
	text.setFont(font);
	text.setString("PRESS R TO RESIGN");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(240, 500);


	text2.setFont(font);
	text2.setString("PRESS ENTER TO CONTINUE");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(210, 600);


	text3.setFont(font);
	text3.setString("PRESS ESC TO RETURN TO MAIN MENU");
	text3.setCharacterSize(35);
	text3.setFillColor(sf::Color(131, 156, 143));
	text3.setPosition(140, 700);

	while (true) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.draw(text3);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Enter:
					return true;
				case sf::Keyboard::R:
					resignScreen(window);
					return false;
				case sf::Keyboard::Escape:
					return false;
				default:
					break;
				}
			}
		}
	}

}

void Draw(sf::RenderWindow & window) {

	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2;
	text.setFont(font);
	text.setString("DRAW BY INSUFFICIENT MATERIAL\n\n\t\t/MUTUAL AGREEMENT");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(120, 400);


	text2.setFont(font);
	text2.setString("PRESS ENTER TO CONTINUE");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(210, 700);

	while (true) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Enter:
					return;
				default:
					break;
				}
			}
		}
	}
}

enum Pieces pawnPromotion(sf::RenderWindow &window) {

	sf::Sprite queenSprite;
	sf::Sprite bishopSprite;
	sf::Sprite knightSprite;
	sf::Sprite rookSprite;

	if (turn) {

		queenSprite = sf::Sprite(whiteQueen);
		bishopSprite = sf::Sprite(whiteBishop);
		knightSprite = sf::Sprite(whiteKnight);
		rookSprite = sf::Sprite(whiteRook);

	}
	else {
		queenSprite = sf::Sprite(blackQueen);
		bishopSprite = sf::Sprite(blackBishop);
		knightSprite = sf::Sprite(blackKnight);
		rookSprite = sf::Sprite(blackRook);
	}
	
	sf::Text text,text2,text3,text4,text5;
	text.setFont(font);
	text.setString("CHOOSE A PIECE FOR PROMOTION:");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(100, 20);

	text2.setFont(font);
	text2.setString("PRESS 0 FOR QUEEN PROMOTION:");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(100, 100);

	queenSprite.setPosition(400, 200);

	text3.setFont(font);
	text3.setString("PRESS 1 FOR BISHOP PROMOTION:");
	text3.setCharacterSize(35);
	text3.setFillColor(sf::Color(131, 156, 143));
	text3.setPosition(100, 300);

	bishopSprite.setPosition(400, 400);

	text4.setFont(font);
	text4.setString("PRESS 2 FOR KNIGHT PROMOTION:");
	text4.setCharacterSize(35);
	text4.setFillColor(sf::Color(131, 156, 143));
	text4.setPosition(100, 500);

	knightSprite.setPosition(400, 600);

	text5.setFont(font);
	text5.setString("PRESS 3 FOR ROOK PROMOTION:");
	text5.setCharacterSize(35);
	text5.setFillColor(sf::Color(131, 156, 143));
	text5.setPosition(100, 700);

	rookSprite.setPosition(400, 800);


	sf::Event event;
	while (true) {
		window.clear(sf::Color(29, 59, 52));
		window.draw(text);
		window.draw(text2);
		window.draw(text3);
		window.draw(text4);
		window.draw(text5);
		window.draw(queenSprite);
		window.draw(bishopSprite);
		window.draw(knightSprite);
		window.draw(rookSprite);

		window.display();


		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Num0:
					return turn ? WHITE_QUEEN : BLACK_QUEEN;
				case sf::Keyboard::Num1:
					return turn ? WHITE_BISHOP : BLACK_BISHOP;
				case sf::Keyboard::Num2:
					return turn ? WHITE_KNIGHT : BLACK_KNIGHT;
				case sf::Keyboard::Num3:
					return turn ? WHITE_ROOK : BLACK_ROOK;
				default:
					break;
				}
			}
		}
	}

}


void kingMove(int row, int col) {
	

	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			if (i == 0 && j == 0) continue;
			int r = row + i;
			int c = col + j;
			if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
				if (boardArray[r][c] == EMPTY || takeAblePieces(r, c)) {
					movesArray[r][c] = true;
				}
			}
		}
	}
	bool isWhite = (boardArray[row][col] == WHITE_KING);
	bool isInCheck = isWhite ? inCheck[0] : inCheck[1];

	if (isInCheck) return;

	bool temp[ROWS][COLS];

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			temp[i][j] = movesArray[i][j];
		}
	}

	if (isWhite && !whiteKingMoved) {
		// Kingside
		if (!rightWhiteRookMoved && boardArray[7][5] == EMPTY && boardArray[7][6] == EMPTY) {
			if (isMoveLegal(7, 4, 7, 5, WHITE_KING, WHITE_KING) && isMoveLegal(7, 4, 7, 6, WHITE_KING, WHITE_KING)) {
				temp[7][6] = true;

				whiteKingCanCastleRight = true;
			}
		}
		// Queenside
		if (!leftWhiteRookMoved && boardArray[7][3] == EMPTY && boardArray[7][2] == EMPTY && boardArray[7][1] == EMPTY) {
			if (isMoveLegal(7, 4, 7, 3, WHITE_KING, WHITE_KING) && isMoveLegal(7, 4, 7, 2, WHITE_KING, WHITE_KING) && isMoveLegal(7, 4, 7, 1, WHITE_KING, WHITE_KING)) {
				temp[7][2] = true;

				whiteKingCanCastleLeft = true;
			}
		}
	}
	else if (!isWhite && !blackKingMoved) {
		// Kingside
		if (!rightBlackRookMoved && boardArray[0][5] == EMPTY && boardArray[0][6] == EMPTY) {
			if (isMoveLegal(0, 4, 0, 5, BLACK_KING, BLACK_KING) && isMoveLegal(0, 4, 0, 6, BLACK_KING, BLACK_KING)) {
				temp[0][6] = true;
				blackKingCanCastleRight = true;
			}
		}
		// Queenside
		if (!leftBlackRookMoved && boardArray[0][3] == EMPTY && boardArray[0][2] == EMPTY && boardArray[0][1] == EMPTY) {
			if (isMoveLegal(0, 4, 0, 3, BLACK_KING, BLACK_KING) && isMoveLegal(0, 4, 0, 2, BLACK_KING, BLACK_KING) && isMoveLegal(0, 4, 0, 1, BLACK_KING, BLACK_KING)) {
				temp[0][2] = true;
				blackKingCanCastleLeft = true;
			}
		}
	}


	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			movesArray[i][j] = temp[i][j];
		}
	}
}


void rookMove(int row, int col) {
	
	for (int r = row - 1; r >= 0; r--) {
		if (boardArray[r][col] == EMPTY) {
			movesArray[r][col] = true;
		}
		else {
			if (takeAblePieces(r, col)) {
				movesArray[r][col] = true;
			}
			break; 
		}
	}

	for (int r = row + 1; r < ROWS; r++) {
		if (boardArray[r][col] == EMPTY) {
			movesArray[r][col] = true;
		}
		else {
			if (takeAblePieces(r, col)) {
				movesArray[r][col] = true;
			}
			break; 
		}
	}

	for (int c = col - 1; c >= 0; c--) {
		if (boardArray[row][c] == EMPTY) {
			movesArray[row][c] = true;
		}
		else {
			if (takeAblePieces(row, c)) {
				movesArray[row][c] = true;
			}
			break; 
		}
	}

	for (int c = col + 1; c < COLS; c++) {
		if (boardArray[row][c] == EMPTY) {
			movesArray[row][c] = true;
		}
		else {
			if (takeAblePieces(row, c)) {
				movesArray[row][c] = true;
			}
			break; 
		}
	}
}


void bishopMove(int row, int col) {
	bool topLeftBlock = false;
	bool topRightBlock = false;
	bool bottomLeftBlock = false;
	bool bottomRightBlock = false;

	for (int i = 1; i < ROWS; i++) {
		int topLeftRow = row - i;
		int topLeftCol = col - i;

		int topRightRow = row - i;
		int topRightCol = col + i;

		int bottomLeftRow = row + i;
		int bottomLeftCol = col - i;

		int bottomRightRow = row + i;
		int bottomRightCol = col + i;

		if (!topLeftBlock && topLeftRow >= 0 && topLeftCol >= 0) {
			if (boardArray[topLeftRow][topLeftCol] == EMPTY) {
				movesArray[topLeftRow][topLeftCol] = true;
			}
			else if (takeAblePieces(topLeftRow, topLeftCol)) {
				movesArray[topLeftRow][topLeftCol] = true;
				topLeftBlock = true;
			}
			else {
				topLeftBlock = true;
			}
		}

		if (!topRightBlock && topRightRow >= 0 && topRightCol < COLS) {
			if (boardArray[topRightRow][topRightCol] == EMPTY) {
				movesArray[topRightRow][topRightCol] = true;
			}
			else if (takeAblePieces(topRightRow, topRightCol)) {
				movesArray[topRightRow][topRightCol] = true;
				topRightBlock = true;
			}
			else {
				topRightBlock = true;
			}
		}

		if (!bottomLeftBlock && bottomLeftRow < ROWS && bottomLeftCol >= 0) {
			if (boardArray[bottomLeftRow][bottomLeftCol] == EMPTY) {
				movesArray[bottomLeftRow][bottomLeftCol] = true;
			}
			else if (takeAblePieces(bottomLeftRow, bottomLeftCol)) {
				movesArray[bottomLeftRow][bottomLeftCol] = true;
				bottomLeftBlock = true;
			}
			else {
				bottomLeftBlock = true;
			}
		}

		if (!bottomRightBlock && bottomRightRow < ROWS && bottomRightCol < COLS) {
			if (boardArray[bottomRightRow][bottomRightCol] == EMPTY) {
				movesArray[bottomRightRow][bottomRightCol] = true;
			}
			else if (takeAblePieces(bottomRightRow, bottomRightCol)) {
				movesArray[bottomRightRow][bottomRightCol] = true;
				bottomRightBlock = true;
			}
			else {
				bottomRightBlock = true;
			}
		}
	}
}



void queenMove(int row, int col) {
	bishopMove(row, col);
	rookMove(row, col);
}
void knightMove(int row, int col) {
	
	int knightMoves[8][2] = {
		{-2, -1}, {-2, 1},
		{-1, -2}, {-1, 2},
		{1, -2},  {1, 2},
		{2, -1},  {2, 1}
	};

	for (int i = 0; i < 8; ++i) {
		int r = row + knightMoves[i][0];
		int c = col + knightMoves[i][1];

		if (r >= 0 && r < ROWS && c >= 0 && c < COLS) {
			if (boardArray[r][c] == EMPTY || takeAblePieces(r, c)) {
				movesArray[r][c] = true;
			}
		}
	}
}


void tryEnPassant(int row, int col) {

	int fromR = lastPlayedMove[0];
	int fromC = lastPlayedMove[1];
	int toR = lastPlayedMove[2];
	int toC = lastPlayedMove[3];

	if (turn) {
		if (boardArray[toR][toC] != BLACK_PAWN) return;
		if (fromR != 1 || toR != 3) return;
	}
	else {
		if (boardArray[toR][toC] != WHITE_PAWN) return;
		if (fromR != 6 || toR != 4) return; 
	}

	if (toR != row) return;               
	if (abs(toC - col) != 1) return;      

	if (turn) {
		if (boardArray[2][toC] == EMPTY) {
			movesArray[2][toC] = true;

			enPassantPossible = true;
			enPassCoord[0] = 2;
			enPassCoord[1] = toC;
		}
	}
	else {
		if (boardArray[5][toC] == EMPTY) {
			movesArray[5][toC] = true;

			enPassantPossible = true;

			enPassCoord[0] = 5;
			enPassCoord[1] = toC;
		}
	}
}

void pawnMove(int row, int col) {

	if (turn) {
		if (row == 6) {//initial position

			if (boardArray[row - 1][col] == EMPTY) {
				movesArray[row - 1][col] = true;

				if (boardArray[row - 2][col] == EMPTY) {
					movesArray[row - 2][col] = true;
				}
			}

			
		}
		else {
			if (boardArray[row - 1][col] == EMPTY) {
				movesArray[row - 1][col] = true;
			}
		}

		if (col -1 >= 0 && row-1 >=0 && boardArray[row - 1][col - 1] != EMPTY && takeAblePieces(row - 1, col - 1)) {
			movesArray[row - 1][col - 1] = true;
		}
		if (col + 1 < COLS && row - 1 >= 0 && boardArray[row - 1][col + 1] != EMPTY && takeAblePieces(row - 1, col + 1)) {
			movesArray[row - 1][col + 1] = true;
		}
		
	}
	else {
		if (row == 1) {

			if (boardArray[row + 1][col] == EMPTY) {
				movesArray[row + 1][col] = true;

				if (boardArray[row + 2][col] == EMPTY) {
					movesArray[row + 2][col] = true;
				}
			}
		}
		else {
			if (boardArray[row + 1][col] == EMPTY) {
				movesArray[row + 1][col] = true;
			}
		}
		if (col + 1 < COLS && row + 1 < ROWS && boardArray[row + 1][col + 1] != EMPTY && takeAblePieces(row + 1, col + 1)) {
			movesArray[row + 1][col + 1] = true;
		}
		if (col - 1 >= 0 && row + 1 < ROWS && boardArray[row + 1][col - 1] != EMPTY && takeAblePieces(row + 1, col - 1)) {
			movesArray[row + 1][col - 1] = true;
		}
	}



	//enpassant
	tryEnPassant(row, col);


}

void resetMovesArray() {
	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLS; ++j) {
			movesArray[i][j] = false;
		}
	}
}

void resetResolveArray() {
	for (int i = 0; i < ROWS; ++i) {
		for (int j = 0; j < COLS; ++j) {
			resolveCheckArray[i][j] = false;
		}
	}
}


pair<int,int> getKingPos(enum Pieces king) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			if (boardArray[i][j] == king) {
				pair<int, int> temp = make_pair(i, j);
				return temp;
			}
		}
	}

	pair<int, int> temp = make_pair(-1, -1);
	return temp;
}

void showPossibleMoves(int row, int col, enum Pieces pieceSelected) {

	if (turn) {


		switch (pieceSelected) {
		case WHITE_KING: kingMove(row, col); break;
		case WHITE_QUEEN: queenMove(row, col); break;
		case WHITE_ROOK: rookMove(row, col); break;
		case WHITE_BISHOP: bishopMove(row, col); break;
		case WHITE_KNIGHT: knightMove(row, col); break;
		case WHITE_PAWN: pawnMove(row, col); break;
		default:return;
		}

	}
	else {

		switch (pieceSelected) {
		case BLACK_KING: kingMove(row, col); break;
		case BLACK_QUEEN: queenMove(row, col); break;
		case BLACK_ROOK: rookMove(row, col); break;
		case BLACK_BISHOP: bishopMove(row, col); break;
		case BLACK_KNIGHT: knightMove(row, col); break;
		case BLACK_PAWN: pawnMove(row, col); break;
		default:
			return;
		}
	}

	cout << "Showing possible moves for the selected piece..." << endl;
}



void buildCheckPath(int fromR, int fromC, int toR, int toC, vector<pair<int, int>>& outPath) {
	int dr = (toR > fromR) ? 1 : (toR < fromR) ? -1 : 0;
	int dc = (toC > fromC) ? 1 : (toC < fromC) ? -1 : 0;
	int r = fromR + dr, c = fromC + dc;
	while (r != toR || c != toC) {
		outPath.emplace_back(r, c);
		r += dr;
		c += dc;
	}
}

bool kingInCheck(enum Pieces king, int& attackerCount) {

	pair<int, int> kingPos = getKingPos(king);

	int kRow = kingPos.first, kCol = kingPos.second;

	bool temp = turn;
	turn = (king == WHITE_KING) ? false : true;

	resetMovesArray();
	attackerSquares.clear();
	blockingSquares.clear();

	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLS; ++c) {
			enum Pieces curr = boardArray[r][c];
			if (curr == EMPTY) continue;

			bool isEnemy =
				(king == WHITE_KING && curr >= BLACK_KING && curr <= BLACK_PAWN) ||
				(king == BLACK_KING && curr >= WHITE_KING && curr <= WHITE_PAWN);

			if (!isEnemy) continue;

			resetMovesArray();
			switch (curr) {
			case BLACK_KING: case WHITE_KING:
				kingMove(r, c);
				break;
			case BLACK_BISHOP: case WHITE_BISHOP:
				bishopMove(r, c);
				break;
			case BLACK_KNIGHT: case WHITE_KNIGHT:
				knightMove(r, c);
				break;
			case BLACK_ROOK:   case WHITE_ROOK:
				rookMove(r, c);
				break;
			case BLACK_QUEEN:  case WHITE_QUEEN:
				queenMove(r, c);
				break;
			case BLACK_PAWN:   case WHITE_PAWN:
				pawnMove(r, c);
				break;
			default:
				break;
			}

			if (movesArray[kRow][kCol]) {
				attackerSquares.emplace_back(r, c);
			}
		}
	}

	if (attackerSquares.size() == 1) {
		int ar = attackerSquares[0].first;
		int ac = attackerSquares[0].second;

		bool flag = (boardArray[ar][ac] == WHITE_BISHOP || boardArray[ar][ac] == BLACK_BISHOP ||
					 boardArray[ar][ac] == WHITE_ROOK || boardArray[ar][ac] == BLACK_ROOK ||
					 boardArray[ar][ac] == WHITE_QUEEN || boardArray[ar][ac] == BLACK_QUEEN);

		if (flag) {
			buildCheckPath(ar, ac, kRow, kCol, blockingSquares);
		}
	}

	bool attacked = (!attackerSquares.empty());
	attackerCount = (int)attackerSquares.size();

	turn = temp;
	resetMovesArray();


	return attacked;
}

bool isMoveLegal(int fromRow, int fromCol, int toRow, int toCol,enum Pieces piece, enum Pieces king){

	enum Pieces dest = boardArray[toRow][toCol];
	boardArray[toRow][toCol] = piece;
	boardArray[fromRow][fromCol] = EMPTY;

	int t = 0;
	bool illegal = kingInCheck(king, t);

	boardArray[fromRow][fromCol] = piece;
	boardArray[toRow][toCol] = dest;
	return !illegal;
}

void resolveCheck(int row, int col, enum Pieces resolvingPiece) {
	resetResolveArray();
	resetMovesArray();

	enum Pieces kingPiece = turn ? WHITE_KING : BLACK_KING;
	int attackerCount = 0;
	bool inChk = kingInCheck(kingPiece, attackerCount);
	if (!inChk) return;

	if (attackerCount > 1) {
		if (resolvingPiece != WHITE_KING && resolvingPiece != BLACK_KING) {
			return;
		}

		kingMove(row, col);
		vector<pair<int, int>> possibleMoves;

		for (int r = 0; r < ROWS; ++r)
			for (int c = 0; c < COLS; ++c)
				if (movesArray[r][c])
					possibleMoves.emplace_back(r, c);

		for (auto& cand : possibleMoves) {
			int toR = cand.first, toC = cand.second;
			enum Pieces dest = boardArray[toR][toC];

			boardArray[toR][toC] = resolvingPiece;
			boardArray[row][col] = EMPTY;
			int t = 0;
			if (!kingInCheck(kingPiece, t)) {
				resolveCheckArray[toR][toC] = true;
			}
			boardArray[row][col] = resolvingPiece;
			boardArray[toR][toC] = dest;
		}

		for (int r = 0; r < ROWS; ++r)
			for (int c = 0; c < COLS; ++c)
				movesArray[r][c] = resolveCheckArray[r][c];
		return;
	}

	switch (resolvingPiece) {
	case WHITE_KING:   case BLACK_KING:
		kingMove(row, col);
		break;
	case WHITE_QUEEN:  case BLACK_QUEEN:
		queenMove(row, col);
		break;
	case WHITE_ROOK:   case BLACK_ROOK:
		rookMove(row, col);
		break;
	case WHITE_BISHOP: case BLACK_BISHOP:
		bishopMove(row, col);
		break;
	case WHITE_KNIGHT: case BLACK_KNIGHT:
		knightMove(row, col);
		break;
	case WHITE_PAWN:   case BLACK_PAWN:
		pawnMove(row, col);
		break;
	default:
		return;
	}

	int aRow = attackerSquares[0].first;
	int aCol = attackerSquares[0].second;

	vector<pair<int, int>> possibleMoves;
	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLS; ++c) {
			if (movesArray[r][c]) {
				possibleMoves.emplace_back(r, c);
			}
		}
	}

	for (auto& cand : possibleMoves) {
		int toR = cand.first, toC = cand.second;

		enum Pieces dest = boardArray[toR][toC];

		if (resolvingPiece == WHITE_KING || resolvingPiece == BLACK_KING) {
			boardArray[toR][toC] = resolvingPiece;
			boardArray[row][col] = EMPTY;
			int t = 0;
			if (!kingInCheck(kingPiece, t)) {
				resolveCheckArray[toR][toC] = true;
			}
			boardArray[row][col] = resolvingPiece;
			boardArray[toR][toC] = dest;
			continue;
		}

		bool cancapture = (toR == aRow && toC == aCol);
		bool isblocking = false;
		for (auto& sq : blockingSquares) {
			if (sq.first == toR && sq.second == toC) {
				isblocking = true;
				break;
			}
		}
		if (!cancapture && !isblocking) {
			continue;
		}

		boardArray[toR][toC] = resolvingPiece;
		boardArray[row][col] = EMPTY;
		int t = 0;
		if (!kingInCheck(kingPiece, t)) {
			resolveCheckArray[toR][toC] = true;
		}
		boardArray[row][col] = resolvingPiece;
		boardArray[toR][toC] = dest;
	}

	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLS; ++c) {
			movesArray[r][c] = resolveCheckArray[r][c];
		}
	}
}

void checkEnPassRules(int row, int col, enum Pieces p) {

	if (p == WHITE_PAWN) {
		if (row == enPassCoord[0] && col == enPassCoord[1]) {
			boardArray[row + 1][col] = EMPTY;
		}
	}
	else if (p == BLACK_PAWN) {
		if (row == enPassCoord[0] && col == enPassCoord[1]) {
			boardArray[row - 1][col] = EMPTY;
		}
	}
	enPassCoord[0] = -1;
	enPassCoord[1] = -1;
	enPassantPossible = false;
}

/*castling rules 
1)King has not moved.

2)Rook (on the castling side) has not moved.

3)No pieces between the king and the rook.

4)King is not in check.

5)Squares the king passes through are not under attack.

6)The square the king ends up on is not under attack.*/
void checkCastleRules(int row, int col, enum Pieces p) {


	if (p == WHITE_KING) {
		if (!whiteKingMoved) {
			whiteKingMoved = true;
		}

		if (row == 7 && col == 6) {
			boardArray[7][5] = WHITE_ROOK;
			boardArray[7][7] = EMPTY;
			whiteKingCanCastleRight = false;
			whiteKingCanCastleLeft = false;
		}
		if (row == 7 && col == 2) {
			boardArray[7][3] = WHITE_ROOK;
			boardArray[7][0] = EMPTY;
			whiteKingCanCastleRight = false;
			whiteKingCanCastleLeft = false;

		}
	}
	else if (p == BLACK_KING) {
		if (!blackKingMoved) {
			blackKingMoved = true;
		}

		if (row == 0 && col == 6) {
			boardArray[0][5] = BLACK_ROOK;
			boardArray[0][7] = EMPTY;
			blackKingCanCastleRight = false;
			blackKingCanCastleLeft = false;

		}
		if (row == 0 && col == 2) {
			boardArray[0][3] = BLACK_ROOK;
			boardArray[0][0] = EMPTY;
			blackKingCanCastleRight = false;
			blackKingCanCastleLeft = false;

		}
	}

	if (boardArray[7][7] == EMPTY) {
		if (!rightWhiteRookMoved) {
			rightWhiteRookMoved = true;
		}
	}
	if (boardArray[7][0] == EMPTY) {
		if (!leftWhiteRookMoved) {
			leftWhiteRookMoved = true;
		}
	}
	if (boardArray[0][7] == EMPTY) {
		if (!rightBlackRookMoved) {
			rightBlackRookMoved = true;
		}
	}
	if (boardArray[0][0] == EMPTY) {
		if (!leftBlackRookMoved) {
			leftBlackRookMoved = true;
		}
	}

}

void movePiece(int row, int col, enum Pieces pieceSelected) {
	enum Pieces king = turn ? WHITE_KING : BLACK_KING;
	int attackerCount = 0;

	bool attacked = kingInCheck(king, attackerCount);

	if (king == WHITE_KING) inCheck[0] = attacked;
	else if (king == BLACK_KING) inCheck[1] = attacked;

	if (attacked) {

		resolveCheck(row, col, pieceSelected);
	}
	else {
		showPossibleMoves(row, col, pieceSelected);
		
	}
}

bool pawnMoved(int fromRow,int fromCol) {

	
	if (boardArray[fromRow][fromCol] == BLACK_PAWN || boardArray[fromRow][fromCol] == WHITE_PAWN) {
		return true;
	}
	return false;
}
bool capturedPiece(int toRow,int toCol) {

	if (boardArray[toRow][toCol] != EMPTY) {
		return true;
	}

	return false;
}

bool fiftyMoveDraw() {
	return fiftyCount == 50;
}

bool DrawByInsufficientMaterial() {

	int whiteQueenCount = 0;
	int whiteRookCount = 0;
	int whiteBishopCount = 0;
	int whiteKnightCount = 0;
	int whitePawnCount = 0;

	int blackQueenCount = 0;
	int blackRookCount = 0;
	int blackBishopCount = 0;
	int blackKnightCount = 0;
	int blackPawnCount = 0;

	for (int r = 0; r < ROWS; ++r) {
		for (int c = 0; c < COLS; ++c) {
			switch (boardArray[r][c]) {
			case WHITE_QUEEN:  whiteQueenCount++;  break;
			case WHITE_ROOK:   whiteRookCount++;   break;
			case WHITE_BISHOP: whiteBishopCount++; break;
			case WHITE_KNIGHT: whiteKnightCount++; break;
			case WHITE_PAWN:   whitePawnCount++;   break;

			case BLACK_QUEEN:  blackQueenCount++;  break;
			case BLACK_ROOK:   blackRookCount++;   break;
			case BLACK_BISHOP: blackBishopCount++; break;
			case BLACK_KNIGHT: blackKnightCount++; break;
			case BLACK_PAWN:   blackPawnCount++;   break;

			default: break;
			}
		}
	}

	//these material isnt a draw
	if (whiteQueenCount > 0 || whiteRookCount > 0 || whitePawnCount > 0 || blackQueenCount > 0 || blackRookCount > 0 || blackPawnCount > 0) {
		return false;
	}

	int whiteMinors = whiteBishopCount + whiteKnightCount;
	int blackMinors = blackBishopCount + blackKnightCount;
	int totalMinors = whiteMinors + blackMinors;

	// K vs K
	if (totalMinors == 0) {
		return true;
	}

	// K+N vs K
	if (whiteKnightCount == 1 && totalMinors == 1 && blackMinors == 0) {
		return true;
	}
	if (blackKnightCount == 1 && totalMinors == 1 && whiteMinors == 0) {
		return true;
	}

	// K+B vs K
	if (whiteBishopCount == 1 && totalMinors == 1 && blackMinors == 0) {
		return true;
	}
	if (blackBishopCount == 1 && totalMinors == 1 && whiteMinors == 0) {
		return true;
	}

	// K+B vs K+B
	if (whiteBishopCount == 1 && blackBishopCount == 1 && whiteKnightCount == 0 && blackKnightCount == 0){
		return true;
	}

	return false;
}


string generateBoardSignature() {
	std::string state = "";

	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLS; c++) {
			state += std::to_string(boardArray[r][c]) + ",";
		}
	}

	state += (turn ? "w" : "b");

	if (whiteKingCanCastleLeft) state += "Q";
	if (whiteKingCanCastleRight) state += "K";
	if (blackKingCanCastleLeft) state += "q";
	if (blackKingCanCastleRight) state += "k";

	if (enPassantPossible) {
		state += "e" + std::to_string(enPassCoord[0]) + std::to_string(enPassCoord[1]);
	}

	return state;
}

bool isThreefoldRepetition() {
	std::string current = generateBoardSignature();
	int count = 0;

	for (const auto& state : boardState) {
		if (state == current) count++;
	}

	return count >= 3;
}


void selectMove(int row,int col) {
	bool isWhitePiece = boardArray[row][col] >= WHITE_KING && boardArray[row][col] <= WHITE_PAWN;

	if ((turn && isWhitePiece) || (!turn && !isWhitePiece)) {
		resetMovesArray();
		resetResolveArray();

		enum Pieces piece = boardArray[row][col];
		movePiece(row, col, piece);
		isSelected = true;
		selectedRow = row;
		selectedCol = col;
		cout << "Selected piece at (" << row << ", " << col << ")" << endl;
	}
}

int playMove(int row,int col,sf::RenderWindow & window) {
	bool legalMove = false;
	enum Pieces king = turn ? WHITE_KING : BLACK_KING;

	if ((turn && inCheck[0]) || (!turn && inCheck[1])) {
		if (resolveCheckArray[row][col]) {

			if (isMoveLegal(selectedRow, selectedCol, row, col, boardArray[selectedRow][selectedCol], king)) {
				legalMove = true;
			}
		}
		else {
			checkMateScreen(window);
			return 1;
		}
	}
	else {
		if (movesArray[row][col]) {
			if (isMoveLegal(selectedRow, selectedCol, row, col, boardArray[selectedRow][selectedCol], king)) {
				legalMove = true;
			}
			else {
				staleMateScreen(window);
				return 1;
			}
		}
	}

	if (legalMove) {

		if (pawnMoved(selectedRow, selectedCol) && capturedPiece(row, col)) {
			fiftyCount = 0;
		}
		else {
			fiftyCount++;
		}

		enum Pieces p = boardArray[selectedRow][selectedCol];
		if ((p == WHITE_PAWN && row == 0) || (p == BLACK_PAWN && row == 7)) {
			p = pawnPromotion(window);
		}
		checkEnPassRules(row, col, p);
		checkCastleRules(row, col, p);

		boardArray[row][col] = p;
		boardArray[selectedRow][selectedCol] = EMPTY;
		fullMoveCount++;

		std::string sig = generateBoardSignature();
		boardState.push_back(sig);


		if (isThreefoldRepetition()) {
			threeFoldScreen(window);
			return 1;
		}


		lastPlayedMove[0] = selectedRow;
		lastPlayedMove[1] = selectedCol;
		lastPlayedMove[2] = row;
		lastPlayedMove[3] = col;

		turn = !turn;

		int t = 0;


		bool attacked = kingInCheck(king, t);

		if (king == WHITE_KING) inCheck[0] = attacked;
		else if (king == BLACK_KING) inCheck[1] = attacked;



		cout << "Turn switched to " << (turn ? "White" : "Black") << endl;
		cout << "White in check: " << inCheck[0] << ", Black in check: " << inCheck[1] << endl;



	}

	isSelected = false;
	resetMovesArray();
	resetResolveArray();
}


int handleInput(sf::RenderWindow& window) {

	if (DrawByInsufficientMaterial()) {
		Draw(window);
		return 1;
	}

	if (fiftyMoveDraw()) {
		fiftyMoveScreen(window);
		return 1;

	}


	bool pause = true;
	sf::Event event;
	while (window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			window.close();

		if (event.type == sf::Event::MouseButtonPressed) {
			if (event.mouseButton.button == sf::Mouse::Left) {
				sf::Vector2i mousePos = sf::Mouse::getPosition(window);
				int row = mousePos.y / 123.0f; 
				int col = mousePos.x / 123.0f;


				if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
					if (!isSelected) {
						if (boardArray[row][col] != EMPTY) {
							selectMove(row, col);
						}
					}
					else {
						if (playMove(row, col, window) == 1) {
							return 1;
						}
					}
				}
			}
		}

		if (event.type == sf::Event::KeyPressed) {
			switch (event.key.code) {
			case sf::Keyboard::Escape:
				pause = pauseScreen(window);
				if (!pause) {
					return 1;
				}
				break;
			default:
				break;
			}
		}
	}
}

void resetGame() {
	resetMovesArray();
	resetResolveArray();


	enum Pieces temp[ROWS][COLS] = {
		{BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK },
		{BLACK_PAWN, BLACK_PAWN  , BLACK_PAWN  , BLACK_PAWN , BLACK_PAWN, BLACK_PAWN  , BLACK_PAWN ,  BLACK_PAWN, },
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
		{EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,EMPTY , EMPTY, EMPTY},
		{WHITE_PAWN, WHITE_PAWN  , WHITE_PAWN  , WHITE_PAWN , WHITE_PAWN, WHITE_PAWN  , WHITE_PAWN ,  WHITE_PAWN, },
		{WHITE_ROOK, WHITE_KNIGHT,WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK} };

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			boardArray[i][j] = temp[i][j];
		}
	}


	isSelected = false;
	selectedRow = -1;
	selectedCol = -1;

	boardState.clear();
	whiteKingCanCastleLeft = false;
	whiteKingCanCastleRight = false;
	blackKingCanCastleLeft = false;
	blackKingCanCastleRight = false;
	enPassantPossible = false;


	inCheck[0] = false;
	inCheck[1] = false;

	for (int i = 0; i < 4; i++)lastPlayedMove[i] = -1;

	fiftyCount = 0;
	fullMoveCount = 0;

	turn = true;

	whiteKingMoved = false;
	blackKingMoved = false;
	rightWhiteRookMoved = false;
	leftWhiteRookMoved = false;
	rightBlackRookMoved = false;
	leftBlackRookMoved = false;
	enPassCoord[0] = -1;
	enPassCoord[1] = -1;

	attackerSquares.clear();
	blockingSquares.clear();

}

void executeGame(sf::RenderWindow & window) {

	while (window.isOpen()) {

		if (handleInput(window) == 1) {
			resetGame();
			return;
		}

		renderGame(window);
		window.display();
	}
}

int playStockMove(int row, int col, sf::RenderWindow& window,enum Pieces promo) {
	bool legalMove = false;
	enum Pieces king = turn ? WHITE_KING : BLACK_KING;

	if ((turn && inCheck[0]) || (!turn && inCheck[1])) {
		if (resolveCheckArray[row][col]) {

			if (isMoveLegal(selectedRow, selectedCol, row, col, boardArray[selectedRow][selectedCol], king)) {
				legalMove = true;
			}
		}
		else {
			checkMateScreen(window);
			return 1;
		}
	}
	else {
		if (movesArray[row][col]) {
			if (isMoveLegal(selectedRow, selectedCol, row, col, boardArray[selectedRow][selectedCol], king)) {
				legalMove = true;
			}
			else {

				staleMateScreen(window);
				return 1;
			}
		}
	}

	if (legalMove) {

		if (pawnMoved(selectedRow, selectedCol) && capturedPiece(row, col)) {
			fiftyCount = 0;
		}
		else {
			fiftyCount++;
		}

		enum Pieces p = boardArray[selectedRow][selectedCol];
		if ((p == WHITE_PAWN && row == 0) || (p == BLACK_PAWN && row == 7)) {
			p = promo;
		}
		checkEnPassRules(row, col, p);
		checkCastleRules(row, col, p);

		boardArray[row][col] = p;
		boardArray[selectedRow][selectedCol] = EMPTY;
		fullMoveCount++;

		std::string sig = generateBoardSignature();
		boardState.push_back(sig);


		if (isThreefoldRepetition()) {
			threeFoldScreen(window);
			return 1;
		}


		lastPlayedMove[0] = selectedRow;
		lastPlayedMove[1] = selectedCol;
		lastPlayedMove[2] = row;
		lastPlayedMove[3] = col;

		turn = !turn;

		int t = 0;


		bool attacked = kingInCheck(king, t);

		if (king == WHITE_KING) inCheck[0] = attacked;
		else if (king == BLACK_KING) inCheck[1] = attacked;



		cout << "Turn switched to " << (turn ? "White" : "Black") << endl;
		cout << "White in check: " << inCheck[0] << ", Black in check: " << inCheck[1] << endl;



	}

	isSelected = false;
	resetMovesArray();
	resetResolveArray();
}


//-----------------------------------------------------------------------------------------------------------------------
HANDLE hChildStdin_Read = NULL;  
HANDLE hChildStdin_Write = NULL; 
HANDLE hChildStdout_Read = NULL;
HANDLE hChildStdout_Write = NULL;  

PROCESS_INFORMATION piStockfish = { 0 };

SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

struct ParsedMove {
	int fromRow;
	int fromCol;
	int toRow;
	int toCol;
	enum Pieces promotion;

	ParsedMove()
		: fromRow(-1), fromCol(-1), toRow(-1), toCol(-1), promotion(EMPTY) {
	}
};


void startStockfish() {

	if (!CreatePipe(&hChildStdin_Read, &hChildStdin_Write, &sa, 0)) {
		std::cerr << "CreatePipe (stdin) failed.\n";
		return;
	}

	SetHandleInformation(hChildStdin_Write, HANDLE_FLAG_INHERIT, 0);

	if (!CreatePipe(&hChildStdout_Read, &hChildStdout_Write, &sa, 0)) {
		std::cerr << "CreatePipe (stdout) failed.\n";
		return;
	}
	SetHandleInformation(hChildStdout_Read, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = hChildStdin_Read;   // Child reads from this
	si.hStdOutput = hChildStdout_Write; // Child writes to this
	si.hStdError = hChildStdout_Write;

	BOOL ok = CreateProcess(
		L"assets/stockfish/stockfish.exe",
		NULL,
		NULL, NULL, TRUE,
		0, NULL, NULL,
		&si, &piStockfish
	);

	if (!ok) {
		std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
		return;
	}

	CloseHandle(hChildStdin_Read);
	CloseHandle(hChildStdout_Write);
}


void sendCommand(const std::string& cmd) {

	if (hChildStdin_Write == INVALID_HANDLE_VALUE) return;
	DWORD written = 0;
	std::string line = cmd + "\n";
	BOOL ok = WriteFile(hChildStdin_Write, line.c_str(), (DWORD)line.size(), &written, NULL);
	if (!ok) {
		std::cerr << "WriteFile to Stockfish stdin failed.\n";
	}
}


string readBestMove() {
	std::string accumulated;
	char buffer[128];
	DWORD bytesRead = 0;

	while (true) {
		BOOL ok = ReadFile(hChildStdout_Read, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
		if (!ok || bytesRead == 0) {
			break;
		}
		buffer[bytesRead] = '\0';
		accumulated += buffer;

		if (accumulated.find("bestmove") != std::string::npos) {
			break;
		}
	}

	size_t pos = accumulated.find("bestmove");
	if (pos == std::string::npos) {
		return "";
	}

	size_t eol = accumulated.find('\n', pos);
	std::string bestLine = (eol != std::string::npos)
		? accumulated.substr(pos, eol - pos)
		: accumulated.substr(pos);

	return bestLine;
}

void stopStockfish() {
	if (hChildStdin_Write && hChildStdin_Write != INVALID_HANDLE_VALUE) {

		sendCommand("quit");
		CloseHandle(hChildStdin_Write);
		hChildStdin_Write = NULL;
	}

	if (piStockfish.hProcess && piStockfish.hProcess != INVALID_HANDLE_VALUE) {
		DWORD exitCode = STILL_ACTIVE;
		GetExitCodeProcess(piStockfish.hProcess, &exitCode);
		if (exitCode == STILL_ACTIVE) {
			WaitForSingleObject(piStockfish.hProcess, 1000);
		}
		GetExitCodeProcess(piStockfish.hProcess, &exitCode);
		if (exitCode == STILL_ACTIVE) {
			TerminateProcess(piStockfish.hProcess, 0);
		}
		CloseHandle(piStockfish.hProcess);
		piStockfish.hProcess = NULL;
	}

	if (hChildStdout_Read && hChildStdout_Read != INVALID_HANDLE_VALUE) {
		CloseHandle(hChildStdout_Read);
		hChildStdout_Read = NULL;
	}
}





void setDifficulty(int level) {
	sendCommand("uci");
	sendCommand("setoption name Skill Level value " + std::to_string(level));
	sendCommand("isready");
}



ParsedMove moveParser(const std::string& bestMove) {
	ParsedMove m;
	if (bestMove.size() < 4) return m;

	m.fromCol = bestMove[0] - 'a';
	m.fromRow = '8' - bestMove[1];
	m.toCol = bestMove[2] - 'a';
	m.toRow = '8' - bestMove[3];

	if (bestMove.size() == 5) {
		char promoChar = bestMove[4];
		switch (promoChar) {
		case 'q': m.promotion = (turn ? WHITE_QUEEN : BLACK_QUEEN);  break;
		case 'r': m.promotion = (turn ? WHITE_ROOK : BLACK_ROOK);   break;
		case 'b': m.promotion = (turn ? WHITE_BISHOP : BLACK_BISHOP); break;
		case 'n': m.promotion = (turn ? WHITE_KNIGHT : BLACK_KNIGHT); break;
		default:  m.promotion = EMPTY; break;
		}
	}

	return m;
}


char pieceToFEN(enum Pieces piece) {
	switch (piece) {
	case WHITE_KING: return 'K';
	case WHITE_QUEEN: return 'Q';
	case WHITE_ROOK: return 'R';
	case WHITE_BISHOP: return 'B';
	case WHITE_KNIGHT: return 'N';
	case WHITE_PAWN: return 'P';
	case BLACK_KING: return 'k';
	case BLACK_QUEEN: return 'q';
	case BLACK_ROOK: return 'r';
	case BLACK_BISHOP: return 'b';
	case BLACK_KNIGHT: return 'n';
	case BLACK_PAWN: return 'p';
	default: return '1'; 
	}
}

string generateFEN() {
	std::string fen = "";

	for (int row = 0; row < 8; ++row) {
		int emptyCount = 0;
		for (int col = 0; col < 8; ++col) {
			enum Pieces piece = boardArray[row][col];
			if (piece == EMPTY) {
				emptyCount++;
			}
			else {
				if (emptyCount > 0) {
					fen += std::to_string(emptyCount);
					emptyCount = 0;
				}
				fen += pieceToFEN(piece);
			}
		}
		if (emptyCount > 0) {
			fen += std::to_string(emptyCount);
		}
		if (row < 7) fen += '/';
	}

	fen += (turn ? " w " : " b ");

	std::string castling = "";
	if (whiteKingCanCastleRight) castling += 'K';
	if (whiteKingCanCastleLeft) castling += 'Q';
	if (blackKingCanCastleRight) castling += 'k';
	if (blackKingCanCastleLeft) castling += 'q';
	fen += (castling.empty() ? "-" : castling) + std::string(" ");

	if (enPassCoord[0] != -1 && enPassCoord[1] != -1) {
		char file = 'a' + (enPassCoord[0]);
		char rank = '8' - (enPassCoord[1]);
		fen += std::string() + file + rank + " ";
	}
	else {
		fen += "- ";
	}

	fen += std::to_string(fiftyCount) + " ";

	fen += std::to_string(fullMoveCount);

	return fen;
}





string generateBestMove(const std::string& fen) {

	sendCommand("position fen " + fen);
	sendCommand("go movetime 1000");

	std::string bestLine = readBestMove();

	size_t pos = bestLine.find("bestmove ");
	if (pos != std::string::npos) {
		pos += strlen("bestmove ");
		size_t end = bestLine.find(" ", pos); 
		if (end == std::string::npos) {
			end = bestLine.length();
		}
		std::string move = bestLine.substr(pos, end - pos);
		if (move == "(none)") {
			return "draw";
		}
		return move;
	}
	std::cerr << "No bestmove found in Stockfish output: " << bestLine << std::endl;
	return "resign";
}

int Input2(sf::RenderWindow &window,bool playerColor) {

	if (DrawByInsufficientMaterial()) {
		Draw(window);
		return 1;
	}

	if (fiftyMoveDraw()) {
		fiftyMoveScreen(window);
		return 1;

	}

	if (playerColor) { //white
		if (turn) {

			bool pause = true;
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					window.close();

				if (event.type == sf::Event::MouseButtonPressed) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						sf::Vector2i mousePos = sf::Mouse::getPosition(window);
						int row = mousePos.y / 123.0f;
						int col = mousePos.x / 123.0f;


						if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
							if (!isSelected) {
								if (boardArray[row][col] != EMPTY) {

									selectMove(row, col);
								}
							}
							else {

								if (playMove(row, col, window) == 1) {
									return 1;
								}
							}
						}
					}
				}

				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
					case sf::Keyboard::Escape:
						pause = pauseScreen(window);
						if (!pause) {
							return 1;
						}
						break;
					default:
						break;
					}
				}
			}
		}
		else {

			string fen = generateFEN();
			string move = generateBestMove(fen);

			if (move == "resign") {
				resignScreen(window);
				return 1;
			}
			if (move == "draw") {
				Draw(window);
				return 1;
			}

			ParsedMove p = moveParser(move);



			selectMove(p.fromRow, p.fromCol);

			if (playStockMove(p.toRow, p.toCol, window,p.promotion) == 1) {
				return 1;
			}

		}
	}
	else {
		if (turn) {
			string fen = generateFEN();
			string move = generateBestMove(fen);

			ParsedMove p = moveParser(move);



			selectMove(p.fromRow, p.fromCol);

			if (playStockMove(p.toRow, p.toCol, window, p.promotion) == 1) {
				return 1;
			}
		}
		else {

			bool pause = true;
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					window.close();

				if (event.type == sf::Event::MouseButtonPressed) {
					if (event.mouseButton.button == sf::Mouse::Left) {
						sf::Vector2i mousePos = sf::Mouse::getPosition(window);
						int row = mousePos.y / 123.0f;
						int col = mousePos.x / 123.0f;


						if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
							if (!isSelected) {
								if (boardArray[row][col] != EMPTY) {
									bool isWhitePiece = boardArray[row][col] >= WHITE_KING && boardArray[row][col] <= WHITE_PAWN;

									selectMove(row, col);
								}
							}
							else {

								if (playMove(row, col, window) == 1) {
									return 1;
								}
							}
						}
					}
				}

				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
					case sf::Keyboard::Escape:
						pause = pauseScreen(window);
						if (!pause) {
							return 1;
						}
						break;
					default:
						break;
					}
				}
			}
		}
	}

}



void playWithStockFish(int difficulty,sf::RenderWindow & window,bool playerColor) {

	turn = true;



	startStockfish();
	setDifficulty(difficulty);



	while (window.isOpen()) {

		if (Input2(window,playerColor) == 1) {
			stopStockfish();
			resetGame();
			return;
		}

		renderGame(window);
		window.display();
	}
}

bool playAs(sf::RenderWindow& window) {
	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2;

	text.setFont(font);
	text.setString("PRESS 1 TO PLAY AS WHITE");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(170, 500);


	text2.setFont(font);
	text2.setString("PRESS 2 TO PLAY AS BLACK");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(170, 600);



	while (window.isOpen()) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {


			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Num1:
					return true;
					break;
				case sf::Keyboard::Num2:
					return false;
					break;
				default:
					break;
				}
			}
		}
	}
}

int displayStockFishLevels(sf::RenderWindow & window) {

	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2, text3, text4;
	text.setFont(font);
	text.setString("STOCKFISH");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(360, 400);

	text2.setFont(font);
	text2.setString("PRESS 1 FOR EASY MODE");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(240, 500);


	text3.setFont(font);
	text3.setString("PRESS 2 FOR MEDIUM MODE");
	text3.setCharacterSize(35);
	text3.setFillColor(sf::Color(131, 156, 143));
	text3.setPosition(220, 600);


	text4.setFont(font);
	text4.setString("PRESS 3 FOR HARD MODE");
	text4.setCharacterSize(35);
	text4.setFillColor(sf::Color(131, 156, 143));
	text4.setPosition(240, 700);


	while (window.isOpen()) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.draw(text3);
		window.draw(text4);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {


			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Num1:
					return 5;
					break;
				case sf::Keyboard::Num2:
					return 10;
					break;
				case sf::Keyboard::Num3:
					return 20;
				case sf::Keyboard::Escape:
					return 0;
				default:
					break;
				}
			}
		}
	}

}

void menu() {

	sf::RenderWindow window(sf::VideoMode(960, 960), "Chess Game");

	sf::Sprite titleSpr(title);
	titleSpr.setPosition(170, 100);
	titleSpr.setScale(0.7f, 0.7f);

	sf::Text text, text2;
	text.setFont(font);
	text.setString("PRESS 1 FOR 2 PLAYER CHESS");
	text.setCharacterSize(35);
	text.setFillColor(sf::Color(131, 156, 143));
	text.setPosition(190, 500);

	text2.setFont(font);
	text2.setString("PRESS 2 TO PLAY WITH STOCKFISH");
	text2.setCharacterSize(35);
	text2.setFillColor(sf::Color(131, 156, 143));
	text2.setPosition(155, 700);

	bool playerColor = false;
	int difficulty = 0;

	while (window.isOpen()) {
		window.clear(sf::Color(29, 59, 52));

		window.draw(titleSpr);
		window.draw(text);
		window.draw(text2);
		window.display();

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::Num1:
					executeGame(window);
					break;
				case sf::Keyboard::Num2:
					difficulty = displayStockFishLevels(window);
					playerColor = playAs(window);
					playWithStockFish(difficulty,window,playerColor);
					break;
				case sf::Keyboard::Escape:
					return;
				default:
					break;
				}
			}
		}
	}
}


void initializeTextures() {


	title.loadFromFile("assets/images/title.png");
    board.loadFromFile("assets/images/board.png");

	whiteKing.loadFromFile("assets/images/white_king.png");
	whiteQueen.loadFromFile("assets/images/white_queen.png");
	whiteRook.loadFromFile("assets/images/white_rook.png");
	whiteBishop.loadFromFile("assets/images/white_bishop.png");
	whiteKnight.loadFromFile("assets/images/white_knight.png");
	whitePawn.loadFromFile("assets/images/white_pawn.png");
	blackKing.loadFromFile("assets/images/black_king.png");
	blackQueen.loadFromFile("assets/images/black_queen.png");
	blackRook.loadFromFile("assets/images/black_rook.png");
	blackBishop.loadFromFile("assets/images/black_bishop.png");
	blackKnight.loadFromFile("assets/images/black_knight.png");
    blackPawn.loadFromFile("assets/images/black_pawn.png");

	moveIndicator.loadFromFile("assets/images/move_Indicator.png");
	checkIndicator.loadFromFile("assets/images/check_Indicator.jpg");

}

void loadFonts() {
	font.loadFromFile("assets/fonts/font.otf");
}

void initBoard() {


}

void initalizeGame() {

    initializeTextures();
    loadFonts();

	//resetGame();

	inCheck[0] = false;
	inCheck[1] = false;
}


int main() {


	turn = true;
    initalizeGame();

	menu();

   
    return 0;
}
