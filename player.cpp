#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <limits.h>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
using namespace std;

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

int player, opponent;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;

enum DIRCTION {
    VIRT = 0,
    HORI = 1,
    RL = 2,
    LR = 3
};
int dirx[4] = {-1,0,1,1};
int diry[4] = {0,1,-1,1};
int dep = 5; //initialize ,deeper win
vector<int> chess[15];//chess board
map<int,set<int>> blank;

struct Point {
    int x;
    int y;
    int dir[4];
    Point(int x, int y) :x(x),y(y) {
        dir[0] = dir[1] = dir[2] = dir[3] = 0;
    }
};
Point spot;

struct State_Value {
    void update_board(int x, int y, int color) {
        board[x][y] = color;
    }
    void restore_board(int x, int y) {
        board[x][y] = EMPTY;
    }
};

int setHeuristic() {
    int val;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point ij(i, j);
            for (int k = 0; k < 4; k++) {
                int new_x = i+2*dirx[k];
                int new_y = j+2*diry[k];
                if(ij.dir[k]) continue;
                if(!board[i][j]) continue;
                if(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) {
                    val+=1;
                    
                }
                else {

                }
            }
        }
    }
}

int val_alphabeta(int nx, int ny, int depth, int alpha, int beta, bool user) {
    int val;
    int flag = 0;
    if(depth == 0) return setHeuristic();
    if(user) {
        board[nx][ny] = player; //state
        val = INT32_MIN;
        for(auto row : blank) {
            for(auto col : row.second) {
                val = max(val, val_alphabeta(row.first, col, depth-1, alpha, beta, false));  
                if(alpha < val && alpha < beta) {    //choose to put
                    alpha = val;
                    spot.x = row.first;
                    spot.y = col; 
                }
                if(alpha >= beta) {
                    flag = 1;
                    break;
                }
                
            }
            if(flag) break;
        }
        board[nx][ny] = EMPTY;      //state
        return val;
    }
    else {
        board[nx][ny] = opponent;  //state
        val = INT32_MAX;
        for(auto row : blank) {
            for(auto col : row.second) {
                val = min(val, val_alphabeta(row.first, col, depth-1, alpha, beta, true));  
                beta = min(beta, val);
                if(alpha >= beta) {
                    flag = 1;
                    break;
                }
                
            }
            if(flag) break;
        }
        board[nx][ny] = EMPTY;         //state
        return val;
    }
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
            if(board[i][j]) blank.at(i).erase(j);
        }
    }
    if(player == BLACK) opponent = WHITE;
    else opponent = BLACK;
}

void write_valid_spot(std::ofstream& fout) {
    srand(time(NULL));
    int x, y;
    // Keep updating the output until getting killed.
    while(true) {
        // Choose a random spot.
        int x = (rand() % SIZE);
        int y = (rand() % SIZE);
        if (board[x][y] == EMPTY) {
            fout << spot.x << " " << spot.y << std::endl;
            // Remember to flush the output to ensure the last action is written to file.
            fout.flush();
        }
    }
}

int main(int, char** argv) {
    set<int> c;
    for(int i = 0; i < SIZE; i++) {
        c.insert(i);
    }
    for(int i = 0; i < SIZE; i++) {
        blank.insert(pair<int, set<int>>(i, c));
    }
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
