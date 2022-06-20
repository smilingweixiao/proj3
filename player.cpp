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
    vector<int> possible_x;
    vector<int> possible_y;
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
    //defense
    void find_possiblity() {
        possible_x.clear();
        possible_y.clear();
        int new_x = new_opponent.x, new_y = new_opponent.y;
        int count, lock, judge;
        //橫
        count = 1; lock = 0; judge = 0;
        new_x = new_opponent.x; new_y = new_opponent.y;
        new_y--;
        while(new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == opponent) {
                count++;
                new_y--;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                judge++;
                break;
            }
            else {
                judge++;
                break;
            }
        }
        new_x = new_opponent.x; new_y = new_opponent.y;
        new_y++;
        while(new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == opponent) {
                count++;
                new_y++;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                judge++;
                //死四
                if(lock==1 && count==4) {
                    m = new_x;
                    n = new_y-5;
                    return;
                }
                break;
            }
            else {
                judge++;
                //死四
                if(lock==1 && count==4) {
                    m = new_x;
                    n = new_y;
                    return;
                }
                //活三
                if(lock==0 && count==3) {
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y-4);
                    return;
                }
                //活二
                if(lock==0 && count==2) {
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y-3);
                }
                break;
            }
        }

        
        //豎
        count = 1; lock = 0; judge = 0;
        new_x = new_opponent.x; new_y = new_opponent.y;
        new_x--;
        while(new_x >= 0 && new_x < SIZE) {
            if(bd[new_x][new_y] == opponent) {
                count++;
                new_x--;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                judge++;
                break;
            }
            else {
                judge++;
                break;
            }
        }
        new_x = new_opponent.x; new_y = new_opponent.y;
        new_x++;
        while(new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == opponent) {
                count++;
                new_x++;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                judge++;
                //死四
                if(lock == 1&&count==4) {
                    m = new_x-5;
                    n = new_y;
                    return;
                }
                break;
            }
            else {
                judge++;
                //死四
                if(lock==1 && count==4) {
                    m = new_x;
                    n = new_y;
                    return;
                }
                //活三
                if(lock==0 && count==3) {
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x-4);
                    possible_y.push_back(new_y);
                    return;
                }
                //活二
                if(lock==0 && count==2) {
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x-3);
                    possible_y.push_back(new_y);
                }
                break;
            }
        }


        //撇
        count = 1; lock = 0; judge = 0;
        new_x = new_opponent.x; new_y = new_opponent.y;
        new_x++; new_y--;
        while(new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == opponent) {
                count++;
                new_x++;
                new_y--;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                judge++;
                break;
            }
            else {
                judge++;
                break;
            }
        }
        new_x = new_opponent.x; new_y = new_opponent.y;
        new_x--; new_y++;
        while(new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == opponent) {
                count++;
                new_x--;
                new_y++;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                judge++;
                //死四
                if(lock==1 && count==4) {
                    m = new_x+5;
                    n = new_y-5;
                    return;
                }
                break;
            }
            else {
                judge++;
                //死四
                if(lock==1 && count==4) {
                    m = new_x;
                    n = new_y;
                    return;
                }
                //活三
                if(lock==0 && count==3) {
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x+4);
                    possible_y.push_back(new_y-4);
                    return;
                }
                //活二
                if(lock==0 && count==2) {
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x+3);
                    possible_y.push_back(new_y-3);
                }
                break;
            }
        }

    
        //捺
        count = 1; lock = 0; judge = 0;
        new_x = new_opponent.x; new_y = new_opponent.y;
        new_x++; new_y++;
        while(new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == opponent) {
                count++;
                new_x++;
                new_y++;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                judge++;
                break;
            }
            else {
                judge++;
                break;
            }
        }
        new_x = new_opponent.x; new_y = new_opponent.y;
        new_x--; new_y--;
        while(new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == opponent) {
                count++;
                new_x--;
                new_y--;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                judge++;
                //死四
                if(lock==1 && count==4) {
                    m = new_x+5;
                    n = new_y+5;
                    return;
                }
                break;
            }
            else {
                judge++;
                //死四
                if(lock==1 && count==4) {
                    m = new_x;
                    n = new_y;
                    return;
                }
                //活三
                if(lock==0 && count==3) {
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x+4);
                    possible_y.push_back(new_y+4);
                    return;
                }
                //活二
                if(lock==0 && count==2) {
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x+3);
                    possible_y.push_back(new_y+3);
                }
                break;
            }
        }
    
    }
    int calculate() {

    }

};

void check(int x, int y, int color) {
    if(color == player) {

    }
    else{

    }
}

Node root;
Point new_opponent;

int alphabeta(Node node, int depth, bool user) {
    if(depth == 0) return node.setHeuristic();
    if(depth == dep) {

    }
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
    bool flag = false;
    //要初始化空白棋盤
    while(true){
        int x, y;
        if(board[7][7] == EMPTY) {
            x = 7, y = 7;
        }
        else {
            flag = false;
            for(int i = 0; i < SIZE; i++) {
                for(int j = 0; j < SIZE; j++) {
                    if(root.bd[i][j] != board[i][j] && board[i][j] == opponent) {
                        flag = true;
                        new_opponent.x = i;
                        new_opponent.y = j;
                        break;
                    }
                }
                if(flag) break;
            }
            if(!flag) continue;
            root = Node();
            alphabeta(root,dep,true);
            int x = m;
            int y = n;
        }
        //蒲月局
        //花月
        /*else if(player == BLACK) {
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
        }*/
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