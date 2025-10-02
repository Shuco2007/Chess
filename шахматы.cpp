#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

const int windowWidth = 768;
const int windowHeight = 768;
const int fields = 8;
const int tileSize = windowWidth / fields;

const string ROOK_TEXTURE_WHITE = "шахматы/assets/w_Rook.png";
const string ROOK_TEXTURE_BLACK = "шахматы/assets/b_Rook.png";
const string PAWN_TEXTURE_WHITE = "шахматы/assets/w_Pawn.png";
const string PAWN_TEXTURE_BLACK = "шахматы/assets/b_Pawn.png";
const string HORSE_TEXTURE_WHITE = "шахматы/assets/w_Horse.png";
const string HORSE_TEXTURE_BLACK = "шахматы/assets/b_Horse.png";
const string ELEPHANT_TEXTURE_WHITE = "шахматы/assets/w_Elephant.png";
const string ELEPHANT_TEXTURE_BLACK = "шахматы/assets/b_Elephant.png";
const string QUEEN_TEXTURE_WHITE = "шахматы/assets/w_Queen.png";
const string QUEEN_TEXTURE_BLACK = "шахматы/assets/b_Queen.png";
const string KING_TEXTURE_WHITE = "шахматы/assets/w_King.png";
const string KING_TEXTURE_BLACK = "шахматы/assets/b_King.png";
const string BOARD_TEXTURE = "шахматы/assets/_composite.png";

enum class PieceColor {
	WHITE,
	BLACK
};

class ChessPiece {
protected:
	string textureFile;
	int x, y;
	Texture texture;
	Sprite sprite;
	PieceColor color;

public:
	ChessPiece(const string& texturefile, int _x, int _y, PieceColor _color){
		x = _x;
		y = _y;
		color = _color;
		texture.loadFromFile(texturefile);
		sprite.setTexture(texture);
		sprite.setScale(Vector2f(0.5f, 0.5f));
		sprite.setPosition(x, y);
	}
	
	virtual bool comparison_cords(int _x,int _y) {
		if (x == _x && y == _y) {
			return true;
		}
		else {
			return false;
		}
	}
	virtual bool comparison_colors(PieceColor _color) {
		if (color == _color) {
			return true;
		}
		else {
			return false;
		}
	}
	virtual void movement(int _x, int _y) {
		x = _x;
		y = _y;
	}
	virtual int get_x() {	
		return x;
	}
	virtual int get_y() {
		return y;
	}
	virtual PieceColor get_color() {
		return color;
	}
	virtual void draw(RenderWindow& window) {
		sprite.setPosition(x, y);
		window.draw(sprite);
	}
	virtual ~ChessPiece() = default;
};

class Rook : public ChessPiece {
public:
	Rook(int x, int y, PieceColor color) : ChessPiece(color == PieceColor::WHITE ? ROOK_TEXTURE_WHITE : ROOK_TEXTURE_BLACK, x, y, color) {}

	std::vector<std::vector<int>> creature_points(int index_figure, const std::vector<std::unique_ptr<ChessPiece>>& pieces) {

		std::vector<std::vector<int>> Vector;
		int start_x = pieces[index_figure]->get_x();
		int start_y = pieces[index_figure]->get_y();
		PieceColor color = pieces[index_figure]->get_color();

		std::vector<std::pair<int, int>> directions = {
			{0, tileSize},
			{0, -tileSize}, 
			{tileSize, 0}, 
			{-tileSize, 0}
		};

		for (const auto& dir : directions) {
			int current_x = start_x;
			int current_y = start_y;
			bool piece_found = false;
			while (current_x >= 0 && current_x < 768 && current_y >= 0 && current_y < 768 && !piece_found) {
				current_x += dir.first;
				current_y += dir.second;
				if (current_x >= 0 && current_x < 768 && current_y >= 0 && current_y < 768) {
					bool occupied = false;
					for (int j = 0; j < pieces.size(); ++j) {
						if (j == index_figure) continue;
						if (current_x == pieces[j]->get_x() && current_y == pieces[j]->get_y()) {
							occupied = true;
							if ((color == PieceColor::WHITE && pieces[j]->get_color() == PieceColor::BLACK) ||
								(color == PieceColor::BLACK && pieces[j]->get_color() == PieceColor::WHITE)) {
								Vector.push_back({ current_x, current_y });
							}
							piece_found = true;
							break;
						}
					}
					if (!occupied) {
						Vector.push_back({ current_x, current_y });
					}
				}
				else {
					break;
				}
			}
		}

		return Vector;
	}
};

