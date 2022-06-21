#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};
struct Point {
    int x;
    int y;
};
int dx[4] = {0,1,1,1};
int dy[4] = {1,0,1,-1};
int player;
int opponent;
const int SIZE = 15;
std::array<std::array<int, SIZE>, SIZE> board;
int depth = 3;
int first;
Point bl;

struct Node {
    std::array<std::array<int, SIZE>, SIZE> bd;    //bd
    int alpha, beta;
    int user;
    bool end;
    Point last_put;
    vector<int> possible_x;
    vector<int> possible_y;
    //compare

    void operator=(const Node& rhs) {
        for(int i = 0; i < SIZE; i++) {
            for(int j = 0; j < SIZE; j++) {
                bd[i][j] = rhs.bd[i][j];
            }
        }
        alpha = rhs.alpha; beta = rhs.beta;
        user = rhs.user;
        end = rhs.end;
        last_put.x = rhs.last_put.x;
        last_put.y = rhs.last_put.y;
        possible_x = rhs.possible_x;
        possible_y = rhs.possible_y;
    }
    Node (const Node& rhs) {
        for(int i = 0; i < SIZE; i++) {
            for(int j = 0; j < SIZE; j++) {
                bd[i][j] = rhs.bd[i][j];
            }
        }
        alpha = rhs.alpha; beta = rhs.beta;
        user = rhs.user;
        end = rhs.end;
        last_put.x = rhs.last_put.x;
        last_put.y = rhs.last_put.y;
        possible_x = rhs.possible_x;
        possible_y = rhs.possible_y;
    }
    Node() {
        alpha = INT32_MIN;
        beta = INT32_MAX;
        user = player;
        end = false;
        last_put.x = -1;
        last_put.y = -1;
        possible_x.push_back(-1);
        possible_y.push_back(-1);
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
        possible_x.push_back(-1);
        possible_y.push_back(-1);
    }
    
    //defense
    int calculate() {
        
        int val = 0;
        int flag = 1;
        for(int i = 0; i < 4; i++) {
            flag = 1;
            int new_x = last_put.x+dx[i], new_y = last_put.y+dy[i];
            int count = 1, lock = 2;
            while(new_x >= 0 && new_x < SIZE 
            && new_y >= 0 && new_y < SIZE) {
                if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                    new_x+=dx[i];
                    new_y+=dy[i];
                    count++;
                    if(count>=5) return INT32_MAX;
                }
                else if(bd[new_x][new_y] == EMPTY) {
                    lock--;
                    break;
                }
                else {
                    if(i==0 || i==1) {
                    //    val-=1;
                    }
                    else {
                    //    val-=2;
                    }
                    break;
                }
            }
            
            new_x = last_put.x-dx[i], new_y = last_put.y-dy[i];
            int flag_1 = 0, flag_2 = 0;
            while(new_x >= 0 && new_x < SIZE 
            && new_y >= 0 && new_y < SIZE) {
                if(bd[new_x][new_y] == bd[last_put.x][last_put.y]) {
                    new_x-=dx[i];
                    new_y-=dy[i];
                    count++;
                    if(count>=5) return INT32_MAX;
                }
                else if(bd[new_x][new_y] == EMPTY) {
                    lock--;
                    if(lock==0 && count ==4) return INT32_MAX;
                    else if(lock==0 && count ==3) val+=20;
                    else if(lock==1 && count==4) val+=20;
                    else if(lock==0 && count==1) {
                        if(new_x+4*dx[i]>=0 && new_x+4*dx[i]<SIZE && new_y+4*dy[i]>=0 && new_y+4*dy[i]<SIZE){
                            if(val>=2 && bd[new_x+4*dx[i]][new_y+4*dy[i]]==EMPTY && bd[new_x+3*dx[i]][new_y+3*dy[i]]==bd[last_put.x][last_put.y]) {
                                if(flag_2) {
                                    flag_2 = 0;
                                    val+=15;
                                }
                                flag_1 = 1;
                            }
                        }
                        if(new_x-4*dx[i]>=0 && new_x-4*dx[i]<SIZE && new_y-4*dy[i]>=0 && new_y-4*dy[i]<SIZE){
                            if(val>=2 && bd[new_x-4*dx[i]][new_y-4*dy[i]]==EMPTY && bd[new_x-3*dx[i]][new_y-3*dy[i]]==bd[last_put.x][last_put.y]) {
                                if(flag_2) {
                                    flag_2 = 0;
                                    val+=15;
                                }
                                flag_1 = 1;
                            }
                        }
                    }
                    else if(lock==0 && count==2){
                        val+=4;
                        if(flag_1) {
                            flag_1 = 0;
                            val+=15;
                        }
                        flag_2 = 1;
                    } 
                    else if(lock==1 && count==2 && (i==2||i==3)) val+=2;
                    else if(lock==1 && count==2) val+=1;
                    break;
                }
                else {
                    if(lock==1 && count==4) val+=20;
                    else if(lock==1 && count>=2 && (i==0 || i==1)) val+=1;
                    else if(lock==1 && count>=2) val+=2;
                    //else if(lock==1 && (i==0||i==1)) val-=1;
                    //else if(lock==1) val-=2;
                    break;
                }
            }
        
            if(flag) {
                if(lock==0 && count==4) val+=25;
            }
        
        }
        
