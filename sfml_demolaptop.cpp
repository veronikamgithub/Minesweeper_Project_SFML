// SFML_Demo.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include <iostream>
#include <istream>
#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <unordered_map>
#include "TextureManager.h"
#include "Random.h"
using std::map;
using std::istringstream;
using std::getline;
using std::string;
using std::ifstream;
using namespace sf;
using std::cout;
using std::endl;
using std::vector;
using std::unordered_map;

struct Tile {
    bool isBomb = false;
    bool isFlag = false;
    bool isRevealed = false;
    unsigned int row = 0;
    unsigned int col = 0;
    unsigned int mineCount = 0;
};


struct GameOption {
    string title = "COP 3503C Minesweeper";
    unsigned int numRows = 0;
    unsigned int numCols = 0;
    unsigned int numMines = 0;

    GameOption(string path = "boards/config.cfg") {
        ReadFile(path);
    }

    void ReadFile(string path) {

        ifstream file(path);

        if (file.is_open()) {

            string data;

            getline(file, data);
            numCols = stoi(data);

            getline(file, data);
            numRows = stoi(data);

            getline(file, data);
            numMines = stoi(data);

        }
        else {
            cout << "can't open file" << endl;
        }
    }



    void PrintOptions() {
        cout << "Game Options\n";
        cout << "Cols: " << numRows << "\n";
        cout << "Rows: " << numCols << "\n";
        cout << "Mines: " << numMines << "\n";
    }
};


struct Board {
    bool gamestat = true;
    unsigned int configmines;
    bool debugmode = false;
    bool restart_game = false;
    bool gameOver = false;
    vector <vector<Tile>> grid;
    unsigned int numMines = 0;
    unsigned int numFlags = 0;
    unsigned int numRevealed = 0;
    Board(unsigned int numRows, unsigned int numCols, unsigned int numMines) {
        this->numMines = numMines;
        vector<vector<Tile>> board(numRows, vector<Tile>(numCols));
        for (auto i = 0; i < numRows; i++) {
            for (auto j = 0; j < numCols; j++) {
                board[i][j].row = i;
                board[i][j].col = j;
            }
        }
        grid = board;
        setMines(numMines);
    }
    void setMines(unsigned int numMines) {
        // get random row
        // get random col

        // try to set that tile to bomb, if it's not a bomb already
        int count = 0;
        while (count < numMines) {

            auto row = Random::Int(0, grid.size() - 1);
            auto col = Random::Int(0, grid[0].size() - 1);

            if (!grid[row][col].isBomb) {
                grid[row][col].isBomb = true;
                count++;
            }
            

        }


    }
    unsigned int numRows() {
        return grid.size();
    }
    unsigned int numCols() {
        return grid[0].size();
    }
    void print() {
        for (auto& row : grid) {
            for (auto& tile : row) {
                cout << (tile.isBomb ? "[*]" : "[ ]");
            }
            cout << "\n";
        }

    }
    void resetBoard() {
        unsigned int numRows = 0;
        unsigned int numCols = 0;
        unsigned int numMines = 0;
        vector <vector<Tile>> grid;

       
        


    }

    vector<Tile*> getTileNeighbors(Tile& tile) {
        vector<Tile*> neighbors;

        // check top neigbors
        if (tile.row > 0) {
            
            // has top left neighbor
            if (tile.col > 0) {
                neighbors.push_back(&grid[tile.row - 1][tile.col - 1]);
            }

            // direct top above us
            neighbors.push_back(&grid[tile.row - 1][tile.col]);

            // top right neighbor
            if (tile.col < numCols() - 1) {
                neighbors.push_back(&grid[tile.row - 1][tile.col + 1]);
            }
        }

        // direct left
        if (tile.col > 0) {
            neighbors.push_back(&grid[tile.row][tile.col - 1]);
        }

        // direct right neighbor
        if (tile.col < numCols() - 1) {
            neighbors.push_back(&grid[tile.row][tile.col + 1]);
        }

        // check top neigbors
        if (tile.row < numRows() - 1) {

            // has top left neighbor
            if (tile.col > 0) {
                neighbors.push_back(&grid[tile.row + 1][tile.col - 1]);
            }

            // direct top above us
            neighbors.push_back(&grid[tile.row + 1][tile.col]);

            // top right neighbor
            if (tile.col < numCols() - 1) {
                neighbors.push_back(&grid[tile.row + 1][tile.col + 1]);
            }
        }

        return neighbors;
    }