class Pawn : public ChessPiece {
public:
	Pawn(int x, int y, PieceColor color) : ChessPiece(color == PieceColor::WHITE ? PAWN_TEXTURE_WHITE : PAWN_TEXTURE_BLACK, x, y, color) {}

	vector<vector<int>> creature_points(int index_figure, const vector<unique_ptr<ChessPiece>>& pieces) {
		vector<vector<int>> valid_moves;
		int start_x = pieces[index_figure]->get_x();
		int start_y = pieces[index_figure]->get_y();
		int move_direction = (color == PieceColor::WHITE) ? -1 : 1;
		int start_rank = (color == PieceColor::WHITE) ? 6 : 1; 
		int friendly_start = (color == PieceColor::WHITE) ? 0 : 16;
		int friendly_end = (color == PieceColor::WHITE) ? 16 : 32;
		bool on_starting_rank = (start_y == 15 + tileSize * start_rank);
		int forward_y = start_y + move_direction * tileSize;

		if (forward_y >= 0 && forward_y < windowWidth) {
			bool occupied = false;
			for (const auto& piece : pieces) {
				if (start_x == piece->get_x() && forward_y == piece->get_y()) {
					occupied = true;
					break;
				}
			}
			if (!occupied) {
				valid_moves.push_back({ start_x, forward_y });
			}
		}
		if (on_starting_rank) {
			int double_forward_y = start_y + 2 * move_direction * tileSize;
			if (double_forward_y >= 0 && double_forward_y < windowWidth) {
				bool occupied = false;
				for (const auto& piece : pieces) {
					if ((start_x == piece->get_x() && forward_y == piece->get_y()) ||
						(start_x == piece->get_x() && double_forward_y == piece->get_y())) {
						occupied = true;
						break;
					}
				}
				if (!occupied) {
					valid_moves.push_back({ start_x, double_forward_y });
				}
			}
		}

		int diag_left_x = start_x - tileSize;
		int diag_right_x = start_x + tileSize;
		int diag_y = start_y + move_direction * tileSize;

		if (diag_y >= 0 && diag_y < windowWidth) {
			if (diag_left_x >= 0 && diag_left_x < windowWidth) {
				for (const auto& piece : pieces) {
					if (diag_left_x == piece->get_x() && diag_y == piece->get_y()) {
						if ((color == PieceColor::WHITE && color == PieceColor::BLACK) ||
							(color == PieceColor::BLACK && color == PieceColor::WHITE)) {
							valid_moves.push_back({ diag_left_x, diag_y });
						}
						break;
					}
				}
			}
			if (diag_right_x >= 0 && diag_right_x < windowWidth) {
				for (const auto& piece : pieces) {
					if (diag_right_x == piece->get_x() && diag_y == piece->get_y()) {
						if ((color == PieceColor::WHITE && color == PieceColor::BLACK) ||
							(color == PieceColor::BLACK && color == PieceColor::WHITE)) {
							valid_moves.push_back({ diag_right_x, diag_y });
						}
						break;
					}
				}
			}
		}
		if (color == PieceColor::WHITE) {
			for (int i = 0; i < pieces.size(); i++) {
				if (start_x + tileSize == pieces[i]->get_x() && start_y - tileSize == pieces[i]->get_y() && i >= 16 && i <= 31 ) {
					valid_moves.push_back({ start_x + tileSize, start_y - tileSize });
				}
				if (start_x - tileSize == pieces[i]->get_x() && start_y - tileSize == pieces[i]->get_y() && i >= 16 && i <= 31) {
					valid_moves.push_back({ start_x - tileSize, start_y - tileSize });
				}
			}
		}
		else {
			for (int i = 0; i < pieces.size(); i++) {
				if (start_x + tileSize == pieces[i]->get_x() && start_y + tileSize == pieces[i]->get_y() && i >= 0 && i <= 15) {
					valid_moves.push_back({ start_x + tileSize, start_y + tileSize });
				}
				if (start_x - tileSize == pieces[i]->get_x() && start_y + tileSize == pieces[i]->get_y() && i >= 0 && i <= 15) {
					valid_moves.push_back({ start_x - tileSize, start_y + tileSize });
				}
			}
		}
		return valid_moves;
	}
};

