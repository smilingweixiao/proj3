#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <algorithm>
#include <set>

using namespace std;

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

int player;
int opponent;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;

int dx[4] = {0,1,1,1};
int dy[4] = {1,0,1,-1};
const int dep = 4;
int m, n;

struct Point {
    int x;
    int y;
};

struct Node {
    std::array<std::array<int, SIZE>, SIZE> bd;    //bd
    int isline[SIZE][SIZE][4];
    int alpha, beta;
    int cnt;
    Node() {
        cnt = 0;
        alpha = INT32_MIN;
        beta = INT32_MAX;
        for(int i = 0; i < SIZE; i++) {
            for(int j = 0; j < SIZE; j++) {
                bd[i][j] = board[i][j];
                if(bd[i][j] == player) cnt++;
            }
        }
    }
    Node(Node node, int x, int y, int color) {
        cnt = 0;
        for(int i = 0; i < SIZE; i++) {
            for(int j = 0; j < SIZE; j++) {
                bd[i][j] = node.bd[i][j];
            }
        }
        bd[x][y] = color;
        alpha = node.alpha; beta = node.beta;
    }
    int setHeuristic() {
        int val = 0;
        int new1_x, new2_x, new3_x, new4_x;
        int new1_y, new2_y, new3_y, new4_y;
        //initialize
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                for (int k = 0; k < 4; k++) {
                    isline[i][j][k] = 0;
                }
            }
        }
        //calculate
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                for (int k = 0; k < 4; k++) {
                    if(isline[i][j][k]) continue;
                    if(board[i][j] == EMPTY) continue;
                    new1_x = i+dx[k]; new2_x = i+dx[k]*2; 
                    new3_x = i+dx[k]*3; new4_x = i+dx[k]*4;
                    new1_y = j+dy[k]; new2_y = j+dy[k]*2;
                    new3_y = j+dy[k]*3; new4_y = j+dy[k]*4;
                    if(new2_x>=0 && new2_x<SIZE && new2_y>=0 && new2_y<SIZE && bd[i][j] == player) {
                        if(bd[i][j] == bd[new1_x][new1_y] && bd[i][j] == bd[new2_x][new2_y]){
                            val++;
                            isline[i][j][k] = isline[new1_x][new1_y][k] = isline[new2_x][new2_y][k] = 1;
                            if(new3_x >= 0 && new3_x < SIZE && new3_y >= 0 && new3_y < SIZE) {
                                if(bd[i][j] == bd[new3_x][new3_y])
                                    isline[new3_x][new3_y][k] = 1;
                            }
                            if(new4_x >= 0 && new4_x < SIZE && new4_y >= 0 && new4_y < SIZE) {
                                if(bd[i][j] == bd[new4_x][new4_y])
                                    isline[new4_x][new4_y][k] = 1;
                            }
                            
                        }
                    }
                    else if(new3_x>=0 && new3_x<SIZE && new3_y>=0 && new3_y<SIZE) {
                        if(bd[i][j] == bd[new1_x][new1_y] && bd[i][j] == bd[new2_x][new2_y]){
                            val--;
                            isline[i][j][k] = isline[new1_x][new1_y][k] = isline[new2_x][new2_y][k] = 1;
                            if(new4_x >= 0 && new4_x < SIZE && new4_y >= 0 && new4_y < SIZE) {
                                if(bd[i][j] == bd[new4_x][new4_y])
                                    isline[new4_x][new4_y][k] = 1;
                            }
                            
                        }
                    }
                }
            }
        }
        return val;
    }

};

int alphabeta(Node node, int depth, bool user) {
    if(depth == 0) return node.setHeuristic();
    int val;
    if(user) {
        val = INT32_MIN;
        for(int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if(node.bd[i][j] != EMPTY) continue;
                val = max(val, alphabeta(Node(node, i, j, player), depth-1, false));
                if(val > node.alpha && depth == dep) {
                    m = i;
                    n = j;
                    //cout << m << n << endl;
                }
                node.alpha = max(node.alpha, val);
                if(node.alpha >= node.beta) break;
            }
            if(node.alpha >= node.beta) break;
        }
        return val;
    }
    else {
        val = INT32_MAX;
        for(int i = 0; i < SIZE; i++) {
            for(int j = 0; j < SIZE; j++) {
                if(node.bd[i][j] != EMPTY) continue;
                val = min(val, alphabeta(Node(node, i, j, opponent), depth-1, true));
                node.beta = min(node.beta, val);
                if(node.alpha >= node.beta) break;
            }
            if(node.alpha >= node.beta) break;
        }
        return val;
    }
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
    if(player == BLACK) opponent = WHITE;
    else opponent = BLACK;
}
void write_valid_spot(std::ofstream& fout, std::ifstream& fin) {
    srand(time(NULL));
    //int x = -1, y = -1, val;
    // Keep updating the output until getting killed.
    
    while(true){
        
        Node node;
        alphabeta(node,dep,true);
        int x = m;
        int y = n;
        if(board[7][7] == EMPTY) {
            x = 7, y = 7;
        }
        //蒲月局
        //花月
        else if(player == BLACK) {
            if(node.cnt == 1) {
                if(board[8][8] == opponent){
                    x = 6; y = 8;
                }
                else if(board[6][8] == opponent) {
                    x = 6; y = 6;
                }
                else if(board[6][6] == opponent) {
                    x = 8; y = 6;
                }
                else if(board[8][6] == opponent) {
                    x = 8; y = 8;
                }

                else if(board[7][8] == opponent) {
                    x = 6; y = 8;
                }
                else if(board[6][7] == opponent) {
                    x = 6; y = 6;
                }
                else if(board[7][6] == opponent) {
                    x = 6; y = 6;
                }
                else if(board[8][7] == opponent) {
                    x = 8; y = 8;
                }
            }
        }
        if (board[x][y] == EMPTY) {
            fout << x << " " << y << std::endl;
            fout.flush();
        }  
    }
}
int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout,fin);
    fin.close();
    fout.close();
    return 0;
}