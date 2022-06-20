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
const int dep = 3; //必須是奇數
int m, n;

struct Point {
    int x;
    int y;
};

struct Node {
    std::array<std::array<int, SIZE>, SIZE> bd;    //bd
    int isline[SIZE][SIZE][4];
    int alpha, beta;
    int user;
    bool end;
    Point last_put;
    vector<int> possible_x;
    vector<int> possible_y;
    Node() {
        alpha = INT32_MIN;
        beta = INT32_MAX;
        user = player;
        end = false;
        last_put.x = new_opponent.x;
        last_put.y = new_opponent.y;
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
        if(node.user == player) user = opponent;
        else user = player;
        end = node.end;
        last_put.x = x;
        last_put.y = y;
        possible_x = node.possible_x;
        possible_y = node.possible_y;
    }
    //defense
    int calculate() {
        int val = 0;
        int flag = 1;
        for(int i = 0; i < 4; i++) {
            flag = 1;
            int new_x = last_put.x+dx[i], new_y = last_put.y+dy[i];
            int count = 1, lock = 0;
            while(new_x >= 0 && new_x < SIZE 
            && new_y >= 0 && new_y < SIZE) {
                if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                    new_x+=dx[i];
                    new_y+=dy[i];
                    count++;
                }
                else if(bd[new_x][new_y] == EMPTY) {
                    break;
                }
                else {
                    if(i==0 || i==1) {
                        val-=1;
                    }
                    else {
                        val-=2;
                    }
                    lock++;
                    break;
                }
            }
            
            new_x = last_put.x-dx[i], new_y = last_put.y-dy[i];
            while(new_x >= 0 && new_x < SIZE 
            && new_y >= 0 && new_y < SIZE) {
                flag = 0;
                if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                    new_x-=dx[i];
                    new_y-=dy[i];
                    count++;
                }
                else if(bd[new_x][new_y] == EMPTY) {
                    if(lock==0 && count ==3) val+=20;
                    else if(lock==1 && count==4) val+=20;
                    else if(lock==0 && count==2) val+=2;
                    else if(lock==1 && count==2 && (i==2||i==3)) val+=2;
                    else if(lock==1 && count==2) val+=1;
                    break;
                }
                else {
                    lock++;
                    if(lock==1 && count==4) val+=20;
                    else if(lock==1 && count>=2 && (i==0 || i==1)) val+=1;
                    else if(lock==1 && count>=2) val+=2;
                    else if(lock==1 && (i==0||i==1)) val-=1;
                    else if(lock==1) val-=2;
                    break;
                }
            }
        