class Horse : public ChessPiece {
public:
	Horse(int x, int y, PieceColor color) : ChessPiece(color == PieceColor::WHITE ? HORSE_TEXTURE_WHITE : HORSE_TEXTURE_BLACK, x, y, color) {}

	vector<vector<int>> creature_points(int index_figure, const vector<unique_ptr<ChessPiece>>& pieces) {
		vector<vector<int>> Vector;
		int start_x = pieces[index_figure]->get_x();
		int start_y = pieces[index_figure]->get_y();

		vector<vector<int>> potential_moves = {
			{start_x + tileSize, start_y - 2 * tileSize},
			{start_x + tileSize, start_y + 2 * tileSize},
			{start_x - tileSize, start_y - 2 * tileSize},
			{start_x - tileSize, start_y + 2 * tileSize},
			{start_x + 2 * tileSize, start_y - tileSize},
			{start_x + 2 * tileSize, start_y + tileSize},
			{start_x - 2 * tileSize, start_y - tileSize},
			{start_x - 2 * tileSize, start_y + tileSize}
		};
		for (const auto& move : potential_moves) {
			int move_x = move[0];
			int move_y = move[1];
			if (move_x >= 0 && move_x < 768 && move_y >= 0 && move_y < 768) {
				bool occupied = false;
				for (int j = 0; j < pieces.size(); ++j) {
					if (move_x == pieces[j]->get_x() && move_y == pieces[j]->get_y()) {
						occupied = true;
						if (color == PieceColor::WHITE) {
							if (j >= 0 && j < 16) {
								break;
							}
							else {
								Vector.push_back({ move_x, move_y });
							}
						}
						else {
							if (j >= 16 && j < 32) {
								break;
							}
							else {
								Vector.push_back({ move_x, move_y });
							}
						}
					}
				}
				if (!occupied) {
					Vector.push_back(move);
				}
			}
		}
		return Vector;
	}
};

class Elephant : public ChessPiece {
public:
	Elephant(int x, int y, PieceColor color) : ChessPiece(color == PieceColor::WHITE ? ELEPHANT_TEXTURE_WHITE : ELEPHANT_TEXTURE_BLACK, x, y, color) {}

	vector<vector<int>> creature_points(int index_figure, const vector<unique_ptr<ChessPiece>>& pieces) {
		vector<vector<int>> Vector;
		int start_x = pieces[index_figure]->get_x();
		int start_y = pieces[index_figure]->get_y();

		vector<vector<int>> potential_moves = {
			{tileSize, tileSize},
			{-tileSize, -tileSize},
			{tileSize, -tileSize},
			{-tileSize, tileSize}
		};

		for (const auto& dir : potential_moves) {
			bool piece_found = false;
			int current_x = start_x;
			int current_y = start_y;
			while (current_x >= 0 && current_x < 768 && current_y >= 0 && current_y < 768 && !piece_found) {
				current_x += dir[0];
				current_y += dir[1];
				if (current_x >= 0 && current_x < 768 && current_y >= 0 && current_y < 768) {
					bool occupied = false;
					for (int j = 0; j < pieces.size(); ++j) {
						if (j == index_figure) continue;
						if (current_x == pieces[j]->get_x() && current_y == pieces[j]->get_y()) {
							occupied = true;
							if ((color == PieceColor::WHITE && pieces[j]->get_color() == PieceColor::BLACK) ||
								(color == PieceColor::BLACK && pieces[j]->get_color() == PieceColor::WHITE)) {
								Vector.push_back({ current_x, current_y });
							}
							piece_found = true;
							break;
							
						}
					}
					if (!occupied) {
						Vector.push_back({ current_x, current_y });
					}
				}
				else {
					break;
				}
			}
		}
		return Vector;
	}
};

class Queen : public ChessPiece {
public:
	Queen(int x, int y, PieceColor color) : ChessPiece(color == PieceColor::WHITE ? QUEEN_TEXTURE_WHITE : QUEEN_TEXTURE_BLACK, x, y, color) {}

