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

//int depth; //initialize ,deeper win
int dirx[8] = {0,0,1,1,1,-1,-1,-1};
int diry[8] = {1,-1,0,1,-1,0,1,-1};
vector<int> chess[15];//chess board
map<int,set<int>> blank;

struct Point {
    int x;
    int y;
};


struct State_Value {
    int alpha, beta;
};

int setHeuristic() {

}

int val_alphabeta(int nx, int ny, int depth, int alpha, int beta, bool user) {
    int val;
    int flag = 0;
    if(depth == 0) return setHeuristic();
    blank.at(nx).erase(ny);
    if(user) {
        board[nx][ny] = player;
        val = INT32_MIN;
        for(auto row : blank) {
            for(auto col : row.second) {
                val = max(val, val_alphabeta(row.first, col, depth-1, alpha, beta, false));  
                alpha = max(alpha, val);
                if(alpha >= beta) {
                    flag = 1;
                    break;
                }
                if(flag) break;
            }
        }
        board[nx][ny] = 0;
        return val;
    }
    else {
        board[nx][ny] = opponent;
        val = INT32_MAX;
        for(auto row : blank) {
            for(auto col : row.second) {
                val = min(val, val_alphabeta(row.first, col, depth-1, alpha, beta, true));  
                beta = min(beta, val);
                if(alpha >= beta) {
                    flag = 1;
                    break;
                }
                if(flag) break;
            }
        }
        board[nx][ny] = 0;
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
    if(player == 1) opponent = 2;
    else opponent = 1;
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
            fout << x << " " << y << std::endl;
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
