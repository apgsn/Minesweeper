#ifndef __FIELD_H_INCLUDED__
#define __FIELD_H_INCLUDED__

#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>

#define NestedLoop3x3 for(int i = -1; i <= 1; i++) for(int j = -1; j <= 1; j++)

template <typename T> // to_string not availabe on gcc
std::string to_string(T val){
  std::stringstream stream;
  stream << val;
  return stream.str();
}

struct Cell{
  int value, shadeDelay, x, y;
  bool covered, marked;
  sf::RectangleShape tile;
  sf::Text text;
};

class Field{
private:
  const int WAVE_EFFECT_DELAY = 13;
  int size;
  std::vector<std::vector<Cell> > field;
public:
  Field(int width, int height, int size, sf::Font &font);
  bool inField(int x, int y);
  void setUpField();
  void setUpGame(int x, int y, int m);
  bool uncover(Cell &c, std::queue<Cell*> &q);
  void shading(Cell &c);
  void endGameReveal();
  Cell& operator()(int x, int y) { return field[x][y]; }
};

#endif