	vector<vector<int>> creature_points(int index_figure, const vector<unique_ptr<ChessPiece>>& pieces) {
		vector<vector<int>> valid_moves;
		int start_x = pieces[index_figure]->get_x();
		int start_y = pieces[index_figure]->get_y();

		vector<pair<int, int>> directions = {
			{tileSize, tileSize},   
			{-tileSize, -tileSize},  
			{-tileSize, tileSize},  
			{tileSize, -tileSize},    
			{0, tileSize},  
			{0, -tileSize}, 
			{tileSize, 0},  
			{-tileSize, 0}   
		};

		for (const auto& dir : directions) {
			int current_x = start_x;
			int current_y = start_y;
			while (current_x >= 0 && current_x < 768 && current_y >= 0 && current_y < 768) {
				current_x += dir.first;
				current_y += dir.second;
				if (current_x >= 0 && current_x < 768 && current_y >= 0 && current_y < 768) {
					bool occupied = false;
					bool is_enemy = false;
					for (int j = 0; j < pieces.size(); ++j) {
						if (current_x == pieces[j]->get_x() && current_y == pieces[j]->get_y()) {
							occupied = true;
							if ((color == PieceColor::WHITE && j >= 16 && j < 32) || (color == PieceColor::BLACK && j >= 0 && j < 16)) {
								is_enemy = true;
							}
							break;
						}
					}
					if (!occupied) {
						valid_moves.push_back({ current_x, current_y });
					}
					else if (is_enemy)
					{
						valid_moves.push_back({ current_x, current_y });
						break;
					}
					else {
						break; 
					}
				}
				else {
					break;
				}
			}
		}

		return valid_moves;
	}
};

class King : public ChessPiece {
public:
	King(int x, int y, PieceColor color) : ChessPiece(color == PieceColor::WHITE ? KING_TEXTURE_WHITE : KING_TEXTURE_BLACK, x, y, color) {}

	vector<vector<int>> creature_points(int index_figure, const vector<unique_ptr<ChessPiece>>& pieces) {
		vector<vector<int>> Vector;
		int x = pieces[index_figure]->get_x();
		int y = pieces[index_figure]->get_y();

		vector<vector<int>> potential_moves = {
			{x + tileSize, y + tileSize},
			{x + tileSize, y - tileSize},
			{x - tileSize, y + tileSize},
			{x - tileSize, y - tileSize},
			{x, y - tileSize},
			{x, y + tileSize},
			{x - tileSize, y},
			{x + tileSize, y}
		};
		for (const auto& move : potential_moves) {
			int move_x = move[0];
			int move_y = move[1];
			if (move_x >= 0 && move_x < 768 && move_y >= 0 && move_y < 768) {
				bool occupied = false;
				for (int j = 0; j < pieces.size(); j++) {
					if (move_x == pieces[j]->get_x() && move_y == pieces[j]->get_y()) {
						occupied = true;
						if (color == PieceColor::WHITE) {
							if (j >= 0 && j < 16) {
								break;
							}
							else {
								Vector.push_back({ move_x, move_y });
							}
						}
						else {
							if (j >= 16 && j < 32) {
								break;
							}
							else {
								Vector.push_back({ move_x, move_y });
							}
						}
					}
				}
				if (!occupied) {
					Vector.push_back({ move_x, move_y });
				}
			}
		}

		return Vector;
	}
};
class Board {
private:
	Texture boardTexture;
	Sprite boardSprite;

public:
	Board() {
		boardTexture.loadFromFile(BOARD_TEXTURE);
		boardSprite.setTexture(boardTexture);
	}

	void draw(RenderWindow& window) {
		window.draw(boardSprite);
	}

	void draw_shariki(RenderWindow& window, vector <CircleShape> shariki) {
		for (int i = 0; i < shariki.size(); i++)
		{
			window.draw(shariki[i]);
		}
	}
};