        int enemys = 0;
        int count = 0;
        for(int i = 0; i < 4; i++) {

            int new_x = last_put.x+dx[i], new_y = last_put.y+dy[i];
            if(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) 
                if(bd[new_x][new_y]!=bd[last_put.x][last_put.y]&&bd[new_x][new_y]!=EMPTY) {
                    new_x+=dx[i]; new_y+=dy[i];
                    count++;
                }
                    
                
            if(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) 
                if(bd[new_x][new_y]!=bd[last_put.x][last_put.y]&&bd[new_x][new_y]!=EMPTY) {
                    new_x+=dx[i]; new_y+=dy[i];
                    count++;
                }
                    
            
            
            new_x = last_put.x-dx[i], new_y = last_put.y-dy[i];
            if(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) 
                if(bd[new_x][new_y]!=bd[last_put.x][last_put.y]&&bd[new_x][new_y]!=EMPTY) {
                    new_x-=dx[i]; new_y-=dy[i];
                    count++;
                }

            if(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) 
                if(bd[new_x][new_y]!=bd[last_put.x][last_put.y]&&bd[new_x][new_y]!=EMPTY) {
                    new_x-=dx[i]; new_y-=dy[i];
                    count++;
                }
                    
    
            if(count>=2) enemys++;
        }
        if(enemys >= 2) val+=30;
        else if(enemys >= 1) val+=15;
        return val;
    }
    void which_to_go() {
        possible_x.clear();
        possible_y.clear();
        //連線
        int new_x, new_y, count, lock, judge = 0;
        for(int i = 0; i < SIZE-4; i++) {
            for(int j = 0; j < SIZE-4; j++) {
                count = 1, lock = 2;
                for(int k = 0; k < 4; k++) {
                    if(bd[i][j] != EMPTY) break;
                    new_x = i+dx[k];
                    new_y = j+dy[k];
                    while(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) {
                        if(bd[new_x][new_y] != user) break;
                        count++;
                        new_x+=dx[k];
                        new_y+=dy[k];
                    }
                    new_x = i-dx[k];
                    new_y = j-dy[k];
                    while(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) {
                        if(bd[new_x][new_y] != user) break;
                        count++;
                        new_x-=dx[k];
                        new_y-=dy[k];
                    }
                    if(count >= 5) {
                        possible_x.clear();
                        possible_y.clear();
                        possible_x.push_back(i);
                        possible_y.push_back(j);
                        return;
                    }
                    
                }
            }
        }

        //堵路
        for(int k = 0; k < 4; k++) {
            count = 1, lock = 0;
            new_x = last_put.x+dx[k]; new_y = last_put.y+dy[k];
            while (new_x>=0 && new_x <SIZE && new_y>=0 && new_y<SIZE) {
                if(bd[new_x][new_y]==bd[last_put.x][last_put.y]) {
                    count++;
                }
                else if(bd[new_x][new_y]==EMPTY) {
                    if(count==4) {
                        judge++;
                        possible_x.clear(); possible_y.clear();
                        possible_x.push_back(new_x); possible_y.push_back(new_y);
                        break;
                    }
                }
                else {
                    lock++;
                    break;
                }
                new_x+=dx[k]; new_y+=dy[k];
            }
            new_x = last_put.x-dx[k]; new_y = last_put.y-dy[k];
            while (new_x>=0 && new_x <SIZE && new_y>=0 && new_y<SIZE) {
                if(bd[new_x][new_y]==bd[last_put.x][last_put.y]) {
                    count++;
                }
                else if(bd[new_x][new_y]==EMPTY) {
                    //活三
                    if(lock==0 && count==3) {
                        judge++;
                        possible_x.clear(); possible_y.clear();
                        possible_x.push_back(new_x); possible_y.push_back(new_y);
                        possible_x.push_back(new_x+4*dx[k]); possible_y.push_back(new_y+4*dy[k]);
                        break;
                    }
                    //死四
                    if(lock<=1 && count==4) {
                        judge++;
                        possible_x.clear(); possible_y.clear();
                        possible_x.push_back(new_x); possible_y.push_back(new_y);
                        break;
                    }
                    //變形活三
                    //活二
                    if(lock==0 && count==2) {
                        int new2_x = new_x-2*dx[k], new2_y = new_y-2*dy[k];
                        int new3_x = new_x+5*dx[k], new3_y = new_x+5*dy[k];
                        int new4_x = new_x+3*dx[k], new4_y = new_y+3*dy[k];
                        if(new2_x>=0 && new2_x<SIZE && new2_y>=0 && new2_y<SIZE) {
                            if(bd[new2_x][new2_y] == EMPTY) {
                                judge++;
                                possible_x.clear(); possible_y.clear();
                                possible_x.push_back(new_x); possible_y.push_back(new_y);
                                break;
                            }
                        }
                        if(new3_x>=0 && new3_x<SIZE && new3_y>=0 && new3_y<SIZE) {
                            if(bd[new3_x][new3_y] == EMPTY) {
                                judge++;
                                possible_x.clear(); possible_y.clear();
                                possible_x.push_back(last_put.x+dx[k]); possible_y.push_back(last_put.y+dy[k]);
                                break;
                            }
                        }
                        possible_x.push_back(new_x);
                        possible_y.push_back(new_y);
                        if(new4_x>=0 && new4_x<SIZE && new4_y>=0 && new4_y<SIZE) {
                            possible_x.push_back(new4_x);
                            possible_y.push_back(new4_y);
                        }
                    }
                }
                else {
                    //死四
                    lock++;
                    if(lock==1 && count==4) {
                        int new4_x = new_x+5*dx[k], new4_y = new_y+5*dy[k];
                        if(new4_x>=0 && new4_x<SIZE && new4_y>=0 && new4_y<SIZE) {
                            judge++;
                            possible_x.push_back(new4_x);  possible_y.push_back(new4_y);
                            break; 
                        }
                    }

                }
                new_x-=dx[k]; new_y-=dy[k];
            }
            if(judge>=2) {
                end = true;
                return;
            }
        }
        
        if(judge == 0) {
            for(int i = 0;i < SIZE;i++) {
                for(int j = 0;j < SIZE;j++) {
                    //if(bd[i][j] == EMPTY) {
                    //    possible_x.push_back(i);
                    //    possible_y.push_back(j);
                    //}
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
        else return;
        //全部空白
    }
};

//判斷是否防禦
Point best_1;//死四
Point best_2;//活三
Point best_3;//必勝
Point best_4;//空格
Node root = Node();

bool check() {

    Point douthr;
    best_1.x = -1; best_1.y = -1;
    best_2.x = -1; best_2.y = -1;
    best_3.x = -1; best_3.y = -1;
    best_4.x = -1; best_4.y = -1;
  
    douthr.x = -1;
    douthr.y = -1;
    //勝利
    int new_x, new_y, count, lock, k, douthree;
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            douthree = 0;
            for(int k = 0; k < 4; k++) {
                count = 1, lock = 2;
                if(board[i][j] != EMPTY) break;
                new_x = i+dx[k];
                new_y = j+dy[k];
                while(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) {
                    if(board[new_x][new_y] == EMPTY) lock--;
                    if(board[new_x][new_y] != player) break;
                    count++;
                    new_x+=dx[k];
                    new_y+=dy[k];
                }
                new_x = i-dx[k];
                new_y = j-dy[k];
                while(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) {
                    if(board[new_x][new_y] == EMPTY) lock--;
                    if(board[new_x][new_y] != player) break;
                    count++;
                    new_x-=dx[k];
                    new_y-=dy[k];
                }
                if(count >= 5) {
                    best_1.x = i;
                    best_1.y = j;
                    return true;
                }
                if(count>=3 && lock==0) {
                    if(count==4) {
                        best_3.x = i;
                        best_3.y = j;
                    }
                    if(douthree) {
                        douthr.x = i;
                        douthr.y = j;
                    }
                    douthree++;
                }
                    
            }
        }
    }

    //需擋有空格
    
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            douthree = 0;
            for(int k = 0; k < 4; k++) {
                count = 0, lock = 2;
                if(board[i][j] != EMPTY) break;
                new_x = i+dx[k];
                new_y = j+dy[k];
                while(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) {
                    if(board[new_x][new_y] == EMPTY) lock--;
                    if(board[new_x][new_y] != opponent) break;
                    count++;
                    new_x+=dx[k];
                    new_y+=dy[k];
                }
                new_x = i-dx[k];
                new_y = j-dy[k];
                while(new_x>=0 && new_x<SIZE && new_y>=0 && new_y<SIZE) {
                    if(board[new_x][new_y] == EMPTY) lock--;
                    if(board[new_x][new_y] != opponent) break;
                    count++;
                    new_x-=dx[k];
                    new_y-=dy[k];
                }
                if(count >= 4) {
                    best_1.x = i;
                    best_1.y = j;
                    return true;
                }
                //雙三
                if(count>=2 && lock==0) {
                    
                    if(douthree) {
                        best_1.x = i;
                        best_1.y = j;
                        //return true;
                    }
                    douthree++;
                }
                
                    
            }
        }
    }
    
    //橫
    lock = 0; count = 0; k = 0;
    for(int i = 0; i < SIZE; i++){
        lock = 0; count = 0;
        for(int j = 0; j < SIZE; j++) {
            if(board[i][j] == opponent) {
                if(j==0 || j==SIZE-1) {
                    lock++;
                }
                count++;
                if(lock==0 && count==4) {
                    best_1.x = i;
                    best_1.y = j-4;
                    //return true;
                }
            }
            else if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i; best_2.y = j-4;
                    //return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                if(lock==0 && count==2) {
                    if(j-5 >= 0) {
                        if(board[i][j-4]==opponent && board[i][j-5]==EMPTY) {
                            best_1.x = i;
                            best_1.y = j-3;
                            best_2.x = i;
                            best_2.y = j;
                            best_4.x = i;
                            best_4.y = j-5;
                            //return true;
                        }
                    }
                    if(j+2 < SIZE) {
                        if(board[i][j+1]==opponent && board[i][j+2]==EMPTY) {
                            best_1.x = i;
                            best_1.y = j;
                            best_2.x = i;
                            best_2.y = j+2;
                            best_4.x = i;
                            best_4.y = j-3;
                            //return true;
                        }
                    }
                    root.possible_x.push_back(i); root.possible_y.push_back(j);
                    root.possible_x.push_back(i); root.possible_y.push_back(j-3);
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
                lock++;
                if(lock == 1 && count == 4) {
                    best_1.x = i; best_1.y = j-5;
                    return true;
                }
                count = 0;
                lock = 1;
            }

        }
    }

    //豎
    lock = 0; count = 0;
    for(int j = 0; j < SIZE; j++){
        lock = 0; count = 0;
        for(int i = 0; i < SIZE; i++) {
            if(board[i][j] == opponent) {
                if(i==0 || i==SIZE-1) {
                    lock++;
                }
                count++;
                if(lock==0 && count==4) {
                    best_1.x = i-4;
                    best_1.y = j;
                    //return true;
                }
            }
            else if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j;
                    //return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                if(lock==0 && count==2) {
                    if(i-5 >= 0) {
                        if(board[i-4][j] == opponent && board[i-5][j]==EMPTY) {
                            best_1.x = i-3;
                            best_1.y = j;
                            best_2.x = i;
                            best_2.y = j;
                            best_4.x = i-5;
                            best_4.y = j;
                            //return true;
                        }
                    }
                    if(i+2 < SIZE) {
                        if(board[i+1][j] == opponent && board[i+2][j]==EMPTY) {
                            best_1.x = i;
                            best_1.y = j;
                            best_2.x = i+2;
                            best_2.y = j;
                            best_4.x = i-3;
                            best_4.y = j;
                            //return true;
                        }
                    }
                    root.possible_x.push_back(i); root.possible_y.push_back(j);
                    root.possible_x.push_back(i-3); root.possible_y.push_back(j);
                }
                count = 0;
                lock = 0;
                
            }
            else {
                //死四
                lock++;
                if(lock == 1 && count == 4) {
                    best_1.x = i-5; best_1.y = j;
                    return true;
                }
                count = 0;
                lock = 1;
            }

        }
    }

    //捺
    lock = 0; count = 0;
    for(int j = 0; j < SIZE; j++){
        lock = 0; count = 0;
        k = j;
        for(int i = 0; i < SIZE && j < SIZE; i++,j++) {
            if(board[i][j] == opponent) {
                if(i==0 || i==SIZE-1 || j==SIZE-1 || j==0) {
                    lock++;
                }
                count++;
                if(lock==0 && count==4) {
                    best_1.x = i-4;
                    best_1.y = j-4;
                    //return true;
                }
            }
            else if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j-4;
                    //return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                if(lock==0 && count==2) {
                    if(j+2 < SIZE && i+2 <SIZE) {
                        if(board[i+1][j+1] == opponent && board[i+2][j+2] == EMPTY) {
                            best_1.x = i;
                            best_1.y = j;
                            best_2.x = i+2;
                            best_2.y = j+2;
                            best_4.x = i-3;
                            best_4.y = j-3;
                            //return true;
                        }
                    }
                    if(j-5 >= 0 && i-5 >= 0) {
                        if(board[i-4][j-4] == opponent && board[i-5][j-5]==EMPTY) {
                            best_1.x = i-3;
                            best_1.y = j-3;
                            best_2.x = i;
                            best_2.y = j;
                            best_4.x = i-5;
                            best_4.y = j-5;
                            //return true;
                        }
                    }
                    root.possible_x.push_back(i); root.possible_y.push_back(j);
                    root.possible_x.push_back(i-3); root.possible_y.push_back(j-3);
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
                lock++;
                if(lock == 1 && count == 4) {
                    best_1.x = i-5; best_1.y = j-5;
                    return true;
                }
                count = 0;
                lock = 1;
            }

        }
        j = k;
    }
    
    lock = 0; count = 0;
    for(int i = 1; i < SIZE; i++){
        //cout<<"four"<<endl;
        lock = 0; count = 0;
        k = i;
        for(int j = 0; i < SIZE && j < SIZE; i++,j++) {
            if(board[i][j] == opponent) {
                if(j==0 || j==SIZE-1 || i==SIZE-1 || i==0) {
                    lock++;
                }
                count++;
                if(count==4 && lock==0) {
                    best_1.x = i-4;
                    best_1.y = j-4;
                    return true;
                }
            }
            else if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j-4;
                    //return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                if(lock==0 && count==2){
                    if(j+2 < SIZE && i+2 <SIZE) {
                        if(board[i+1][j+1] == opponent && board[i+2][j+2] == EMPTY) {
                            best_1.x = i;
                            best_1.y = j;
                            best_2.x = i+2;
                            best_2.y = j+2;
                            best_4.x = i-3;
                            best_4.y = j-3;
                            //return true;
                        }
                    }
                    if(j-5 >= 0 && i-5 >= 0) {
                        if(board[i-4][j-4] == opponent && board[i-5][j-5]==EMPTY) {
                            best_1.x = i-3;
                            best_1.y = j-3;
                            best_2.x = i;
                            best_2.y = j;
                            best_4.x = i-5;
                            best_4.y = j-5;
                            //return true;
                        }
                    }
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
                lock++;
                if(lock == 1 && count == 4) {
                    best_1.x = i-5; best_1.y = j-5;
                    return true;
                }
                count = 0;
                lock = 1;
            }

        }
        i = k;
    }

    //撇
    lock = 0; count = 0;
    for(int j = 0; j < SIZE; j++){
        //cout<<"five"<<endl;
        lock = 0; count = 0;
        k = j;
        for(int i = 0; i < SIZE && j >= 0; i++,j--) {
            if(board[i][j] == opponent) {
                if(j==0 || j==SIZE-1 || i==0 || i==SIZE-1) {
                    lock++;
                }
                count++;
                if(lock==0 && count==4) {
                    best_1.x = i-4;
                    best_1.y = j+4;
                    return true;
                }
            }
            else if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j+4;
                    //return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                else if(lock==0 && count==2) {
                    if(j-2 >= 0 && i+2 <SIZE) {
                        if(board[i+1][j-1] == opponent && board[i+2][j-2]) {
                            best_1.x = i;
                            best_1.y = j;
                            best_2.x = i+2;
                            best_2.y = j-2;
                            best_4.x = i-3;
                            best_4.y = j+3;
                            //return true;
                        }
                    }
                    if(j+5 < SIZE && i-5 >= 0) {
                        if(board[i-5][j+5] == opponent && board[i-5][j+5]) {
                            best_1.x = i-3;
                            best_1.y = j+3;
                            best_2.x = i-5;
                            best_2.y = j+5;
                            best_4.x = i;
                            best_4.y = j;
                            //return true;
                        }
                    }
                    root.possible_x.push_back(i); root.possible_y.push_back(j);
                    root.possible_x.push_back(i-3); root.possible_y.push_back(j+3);
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
                lock++;
                if(lock == 1 && count == 4) {
                    best_1.x = i-5; best_1.y = j+5;
                    return true;
                }
                count = 0;
                lock = 1;
            }

        }
        j = k;
    }

    lock = 0; count = 0;
    for(int i = 1; i < SIZE; i++){
        //cout<<"six"<<endl;
        lock = 0; count = 0;
        k = i;
        for(int j = SIZE-1; i < SIZE && j >= 0 ; i++,j--) {
            if(board[i][j] == opponent) {
                if(j==0 || j==SIZE-1 || i==0 || i==SIZE-1) {
                    lock++;
                }
                count++;
                if(lock==0 && count==4) {
                    best_1.x = i-4;
                    best_1.y = j+4;
                    return true;
                }
            }
            else if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j+4;
                    //return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                else if(lock==0 && count==2) {
                    if(j-1 >= 0 && i+1 <SIZE) {
                        if(board[i+1][j-1] == opponent) {
                            best_1.x = i;
                            best_1.y = j;
                            //return true;
                        }
                    }
                    if(j+4 < SIZE && i-4 >= 0) {
                        if(board[i-4][j+4] == opponent) {
                            best_1.x = i-3;
                            best_1.y = j+3;
                            //return true;
                        }
                    }
                    root.possible_x.push_back(i); root.possible_y.push_back(j);
                    root.possible_x.push_back(i-3); root.possible_y.push_back(j+3);
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
                lock++;
                if(lock == 1 && count == 4) {
                    best_1.x = i-5; best_1.y = j+5;
                    return true;
                }
                count = 0;
                lock = 1;
            }

        }
        i = k;
    }
    
    if(best_3.x != -1) {
        best_1.x = best_3.x;
        best_1.y = best_3.y;
        best_2.x = -1; best_2.y = -1;
        return true;
    }
    else if(best_1.x != -1) {
        return true;
    }

    else if(douthr.x != -1) {
        //cout<<"seven"<<endl;
        best_1.x = douthr.x;
        best_1.y = douthr.y;
        return true;
    }

    return false;
}