            if(flag) {
                if(lock==0 && count==4) val+=20;
            }
        
        }
        return val;
    }
    void which_to_go() {
        //defense
        possible_x.clear();
        possible_y.clear();
        int new_x, new_y;
        int count, lock, judge;
        //橫
        count = 1; lock = 0; judge = 0;
        new_x = last_put.x; new_y = last_put.y;
        new_y--;
        while(new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                count++;
                new_y--;
            }
            else if(bd[new_x][new_y] == user) {
                lock++;
                break;
            }
            else {
                break;
            }
        }
        if(count == 5) {
            end = true;
            return;
        }
        new_x = last_put.x; new_y = last_put.y;
        new_y++;
        while(new_y >= 0 && new_y < SIZE) {

            if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                count++;
                new_y++;
                if(count == 5) {
                    end = true;
                    return ;
                }
            }
            else if(bd[new_x][new_y] == user) {
                lock++;
                //死四
                if(lock==1 && count==4) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y-5);
                }
                break;
            }
            else {
                //死四
                if(lock==1 && count==4) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    break;
                }
                //活三
                if(lock==0 && count==3) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y-4);
                    break;
                }
                //活二
                if(lock==0 && count==2 && judge == 0) {
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y-3);
                }
                break;
            }
        }
        
        //豎
        count = 1; lock = 0;
        new_x = last_put.x; new_y = last_put.y;
        new_x--;
        while(new_x >= 0 && new_x < SIZE) {
            if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                count++;
                new_x--;
            }
            else if(bd[new_x][new_y] == user) {
                lock++;
                break;
            }
            else {
                break;
            }
        }
        if(count == 5) {
            end = true;
            return;
        }
        new_x = last_put.x; new_y = last_put.y;
        new_x++;
        while(new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                count++;
                new_x++;
                if(count == 5) {
                    end = true;
                    return;
                }
            }
            else if(bd[new_x][new_y] == user) {
                lock++;
                //死四
                if(lock == 1&&count==4) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x-5);
                    possible_y.push_back(new_y);
                }
                break;
            }
            else {
                //死四
                if(lock==1 && count==4) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    break;
                }
                //活三
                if(lock==0 && count==3) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x-4);
                    possible_y.push_back(new_y);
                    break;
                }
                //活二
                if(lock==0 && count==2 && judge == 0) {
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x-3);
                    possible_y.push_back(new_y);
                }
                break;
            }
        }
        if(judge == 2) {
            end = true;
            return;
        }
        //撇
        count = 1; lock = 0;
        new_x = last_put.x; new_y = last_put.y;
        new_x++; new_y--;
        while(new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                count++;
                new_x++;
                new_y--;
            }
            else if(bd[new_x][new_y] == user) {
                lock++;
                break;
            }
            else {
                break;
            }
        }
        if(count == 5) {
            end = true;
            return;
        }
        new_x = last_put.x; new_y = last_put.y;
        new_x--; new_y++;
        while(new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                count++;
                new_x--;
                new_y++;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                //死四
                if(lock==1 && count==4) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x+5);
                    possible_y.push_back(new_y-5);
                }
                break;
            }
            else {
                
                //死四
                if(lock==1 && count==4) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    break;
                }
                //活三
                if(lock==0 && count==3) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x+4);
                    possible_y.push_back(new_y-4);
                    break;
                }
                //活二
                if(lock==0 && count==2 && judge == 0) {
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x+3);
                    possible_y.push_back(new_y-3);
                }
                break;
            }
        }
        if(judge == 2) {
            end = true;
            return;
        }
        //捺
        count = 1; lock = 0;
        new_x = last_put.x; new_y = last_put.y;
        new_x++; new_y++;
        while(new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                count++;
                new_x++;
                new_y++;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                break;
            }
            else {
                break;
            }
        }
        if(count == 5) {
            end = true;
            return;
        }
        new_x = last_put.x; new_y = last_put.y;
        new_x--; new_y--;
        while(new_x >= 0 && new_x < SIZE && new_y >= 0 && new_y < SIZE) {
            if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                count++;
                new_x--;
                new_y--;
            }
            else if(bd[new_x][new_y] == player) {
                lock++;
                //死四
                if(lock==1 && count==4) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x+5);
                    possible_y.push_back(new_y+5);
                }
                break;
            }
            else {
                //死四
                if(lock==1 && count==4) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    break;
                }
                //活三
                if(lock==0 && count==3) {
                    judge++;
                    possible_x.clear();
                    possible_y.clear();
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x+4);
                    possible_y.push_back(new_y+4);
                    break;
                }
                //活二
                if(lock==0 && count==2 && judge == 0) {
                    possible_x.push_back(new_x);
                    possible_y.push_back(new_y);
                    possible_x.push_back(new_x+3);
                    possible_y.push_back(new_y+3);
                }
                break;
            }
        }
        if(judge == 2) {
            end = true;
            return;
        }
        if(possible_x.empty()) {
            for(int i = 0;i < SIZE;i++) {
                for(int j = 0;j < SIZE;j++) {
                    if(bd[i][j] == user) {
                        if(i-1 >= 0) {
                            if(bd[i-1][j] == EMPTY) {
                                possible_x.push_back(i-1);
                                possible_y.push_back(j);
                            }
                        }
                        if(i+1 < SIZE) {
                            if(bd[i+1][j] == EMPTY) {
                                possible_x.push_back(i+1);
                                possible_y.push_back(j);
                            }
                        }
                        if(j-1 >= 0) {
                            if(bd[i][j-1] == EMPTY) {
                                possible_x.push_back(i);
                                possible_y.push_back(j-1);
                            } 
                        }
                        if(j+1 < SIZE) {
                            if(bd[i][j+1] == EMPTY) {
                                possible_x.push_back(i);
                                possible_y.push_back(j+1);
                            }
                        }
                        if(i-1 >= 0 && j-1 >= 0) {
                            if(bd[i-1][j-1] == EMPTY) {
                                possible_x.push_back(i-1);
                                possible_y.push_back(j-1);
                            }                            
                        }
                        if(i-1 >= 0 && j+1 < SIZE) {
                            if(bd[i-1][j+1] == EMPTY) {
                                possible_x.push_back(i-1);
                                possible_y.push_back(j+1);
                            }                            
                        }
                        if(i+1 < SIZE && j-1 >= 0) {
                            if(bd[i+1][j-1] == EMPTY) {
                                possible_x.push_back(i+1);
                                possible_y.push_back(j-1);
                            }                            
                        }
                        if(i+1 < SIZE && j+1 < SIZE) {
                            if(bd[i+1][j+1] == EMPTY) {
                                possible_x.push_back(i+1);
                                possible_y.push_back(j+1);
                            }
                        }
                    }
                }
            }
        }

        if(possible_x.empty()) {
            //未完待續
        }
    
    }
};


Node root;
Point new_opponent;
Point each_update;

int alphabeta(Node node, int depth, bool user) {
    node.which_to_go();
    if(node.end) {
        if(node.user == player) return INT32_MIN;
        else return INT32_MAX;
    }
    if(depth == 0) return node.calculate();
    if(depth == dep) {
    
    }
    int val;
    if(user) {
        val = INT32_MIN;
        int len = node.possible_x.size();
        for(int i = 0; i < len; i++) {
            val = max(val, alphabeta(Node(node, node.possible_x[i], node.possible_y[i], player), depth-1, false));
            if(val > node.alpha && depth == dep) {
                    m = node.possible_x[i];
                    n = node.possible_y[i];
            }
            node.alpha = max(node.alpha, val);
            if(node.alpha >= node.beta) break;
        }
        return val;
    }
    else {
        val = INT32_MAX;
        int len = node.possible_x.size();
        for(int i = 0; i < len; i++) {
            val = min(val, alphabeta(Node(node, node.possible_x[i], node.possible_y[i], opponent), depth-1, true));
            node.beta = min(node.beta, val);
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