int defining_an_array_index_and_points(int x, int y, const vector<unique_ptr<ChessPiece>>& pieces, vector<vector<int>>& vector_points, PieceColor color) {
	vector_points.clear();
	int position_mous_x = 15 + tileSize * (x / tileSize);
	int position_mous_y = 15 + tileSize * (y / tileSize);
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i]->comparison_cords(position_mous_x, position_mous_y) == true && pieces[i]->comparison_colors(color) == true) {
			if (Pawn* pawn = dynamic_cast<Pawn*>(pieces[i].get())) {
				vector_points = pawn->creature_points(i, pieces);
			}
			else if (Rook* rook = dynamic_cast<Rook*>(pieces[i].get())) {
				vector_points = rook->creature_points(i, pieces);
			}
			else if (King* king = dynamic_cast<King*>(pieces[i].get())) {
				vector_points = king->creature_points(i, pieces);
			}
			else if (Queen* queen = dynamic_cast<Queen*>(pieces[i].get())) {
				vector_points = queen->creature_points(i, pieces);
			}
			else if (Elephant* elephant = dynamic_cast<Elephant*>(pieces[i].get())) {
				vector_points = elephant->creature_points(i, pieces);
			}
			else if (Horse* horse = dynamic_cast<Horse*>(pieces[i].get())) {
				vector_points = horse->creature_points(i, pieces);
			}
			else {
				std::cout << "Clicked on a piece, but creature_points not implemented yet" << std::endl;
			}
			return i;
		}
	}
	return -1;
}

int defining_an_array_index(int x, int y, const vector<unique_ptr<ChessPiece>>& pieces) {
	int position_mous_x = 15 + tileSize * (x / tileSize);
	int position_mous_y = 15 + tileSize * (y / tileSize);
	for (int i = 0; i < pieces.size(); i++) {
		if (pieces[i]->comparison_cords(position_mous_x, position_mous_y) == true) {
			return i;
		}
	}
	return -2;
}

void render(vector<vector<int>>& vector_points, RenderWindow& window, Board board) {
	if (vector_points.size() != 0)
	{
		Texture t;
		t.loadFromFile("шахматы/assets/square gray light _png_128px.png");
		CircleShape shape(5);
		shape.setTexture(&t);
		vector <CircleShape> shariki;

		for (int i = 0; i < vector_points.size(); i++)
		{

			shape.setPosition(vector_points[i][0] + 25, vector_points[i][1] + 25);
			shariki.push_back(shape);
			
		}
		board.draw_shariki(window, shariki);
	}
}