    void recursiveCheck(Tile& tile) {
        
        auto neighbors = getTileNeighbors(tile);

        for (auto& neighbor : neighbors) {
            if (neighbor->isBomb) {
                tile.mineCount++;
            }
        }

        if (tile.mineCount == 0) {
            for (auto& neighbor : neighbors) {
                if (!neighbor->isRevealed && !neighbor->isBomb && !neighbor->isFlag) {
                    neighbor->isRevealed = true;
                    numRevealed++;
                    recursiveCheck(*neighbor);
                }
            }
        }
        

    
    
    
    
    } //does this need to be a ptr

    bool isWinner() {
        return numRevealed == (numCols() * numRows()) - numMines;
    }

    bool draw(RenderWindow& window, unordered_map<string, sf::Sprite>& sprites) {
        sprites["face_happy"].setPosition(numCols() * 32.0f / 2 - 32, numRows() * 32.0f);
        sprites["face_lose"].setPosition(numCols() * 32.0f / 2 - 32, numRows() * 32.0f);
        sprites["face_win"].setPosition(numCols() * 32.0f / 2 - 32, numRows() * 32.0f);
        for (auto& row : grid) {

            for (auto& tile : row) {

                // always draw either tile hidden or revealed
                if (tile.isRevealed) {
                    sprites["tile_revealed"].setPosition(tile.col * 32.0f, tile.row * 32.0f);
                    window.draw(sprites["tile_revealed"]);
                    tile.isFlag = false;

                    if (tile.mineCount > 0) {
                        sprites["number_" + std::to_string(tile.mineCount)].setPosition(tile.col * 32.0f, tile.row * 32.0f);
                        window.draw(sprites["number_" + std::to_string(tile.mineCount)]);
                    }

                }
                else {
                    sprites["tile_hidden"].setPosition(tile.col * 32.0f, tile.row * 32.0f);
                    window.draw(sprites["tile_hidden"]);
                    if (tile.isFlag) {
                        sprites["flag"].setPosition(tile.col * 32.0f, tile.row * 32.0f);
                        window.draw(sprites["flag"]);
                        tile.isRevealed = false;
                    }
                }

                // if tile is flag
                if (tile.isFlag) {
                    sprites["flag"].setPosition(tile.col * 32.0f, tile.row * 32.0f);
                    window.draw(sprites["flag"]);

                }

                // change this later to only show bombs when you have debug on or the game is over

                if (tile.isBomb) {
                    sprites["mine"].setPosition(tile.col * 32.0f, tile.row * 32.0f);
                    if (tile.isRevealed) {
                        window.draw(sprites["mine"]);

                        //sprites["face_happy"].setTexture(TextureManager::GetTexture("face_lose"));
                        //window.draw(sprites["face_happy"]);

                    }
                    else if (debugmode && !isWinner()) {
                        window.draw(sprites["mine"]);
                    }
                    else if (isWinner()) {
                        tile.isFlag = true;
                            window.draw(sprites["flag"]);
                            
                        
                          //here either draw mine or flag?

                    }
                    else if (tile.isBomb && gameOver) {
                        window.draw(sprites["mine"]);  //how to make all the flags where the bombs are located
                    }
                }
                



            }






            //sprites["face_happy"].setPosition(numCols() * 32.0f / 2 - 32, numRows() * 32.0f);

            if (isWinner()) {
                window.draw(sprites["face_win"]);
            }
            else {
                if (gameOver) {
                    window.draw(sprites["face_lose"]);
                }
                else {
                    window.draw(sprites["face_happy"]);
                }
            }


            // find the ones, tens, and hundreds place for the flag counter
            int totalFlags = isWinner() ? 0 : (int)numMines - (int)numFlags;

            bool negativeTotal = totalFlags < 0;

            // ones place digit
            int ones = totalFlags % 10;
            totalFlags /= 10;

            // tens place digit
            int tens = totalFlags % 10;
            totalFlags /= 10;

            // hundreds place digit
            int hundreds = totalFlags % 10;

            // set position of one's place digit and draw
            auto onesPlace = "digits_" + std::to_string(abs(ones));
            sprites[onesPlace].setPosition(sprites["face_win"].getPosition().x - 32, sprites["face_win"].getPosition().y);
            window.draw(sprites[onesPlace]);

            // set position of tens place digit and draw
            auto tensPlace = "digits_" + std::to_string(abs(tens));
            sprites[tensPlace].setPosition(sprites["face_win"].getPosition().x - 53, sprites["face_win"].getPosition().y);
            window.draw(sprites[tensPlace]);

            // set position of hundreds place digit and draw
            auto hundredsPlace = "digits_" + std::to_string(abs(hundreds));
            sprites[hundredsPlace].setPosition(sprites["face_win"].getPosition().x - 74, sprites["face_win"].getPosition().y);
            window.draw(sprites[hundredsPlace]);

            // show negative sign for negatives
            if (negativeTotal) {
                sprites["digits_sign"].setPosition(sprites["face_win"].getPosition().x - 95, sprites["face_win"].getPosition().y);
                window.draw(sprites["digits_sign"]);
            }
           
            sprites["debug"].setPosition(numCols() * 32.0f / 1 - 300, numRows() * 32.0f);
            window.draw(sprites["debug"]);
            sprites["test_1"].setPosition(numCols() * 32.0f + 250 / 3 - 300, numRows() * 32.0f);
            window.draw(sprites["test_1"]);
            sprites["test_2"].setPosition(numCols() * 32.0f + 450 / 3 - 300, numRows() * 32.0f);
            window.draw(sprites["test_2"]);
            sprites["test_3"].setPosition(numCols() * 32.0f + 650 / 3 - 300, numRows() * 32.0f);
            window.draw(sprites["test_3"]);
           // sprites["digits"].setPosition(numCols() * 32.0f / 3-300, numRows() * 32.0f);
            //window.draw(sprites["digits"]);
           


            //tens hundred thousands place

        
            }
            return restart_game;
        }

    
};

