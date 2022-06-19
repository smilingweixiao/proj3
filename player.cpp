#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <algorithm>

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
const int dep = 2;

struct Point {
    int x;
    int y;
};
Point best;

struct Node {
    std::array<std::array<int, SIZE>, SIZE> bd;    //bd
    int isline[SIZE][SIZE][4];
    int alpha, beta;
    Node() {
        alpha = INT32_MIN;
        beta = INT32_MAX;
        for(int i = 0; i < SIZE; i++) {
            for(int j = 0; j < SIZE; j++) {
                bd[i][j] = board[i][j];
            }
        }
    }
    Node(Node node, int x, int y, int color) {
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
                    if(!board[i][j]) continue;
                    new1_x = i+dx[k]; new2_x = i+dx[k]*2; 
                    new3_x = i+dx[k]*3; new4_x = i+dx[k]*4;
                    new1_y = j+dy[k]; new2_y = j+dy[k]*2;
                    new3_y = j+dy[k]*3; new4_y = j+dy[k]*4;
                    if(new2_x>=0 && new2_x<SIZE && new2_y>=0 && new2_y<SIZE && bd[i][j] == player) {
                        if(bd[i][j] == bd[new1_x][new1_y] && bd[i][j] == bd[new2_x][new2_y]){
                            val++;
                            isline[i][j][k] = isline[new1_x][new1_y][k] = isline[new2_x][new2_y][k] = 1;
                            if(new3_x >= 0 && new3_x < SIZE && new3_y >= 0 && new3_y < SIZE && bd[new3_x][new3_y] == bd[i][j]) {
                                isline[new3_x][new3_y][k] = 1;
                            }
                            if(new4_x >= 0 && new4_x < SIZE && new4_y >= 0 && new4_y < SIZE && bd[new4_x][new4_y] == bd[i][j]) {
                                isline[new4_x][new4_y][k] = 1;
                            }
                            
                        }
                    }
                    else if(new2_x>=0 && new2_x<SIZE && new2_y>=0 && new2_y<SIZE) {
                        if(bd[i][j] == bd[new1_x][new1_y] && bd[i][j] == bd[new2_x][new2_y]){
                            val--;
                            isline[i][j][k] = isline[new1_x][new1_y][k] = isline[new2_x][new2_y][k] = 1;
                            if(new3_x >= 0 && new3_x < SIZE && new3_y >= 0 && new3_y < SIZE) {
                                isline[new3_x][new3_y][k] = 1;
                            }
                            if(new4_x >= 0 && new4_x < SIZE && new4_y >= 0 && new4_y < SIZE) {
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
            fin >> board[i][j];
        }
    }
    if(player == BLACK) opponent = WHITE;
    else opponent = BLACK;
}
void write_valid_spot(std::ofstream& fout, std::ifstream& fin) {
    srand(time(NULL));
    int x, y, val;
    best.x = -1;
    best.y = -1;
    // Keep updating the output until getting killed.
    //while(true) {
        // Choose a random spot.
        //int x = (rand() % SIZE);
        //int y = (rand() % SIZE);
        val = INT32_MIN;
        //best.x = (rand() % SIZE);
        //best.y = (rand() % SIZE);
        Node root = Node();
        for(int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if(root.bd[i][j] != EMPTY) continue;
                val = max(val, alphabeta(Node(root, i, j, player), dep-1, false));
                if(val > root.alpha) {
                    root.alpha = val;
                    x = i;
                    y = j;
                }
                
            }
        }
        //if(best.x == x && best.y == y) {
        //    read_board(fin);
        //    continue;
        //}
        best.x = x; best.y = y;
        fout << x << " " << y << std::endl;
        // Remember to flush the output to ensure the last action is written to file.
        fout.flush();
    return;    
//    }
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