int main() {
	RenderWindow window(VideoMode(windowWidth, windowHeight), L"Шахматная доска", Style::Close);

	Board board;
	vector<unique_ptr<ChessPiece>> pieces;
	int index_figure_1;
	int index_figure_2;
	vector<vector<int>> vector_points;
	bool flag = false;
	PieceColor attacker = PieceColor::WHITE;

	pieces.push_back(make_unique<Pawn>(15 + tileSize * 0, 15 + tileSize * 6, PieceColor::WHITE));//0
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 1, 15 + tileSize * 6, PieceColor::WHITE));//1
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 2, 15 + tileSize * 6, PieceColor::WHITE));//2
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 3, 15 + tileSize * 6, PieceColor::WHITE));//3
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 4, 15 + tileSize * 6, PieceColor::WHITE));//4
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 5, 15 + tileSize * 6, PieceColor::WHITE));//5
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 6, 15 + tileSize * 6, PieceColor::WHITE));//6
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 7, 15 + tileSize * 6, PieceColor::WHITE));//7
	pieces.push_back(make_unique<Rook>(15 + tileSize * 0, 15 + tileSize * 7, PieceColor::WHITE));//8
	pieces.push_back(make_unique<Rook>(15 + tileSize * 7, 15 + tileSize * 7, PieceColor::WHITE));//9
	pieces.push_back(make_unique<Horse>(15 + tileSize * 1, 15 + tileSize * 7, PieceColor::WHITE));//10
	pieces.push_back(make_unique<Horse>(15 + tileSize * 6, 15 + tileSize * 7, PieceColor::WHITE));//11
	pieces.push_back(make_unique<Elephant>(15 + tileSize * 2, 15 + tileSize * 7, PieceColor::WHITE));//12
	pieces.push_back(make_unique<Elephant>(15 + tileSize * 5, 15 + tileSize * 7, PieceColor::WHITE));//13
	pieces.push_back(make_unique<Queen>(15 + tileSize * 3, 15 + tileSize * 7, PieceColor::WHITE));//14
	pieces.push_back(make_unique<King>(15 + tileSize * 4, 15 + tileSize * 7, PieceColor::WHITE));//15

	pieces.push_back(make_unique<Pawn>(15 + tileSize * 0, 15 + tileSize * 1, PieceColor::BLACK));//16
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 1, 15 + tileSize * 1, PieceColor::BLACK));//17
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 2, 15 + tileSize * 1, PieceColor::BLACK));//18
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 3, 15 + tileSize * 1, PieceColor::BLACK));//19
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 4, 15 + tileSize * 1, PieceColor::BLACK));//20
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 5, 15 + tileSize * 1, PieceColor::BLACK));//21
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 6, 15 + tileSize * 1, PieceColor::BLACK));//22
	pieces.push_back(make_unique<Pawn>(15 + tileSize * 7, 15 + tileSize * 1, PieceColor::BLACK));//23
	pieces.push_back(make_unique<Rook>(15 + tileSize * 0, 15 + tileSize * 0, PieceColor::BLACK));//24
	pieces.push_back(make_unique<Rook>(15 + tileSize * 7, 15 + tileSize * 0, PieceColor::BLACK));//25
	pieces.push_back(make_unique<Horse>(15 + tileSize * 1, 15 + tileSize * 0, PieceColor::BLACK));//26
	pieces.push_back(make_unique<Horse>(15 + tileSize * 6, 15 + tileSize * 0, PieceColor::BLACK));//27
	pieces.push_back(make_unique<Elephant>(15 + tileSize * 2, 15 + tileSize * 0, PieceColor::BLACK));//28
	pieces.push_back(make_unique<Elephant>(15 + tileSize * 5, 15 + tileSize * 0, PieceColor::BLACK));//29
	pieces.push_back(make_unique<Queen>(15 + tileSize * 3, 15 + tileSize * 0, PieceColor::BLACK));//30
	pieces.push_back(make_unique<King>(15 + tileSize * 4, 15 + tileSize * 0, PieceColor::BLACK));//31

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed)
				window.close();

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				Vector2i localPosition = Mouse::getPosition(window);
				int position_mous_x = 15 + tileSize * (localPosition.x / tileSize);
				int position_mous_y = 15 + tileSize * (localPosition.y / tileSize);
				int new_index_figure = defining_an_array_index(position_mous_x, position_mous_y, pieces);

				if (new_index_figure != -2 && ((attacker == PieceColor::WHITE && new_index_figure >= 0 && new_index_figure <= 15) || (attacker == PieceColor::BLACK && new_index_figure >= 16 && new_index_figure <= 31))) {
					vector_points.clear();
					index_figure_1 = new_index_figure;
					defining_an_array_index_and_points(localPosition.x, localPosition.y, pieces, vector_points, attacker);
					continue;
				}
				if (size(vector_points) == 0) {
					index_figure_1 = defining_an_array_index_and_points(localPosition.x, localPosition.y, pieces, vector_points, attacker);
				}
				else {
					int position_mous_x = 15 + tileSize * (localPosition.x / tileSize);
					int position_mous_y = 15 + tileSize * (localPosition.y / tileSize);
					index_figure_2 = defining_an_array_index(position_mous_x, position_mous_y, pieces);
					bool flag = true;
					for (int i = 0; i < vector_points.size(); ++i) {
						if (vector_points[i][0] == position_mous_x && vector_points[i][1] == position_mous_y) {
							flag = false;
							for (int j = 0; j < pieces.size(); ++j) {
								if (pieces[j]->comparison_cords(position_mous_x, position_mous_y)) {
									index_figure_2 = j;
									pieces[index_figure_2] = make_unique<ChessPiece>("неправильный", -1, -1, PieceColor::BLACK);
									pieces[index_figure_1]->movement(position_mous_x, position_mous_y);
									attacker = (attacker == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
									vector_points.clear();
									if (index_figure_2 == 31) {
										cout << "The black team won";
										window.close();
									}
									if (index_figure_2 == 15) {
										cout << "The white team won";
										window.close();
									}
									break;
								}
							}
							if (vector_points.size() == 0) {
								break;
							}
							pieces[index_figure_1]->movement(position_mous_x, position_mous_y);
							attacker = (attacker == PieceColor::WHITE) ? PieceColor::BLACK : PieceColor::WHITE;
							vector_points.clear();
							break;
						}
					}
				}
				vector_points.clear();
			}
		}
		window.clear();
		board.draw(window);
		for (const auto& piece : pieces) {
			piece->draw(window);
		}

		render(vector_points,window,board);

		window.display();
	}
	return 0;
}