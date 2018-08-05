#include "Field.h"

Field::Field(int width, int height, int size, sf::Font &font){
  this->size = size;
  field.resize(width);
  for(auto &i : field)
    i.resize(height);
  for(unsigned i = 0; i < field.size(); i++)
    for(unsigned j = 0; j < field[0].size(); j++){
      field[i][j].x = i;
      field[i][j].y = j;
      field[i][j].tile.setSize(sf::Vector2f(size, size));
      field[i][j].tile.setPosition(i*size, j*size);
      field[i][j].text.setFont(font);
      field[i][j].text.setCharacterSize(size/1.5);
    }
}

bool Field::inField(int x, int y){
  return x >= 0 && y >= 0 && x < field.size() && y < field[0].size();
}

void Field::setUpField(){ // (re)set field for a new game
  for(unsigned i = 0; i < field.size(); i++){
    for(unsigned j = 0; j < field[0].size(); j++){
      field[i][j].shadeDelay = 0;
      field[i][j].value = 0;
      field[i][j].covered = true;
      field[i][j].marked = false;
      field[i][j].tile.setFillColor(sf::Color(230 - 40 * ((i+j)%2), 240 - 20 * ((i+j)%2), 240));
      field[i][j].text.setString("");
      field[i][j].text.setFillColor(sf::Color(60, 80, 80, 255));
      field[i][j].text.setPosition(i*size + size*0.30, j*size + size*0.07);
    }
  }
}

void Field::setUpGame(int x, int y, int m){
  NestedLoop3x3  // generate safe space
    if(inField(x + i, y + j))
      field[x + i][y + j].value = -1;

  while(m--){    // place mines
    int i, j;
    while(field[i = rand()%field.size()][j = rand()%field[0].size()].value);
    field[i][j].value = 9; // 9 == mine
  }

  for(int k = 0; k < field.size(); k++){  // place numbers
    for(int l = 0; l < field[0].size(); l++){
      if(field[k][l].value == 9) continue;
      field[k][l].value = 0; // erase -1 from safe space
      NestedLoop3x3
        field[k][l].value += inField(k + i, l + j) && field[k + i][l + j].value == 9;
    }
  }
}

bool Field::uncover(Cell &c, std::queue<Cell*> &q){

  bool explosion = 0;
  if(!c.marked){
    c.covered = false;
    switch(c.value){

      case 0:  // show empty area 'BSF'
        if(!c.shadeDelay){  // start queue to open field with a wave effect
          q.push(&c);
          c.shadeDelay = -1;
        }
        while(q.size()){
          NestedLoop3x3{
            if(inField(q.front()->x + i, q.front()->y + j) && !q.front()->value
              && !field[q.front()->x + i][q.front()->y + j].shadeDelay){
              field[q.front()->x + i][q.front()->y + j].shadeDelay = q.front()->shadeDelay + WAVE_EFFECT_DELAY;
              q.push(&field[q.front()->x + i][q.front()->y + j]);
            }
          }
          q.pop();
          if(q.size())
            explosion += uncover(field[q.front()->x][q.front()->y], q);
        }
      break;

      case 9:  // boom
        c.tile.setFillColor(sf::Color(255, 60, 60));
        explosion = 1;
      break;
    }
  }
  return explosion;
}

void Field::shading(Cell &c){
  if(c.shadeDelay >= 0)
    c.shadeDelay--;
  else switch(c.value){
      case 9: // special case: game is over, show mines
        c.text.setFillColor(c.text.getFillColor() + sf::Color(0, 0, 0, 4));
      break;

      default:
        if(c.tile.getFillColor().b > 70){
          c.tile.setFillColor(sf::Color(c.tile.getFillColor() - sf::Color(2, 2, 2, 2)));
          c.text.setString(c.value ? to_string(c.value) : to_string(""));
          c.text.setFillColor(sf::Color(255, std::max(255 - (c.value-1)*70, 0), std::max(255 - (c.value-1)*70, 0)));
        }
      break;
    }
}

void Field::endGameReveal(){
  for(auto &i : field)
    for(auto &c : i)
      if(c.value == 9){
        c.covered = false;
        c.text.setPosition(c.x * size + size * 0.2, c.y * size);
        c.text.setString(to_string("@"));
        c.text.setFillColor(c.text.getFillColor() - sf::Color(0, 0, 0, 255));
        if(!c.marked)
          c.text.setFillColor(sf::Color(140, 40, 40, 0));
      } else if(c.marked)
        c.text.setString(to_string("x"));
}