Board loadTestBoard(string path, unsigned int numRows, unsigned int numCols) {
    
    Board board(numRows, numCols, 0);

    ifstream file("boards/" + path);

    if (file.is_open()) {

        string row;

        for (auto i = 0; i < numRows; i++) {

            getline(file, row);

            for (auto j = 0; j < numCols; j++) {
                if (row[j] == '1') {
                    board.grid[i][j].isBomb = true;
                    board.numMines++;
                }
            }
        }
    }

    return board;
    
}

    void renderLoop(RenderWindow & window, Board & board, unordered_map<string, sf::Sprite>&sprites) {
        // render loop
        // bool debugmode = false;
        bool restart_game = false;
        while (window.isOpen())
        {
            // event loop
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == Event::Closed)
                    window.close();
                else if (event.type == Event::MouseButtonPressed) {


                    if (event.mouseButton.button == Mouse::Left) {

                        int col = (int)Mouse::getPosition(window).x / 32.0f;
                        int row = (int)Mouse::getPosition(window).y / 32.0f;
                            
                            
                        //board.grid[row][col].isRevealed = true;
                        if (row < board.numRows() && col < board.numCols()) {

                            if (!board.gameOver) {
                                if (!board.grid[row][col].isRevealed && !board.grid[row][col].isFlag) {
                                    if (!board.grid[row][col].isBomb) {
                                        board.grid[row][col].isRevealed = true;
                                        board.numRevealed++;
                                        board.recursiveCheck(board.grid[row][col]);
                                    }
                                    else {
                                        board.gameOver = true;
                                        board.grid[row][col].isRevealed = true;
                                        //window.draw(sprites["mine"]);
                                        bool debugmode = 0;



                                    }
                                }
                            }
                        }
                        else {
                            auto x = sf::Mouse::getPosition(window).x;
                            auto y = sf::Mouse::getPosition(window).y;

                            bool debugClick = sprites["debug"].getGlobalBounds().contains(x, y);
                            if (debugClick) {
                                board.debugmode = !board.debugmode;

                                

                            }
                            else if (sprites["face_happy"].getGlobalBounds().contains(x,y)) {
                                GameOption option;
                                option.ReadFile("boards/config.cfg");
                                Board boardnew(option.numRows, option.numCols, option.numMines);
                                board = boardnew;
                                bool debug = board.debugmode;
                                //Board newBoard(board.numRows(), board.numCols(), board.numMines);
                                //board = newBoard;
                                board.debugmode = debug;
                            }
                            else if (sprites["test_1"].getGlobalBounds().contains(x, y)) {
                                
                                bool debug = board.debugmode;
                                board = loadTestBoard("testboard1.brd", board.numRows(), board.numCols());
                                board.debugmode = debug;
                                

                            }
                            else if (sprites["test_2"].getGlobalBounds().contains(x, y)) {
                                
                                bool debug = board.debugmode;
                                board = loadTestBoard("testboard2.brd", board.numRows(), board.numCols());
                                board.debugmode = debug;
                            }
                            else if (sprites["test_3"].getGlobalBounds().contains(x, y)) {
                                
                                bool debug = board.debugmode;
                                board = loadTestBoard("testboard3.brd", board.numRows(), board.numCols());
                                board.debugmode = debug;
                            }
                        }


                            
                            


                        // looks like the break is coming from here

                    }

                    else if (event.mouseButton.button == Mouse::Right) {
                        if (!board.gameOver) {
                            int col = (int)Mouse::getPosition(window).x / 32.0f;
                            int row = (int)Mouse::getPosition(window).y / 32.0f;
                            // flag tile
                            board.grid[row][col].isFlag = !board.grid[row][col].isFlag;
                            if (board.grid[row][col].isFlag) {
                                board.numFlags++;
                            }
                            else {
                                board.numFlags--;
                            }
                            
                        }


                    }


                }

                    
                    
                
        }
        window.clear();
        board.draw(window, sprites);
        window.display();
    }
}



    int main()
    {
            
        GameOption options;
        unordered_map<string, sf::Texture> textures;
        unordered_map<string, sf::Sprite> sprites;

        sprites["tile_hidden"].setTexture(TextureManager::GetTexture("tile_hidden"));
        sprites["tile_revealed"].setTexture(TextureManager::GetTexture("tile_revealed"));
        sprites["flag"].setTexture(TextureManager::GetTexture("flag"));
        sprites["mine"].setTexture(TextureManager::GetTexture("mine"));
        sprites["debug"].setTexture(TextureManager::GetTexture("debug"));
        sprites["face_happy"].setTexture(TextureManager::GetTexture("face_happy"));
        sprites["face_lose"].setTexture(TextureManager::GetTexture("face_lose"));
        sprites["face_win"].setTexture(TextureManager::GetTexture("face_win"));
        sprites["number_1"].setTexture(TextureManager::GetTexture("number_1"));
        sprites["number_2"].setTexture(TextureManager::GetTexture("number_2"));
        sprites["number_3"].setTexture(TextureManager::GetTexture("number_3"));
        sprites["number_4"].setTexture(TextureManager::GetTexture("number_4"));
        sprites["number_5"].setTexture(TextureManager::GetTexture("number_5"));
        sprites["number_6"].setTexture(TextureManager::GetTexture("number_6"));
        sprites["number_7"].setTexture(TextureManager::GetTexture("number_7"));
        sprites["number_8"].setTexture(TextureManager::GetTexture("number_8"));
        sprites["test_1"].setTexture(TextureManager::GetTexture("test_1"));
        sprites["test_2"].setTexture(TextureManager::GetTexture("test_2"));
        sprites["test_3"].setTexture(TextureManager::GetTexture("test_3"));
        sprites["digits"].setTexture(TextureManager::GetTexture("digits"));

        for (auto i = 0; i <= 10; i++) {
            string key = "digits_" + (i < 10 ? std::to_string(i) : "sign");
            sprites[key] = sprites["digits"];
            sprites[key].setTextureRect(sf::IntRect(i * 21, 0, 21, 32));
        }


        options.ReadFile("boards/config.cfg");
        Board board(options.numRows, options.numCols, options.numMines);
        // load board config to get numRows and numCols
        int x = options.numCols * 32;
        int y = (options.numRows * 32) + 100;


        RenderWindow window(sf::VideoMode(x, y), "Minesweeper");
        renderLoop(window, board, sprites);
        //if (restart_game == true) {
            //board_init(options.numCols, options.numRows, options.numMines);}

        return 0;
    }
    


// Run program: Ctrl + F5 or Debug > Start Without Debugg
// ing menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, Board board(4, 4)