int alphabeta(Node node, int d) {
    node.which_to_go();
    if(node.end) {
        if(node.user == player) return INT32_MIN;
        else return INT32_MAX;
    }
    if(d == 0) return node.calculate();
    int len = node.possible_x.size();
    int ori = node.calculate();
    if(node.user == player) {
        int val = -ori;//INT32_MIN;
        for(int i = 0; i < len; i++) {
            val = max(val, alphabeta(Node(node, node.possible_x[i], node.possible_y[i], player), d-1));
            node.alpha = max(node.alpha, val);
            if(node.alpha >= node.beta) break;
        }
        return val;
    }
    else {
        int val = ori;//INT32_MAX;
        for(int i = 0; i < len; i++) {
            val = min(val, alphabeta(Node(node, node.possible_x[i], node.possible_y[i], opponent), d-1));
            node.beta = min(node.beta, val);
            if(node.alpha >= node.beta) break;
        }
        return val;
    }
}

void read_board(std::ifstream& fin) {
    first = 1;
    fin >> player;
    if(player == BLACK) opponent = WHITE;
    else opponent = BLACK;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
            if(board[i][j] == player) first = 0;
            if(board[i][j] == opponent) {
                bl.x = i;
                bl.y = j;
            }
        }
    }
    
}


void write_valid_spot(std::ofstream& fout) {
    
    // Keep updating the output until getting killed.
    
    while(true) {
        // Choose a random spot.
        int x, y;
        root = Node();
        best_1.x = -1; best_1.y = -1;
        best_2.x = -1; best_2.y = -1;
        if(first && player == BLACK) {
            x = 7, y = 7;
        }
        else if(first && player == WHITE) {
            if(bl.x+2>=0 && bl.x+2<SIZE && bl.y+2 >=0 && bl.y+2 < SIZE) {x = bl.x+2; y = bl.y+2;}
            else if(bl.x-2>=0 && bl.x-2<SIZE && bl.y-2 >=0 && bl.y-2 < SIZE) {x = bl.x-2; y = bl.y-2;}
        }
        else {
            
            if(check()) {
                if(best_2.x != -1) {
                    Node n1 = Node(root, best_1.x, best_1.y, player);
                    int val_1 = n1.calculate();
                    Node n2 = Node(root, best_2.x, best_2.y, player);
                    int val_2 = n2.calculate();
                    if(val_1 >= val_2) {
                        if(best_4.x!=-1) {
                            Node n4 = Node(root, best_4.x, best_4.y, player);
                            int val_4 = n4.calculate();
                            if(val_1>val_4) {
                                x = best_1.x; y = best_1.y;
                            }
                            else {
                                x = best_4.x; y = best_4.y;
                            }
                        }
                        else {
                            x = best_1.x;
                            y = best_1.y;
                        }
                        
                    }
                    else {
                        if(best_4.x!=-1) {
                            Node n4 = Node(root, best_4.x, best_4.y, player);
                            int val_4 = n4.calculate();
                            if(val_2>val_4) {
                                x = best_2.x; y = best_2.y;
                            }
                            else {
                                x = best_4.x; y = best_4.y;
                            }
                        }
                        else {
                            x = best_2.x;
                            y = best_2.y;
                        }   
                    }   
                }
                else {
                    x = best_1.x;
                    y = best_1.y;
                }
            }
            else {
                int val = INT32_MIN;
                root.possible_x.clear(); root.possible_y.clear();
                for(int i = 5;i < SIZE;i++) {
                for(int j = 0;j < SIZE;j++) {
                    //if(root.bd[i][j] == EMPTY) {
                    //    root.possible_x.push_back(i);
                    //    root.possible_y.push_back(j);
                    //}
                    if(root.bd[i][j] == player) {
                        if(i-1 >= 0) {
                            if(root.bd[i-1][j] == EMPTY) {
                                root.possible_x.push_back(i-1);
                                root.possible_y.push_back(j);
                            }
                        }
                        if(i+1 < SIZE) {
                            if(root.bd[i+1][j] == EMPTY) {
                                root.possible_x.push_back(i+1);
                                root.possible_y.push_back(j);
                            }
                        }
                        if(j-1 >= 0) {
                            if(root.bd[i][j-1] == EMPTY) {
                                root.possible_x.push_back(i);
                                root.possible_y.push_back(j-1);
                            } 
                        }
                        if(j+1 < SIZE) {
                            if(root.bd[i][j+1] == EMPTY) {
                                root.possible_x.push_back(i);
                                root.possible_y.push_back(j+1);
                            }
                        }
                        if(i-1 >= 0 && j-1 >= 0) {
                            if(root.bd[i-1][j-1] == EMPTY) {
                                root.possible_x.push_back(i-1);
                                root.possible_y.push_back(j-1);
                            }                            
                        }
                        if(i-1 >= 0 && j+1 < SIZE) {
                            if(root.bd[i-1][j+1] == EMPTY) {
                                root.possible_x.push_back(i-1);
                                root.possible_y.push_back(j+1);
                            }                            
                        }
                        if(i+1 < SIZE && j-1 >= 0) {
                            if(root.bd[i+1][j-1] == EMPTY) {
                                root.possible_x.push_back(i+1);
                                root.possible_y.push_back(j-1);
                            }                            
                        }
                        if(i+1 < SIZE && j+1 < SIZE) {
                            if(root.bd[i+1][j+1] == EMPTY) {
                                root.possible_x.push_back(i+1);
                                root.possible_y.push_back(j+1);
                            }
                        }
                    }
                }
                }
                for(int i = 0;i < 5;i++) {
                for(int j = 0;j < SIZE;j++) {
                    //if(root.bd[i][j] == EMPTY) {
                    //    root.possible_x.push_back(i);
                    //    root.possible_y.push_back(j);
                    //}
                    if(root.bd[i][j] == player) {
                        if(i-1 >= 0) {
                            if(root.bd[i-1][j] == EMPTY) {
                                root.possible_x.push_back(i-1);
                                root.possible_y.push_back(j);
                            }
                        }
                        if(i+1 < SIZE) {
                            if(root.bd[i+1][j] == EMPTY) {
                                root.possible_x.push_back(i+1);
                                root.possible_y.push_back(j);
                            }
                        }
                        if(j-1 >= 0) {
                            if(root.bd[i][j-1] == EMPTY) {
                                root.possible_x.push_back(i);
                                root.possible_y.push_back(j-1);
                            } 
                        }
                        if(j+1 < SIZE) {
                            if(root.bd[i][j+1] == EMPTY) {
                                root.possible_x.push_back(i);
                                root.possible_y.push_back(j+1);
                            }
                        }
                        if(i-1 >= 0 && j-1 >= 0) {
                            if(root.bd[i-1][j-1] == EMPTY) {
                                root.possible_x.push_back(i-1);
                                root.possible_y.push_back(j-1);
                            }                            
                        }
                        if(i-1 >= 0 && j+1 < SIZE) {
                            if(root.bd[i-1][j+1] == EMPTY) {
                                root.possible_x.push_back(i-1);
                                root.possible_y.push_back(j+1);
                            }                            
                        }
                        if(i+1 < SIZE && j-1 >= 0) {
                            if(root.bd[i+1][j-1] == EMPTY) {
                                root.possible_x.push_back(i+1);
                                root.possible_y.push_back(j-1);
                            }                            
                        }
                        if(i+1 < SIZE && j+1 < SIZE) {
                            if(root.bd[i+1][j+1] == EMPTY) {
                                root.possible_x.push_back(i+1);
                                root.possible_y.push_back(j+1);
                            }
                        }
                    }
                }
                }
                if(root.possible_x.empty()) {
                    for(int i = 0; i < SIZE; i++) {
                        for(int j = 0; j < SIZE; j++) {
                            if(root.bd[i][j] == EMPTY) {
                                root.possible_x.push_back(i);
                                root.possible_y.push_back(j);
                            }
                        }
                    }
                }
                int len = root.possible_x.size();
                for(int i = 0; i < len; i++) {
                    
                    val = max(val, alphabeta(Node(root, root.possible_x[i], root.possible_y[i], player), depth-1));
                    
                    if(val > root.alpha) {
                        x = root.possible_x[i]; y = root.possible_y[i];
                        root.alpha = val;
                    }
                    
                }
                
            }
            
        }
        if (board[x][y] == EMPTY) {
            fout << x << " " << y << std::endl;
            // Remember to flush the output to ensure the last action is written to file.
            fout.flush();
        }
    }
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}