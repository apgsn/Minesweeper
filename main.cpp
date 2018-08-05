#include "Field.h"

int main(){
		int width, height, size, mines;
    if(!std::freopen("config.dat", "r", stdin)){
			std::cout << "Cannot load config.dat";
			return 0;
		}
		std::cin.ignore(1000, '\n');
		std::cin >> width >> height >> size >> mines;
		const float PROP_SIZE = size * (width/15.0f > 1.0f ? 1.0f : width/15.0f);
		const float PROP_TEXT_CONST = 0.33;
    sf::RenderWindow window(sf::VideoMode(width*size, height*size + 1.5*PROP_SIZE), "Minesweeper", sf::Style::Titlebar | sf::Style::Close);
    std::srand(std::time(NULL));
    sf::Clock clock;
    sf::Font font;
    font.loadFromFile("font/Roboto-Black.ttf");
    sf::Text taskbarTime("", font, PROP_TEXT_CONST * PROP_SIZE * 2);
		sf::Text taskbarMines("", font, PROP_TEXT_CONST * PROP_SIZE * 2);
    sf::Text taskbarRestart("Restart", font, PROP_TEXT_CONST * PROP_SIZE * 2);
    taskbarTime.setPosition(0.02 * width * size, height * size + PROP_TEXT_CONST * PROP_SIZE);
    taskbarMines.setPosition(0.32 * width * size, height * size + PROP_TEXT_CONST * PROP_SIZE);
    taskbarRestart.setPosition(width * size - 2.8 * PROP_SIZE, height * size + PROP_TEXT_CONST * PROP_SIZE);
		bool setUpNewGame = true, firstClick, gameOver, exploded, wannaRestart;
    int totalMarkers, totalUncoveredCells, timer;
		Field field(width, height, size, font);
  	std::queue<Cell*> q;

    while (window.isOpen()){
    	if(setUpNewGame){
    		totalMarkers = timer = gameOver = exploded = setUpNewGame = 0;
        firstClick = true;
        taskbarMines.setFillColor(sf::Color::White);
				field.setUpField();
    	}

    	sf::Vector2i pos = sf::Mouse::getPosition(window);

    	// mouse hovering on restart
    	if (pos.x - taskbarRestart.getPosition().x > 0
					&& pos.y - taskbarRestart.getPosition().y > -5
    			&& pos.x < window.getSize().x
					&& pos.y < window.getSize().y){
						taskbarRestart.setFillColor(sf::Color(250, 200, 100));
						wannaRestart = true;
			} else {
				taskbarRestart.setFillColor(sf::Color::White);
				wannaRestart = false;
			};

    	sf::Event event;
    	while(window.pollEvent(event)){
				switch (event.type){
					case sf::Event::Closed:
						window.close();
					break;

					case sf::Event::MouseButtonReleased: // user's click
						pos.x /= size;
						pos.y /= size;

						if(wannaRestart)
							setUpNewGame = true;

						if (event.mouseButton.button == sf::Mouse::Left && field.inField(pos.x, pos.y) && !gameOver) {
							if(field(pos.x, pos.y).covered && !field(pos.x, pos.y).marked){		// uncover new field
								if(firstClick){
									firstClick = false;
									clock.restart();
									field.setUpGame(pos.x, pos.y, mines);
								}
								exploded = field.uncover(field(pos.x, pos.y), q);
							}

							else if(!field(pos.x, pos.y).covered){	// uncover field (based on numbers and marks)
								int isItSafe = 0;
								NestedLoop3x3
									isItSafe += field.inField(pos.x + i, pos.y + j) && field(pos.x + i, pos.y + j).marked;
								if(isItSafe >= field(pos.x, pos.y).value)
									NestedLoop3x3
										exploded += field.inField(pos.x + i, pos.y + j) && field.uncover(field(pos.x + i, pos.y + j), q);
								}
							}
						if (event.mouseButton.button == sf::Mouse::Right && field.inField(pos.x, pos.y) && field(pos.x, pos.y).covered && !gameOver) {
							if(!field(pos.x, pos.y).marked){ 	// flagging / unflagging
								field(pos.x, pos.y).marked = true;
								field(pos.x, pos.y).text.setString("?");
								totalMarkers++;
							} else {
								field(pos.x, pos.y).marked = false;
								field(pos.x, pos.y).text.setString("");
								totalMarkers--;
							}
						}
					break;
				}
    	} // end events

			totalUncoveredCells = 0;
			for(unsigned i = 0; i < width; i++){
				for(unsigned j = 0; j < height; j++){
					totalUncoveredCells += !field(i, j).covered;
  				if(!field(i, j).covered)
						field.shading(field(i, j));
				}
			}

    	// check game status + updating taskbar
      if(!gameOver){
				if(exploded || totalUncoveredCells + mines == width * height){
					gameOver = true;
					field.endGameReveal();
				}
        if(!firstClick)
					timer = clock.getElapsedTime().asSeconds();
        taskbarTime.setString("Time: " + to_string(timer/60) + ":" + to_string(timer%60 < 10 ? "0" : "") + to_string(timer%60));
        taskbarMines.setString("Mines left: " + to_string(mines - totalMarkers) + "/" + to_string(mines));
			} else {
        taskbarMines.setString(exploded ? "You lose! D:" : "You win! :D");
        taskbarMines.setFillColor(exploded ? sf::Color(240, 150, 120) : sf::Color(150, 240, 120));
      }

      // draw window
      window.clear(sf::Color(40, 60, 90));
    	for(unsigned i = 0; i < width; i++){
          for(unsigned j = 0; j < height; j++){
            	window.draw(field(i, j).tile);
            	window.draw(field(i, j).text);
          }
      }
			window.draw(taskbarTime);
			window.draw(taskbarMines);
    	window.draw(taskbarRestart);
    	window.display();
    }
  return 0;
}
