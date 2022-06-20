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

struct Node {
    std::array<std::array<int, SIZE>, SIZE> bd;    //bd
    int alpha, beta;
    int user;
    bool end;
    Point last_put;
    vector<int> possible_x;
    vector<int> possible_y;
    //compare
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

//判斷是否防禦
Point best_1;
Point best_2;

bool check() {
    //橫
    int lock = 0, count = 0, k = 0;
    for(int i = 0; i < SIZE; i++){
        lock = 0; count = 0;
        for(int j = 0; j < SIZE; j++) {
            if(board[i][j] == opponent) {
                if(j == 0 || j == SIZE-1) {
                    lock++;
                }
                count++;
            }
            if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i; best_2.y = j-4;
                    return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                if(lock==0 && count==2) {
                    if(j-4 >= 0) {
                        if(board[i][j-4] == opponent) {
                            best_1.x = i;
                            best_1.y = j-3;
                            return true;
                        }
                    }
                    if(j+1 < SIZE) {
                        if(board[i][j+1] == opponent) {
                            best_1.x = i;
                            best_1.y = j;
                            return true;
                        }
                    }
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
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
                if(j == 0 || j == SIZE-1) {
                    lock++;
                }
                count++;
            }
            if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j;
                    return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                if(lock==0 && count==2) {
                    if(i-4 >= 0) {
                        if(board[i-4][j] == opponent) {
                            best_1.x = i-3;
                            best_1.y = j;
                            return true;
                        }
                    }
                    if(i+1 < SIZE) {
                        if(board[i+1][j] == opponent) {
                            best_1.x = i;
                            best_1.y = j;
                            return true;
                        }
                    }
                }
                count = 0;
                lock = 0;
                
            }
            else {
                //死四
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
            }
            if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j-4;
                    return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                //活三變形
                if(lock==0 && count==2) {
                    if(j+1 < SIZE && i+1 <SIZE) {
                        if(board[i+1][j+1] == opponent) {
                            best_1.x = i;
                            best_1.y = j;
                            return true;
                        }
                    }
                    if(j-4 >= 0 && i-4 >= 0) {
                        if(board[i-4][j-4] == opponent) {
                            best_1.x = i-3;
                            best_1.y = j-3;
                            return true;
                        }
                    }
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
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
        lock = 0; count = 0;
        k = i;
        for(int j = 0; i < SIZE && j < SIZE; i++,j++) {
            if(board[i][j] == opponent) {
                if(j==0 || j==SIZE-1 || i==SIZE-1 || i==0) {
                    lock++;
                }
                count++;
            }
            if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j-4;
                    return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                if(lock==0 && count==2) {
                    if(j+1 < SIZE && i+1 <SIZE) {
                        if(board[i+1][j+1] == opponent) {
                            best_1.x = i;
                            best_1.y = j;
                            return true;
                        }
                    }
                    if(j-4 >= 0 && i-4 >= 0) {
                        if(board[i-4][j-4] == opponent) {
                            best_1.x = i-3;
                            best_1.y = j-3;
                            return true;
                        }
                    }
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
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
        lock = 0; count = 0;
        k = j;
        for(int i = 0; i < SIZE && j >= 0; i++,j--) {
            if(board[i][j] == opponent) {
                if(j==0 || j==SIZE-1 || i==0 || i==SIZE-1) {
                    lock++;
                }
                count++;
            }
            if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i-4; best_2.y = j+4;
                    return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                if(lock==0 && count==2) {
                    if(j-1 >= 0 && i+1 <SIZE) {
                        if(board[i+1][j-1] == opponent) {
                            best_1.x = i;
                            best_1.y = j;
                            return true;
                        }
                    }
                    if(j+4 < SIZE && i-4 >= 0) {
                        if(board[i-4][j+4] == opponent) {
                            best_1.x = i-3;
                            best_1.y = j+3;
                            return true;
                        }
                    }
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
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
    for(int i = 0; i < SIZE; i++){
        lock = 0; count = 0;
        k = i;
        for(int j = 1; i < SIZE && j >= 0 ; i++,j--) {
            if(board[i][j] == opponent) {
                if(j==0 || j==SIZE-1 || i==0 || i==SIZE-1) {
                    lock++;
                }
                count++;
            }
            if(board[i][j] == EMPTY) {
                //活三
                if(count == 3 && lock == 0) {
                    best_1.x = i; best_1.y = j;
                    best_2.x = i; best_2.y = j-4;
                    return true;
                }
                //死四
                else if(count == 4) {
                    best_1.x = i; best_1.y = j;
                    return true;
                }
                count = 0;
                lock = 0;
            }
            else {
                //死四
                if(lock == 1 && count == 4) {
                    best_1.x = i; best_1.y = j-5;
                    return true;
                }
                count = 0;
                lock = 1;
            }

        }
        i = k;
    }

    return false;
}

 
int depth = 3;
int first;

int alphabeta(Node node, int d) {
    node.which_to_go();
    if(node.end) {
        if(node.user == player) return INT32_MIN;
        else return INT32_MAX;
    }
    if(d == 0) return node.calculate();
    int len = node.possible_x.size();
    if(node.user == player) {
        int val = INT32_MIN;
        for(int i = 0; i < len; i++) {
            val = max(val, alphabeta(Node(node, node.possible_x[i], node.possible_y[i], player), d-1));
            node.alpha = max(node.alpha, val);
            if(node.alpha >= node.beta) break;
        }
        return val;
    }
    else {
        int val = INT32_MAX;
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
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
            if(board[i][j] != EMPTY) first = 0;
        }
    }
    if(player == BLACK) opponent = WHITE;
    else opponent = BLACK;
}


void write_valid_spot(std::ofstream& fout) {
    
    // Keep updating the output until getting killed.
    
    while(true) {
        // Choose a random spot.
        int x, y;
        Node root;
        best_1.x = -1; best_1.y = -1;
        best_2.x = -1; best_2.y = -1;
        if(first && player == BLACK) {
            x = 7, y = 7;
        }
        else {
            if(check()) {
                if(best_2.x != -1) {
                    Node n1 = Node(root, best_1.x, best_1.y, player);
                    int val_1 = n1.calculate();
                    Node n2 = Node(root, best_2.x, best_2.y, player);
                    int val_2 = n2.calculate();
                    if(val_1 >= val_2) {
                        x = best_1.x;
                        y = best_1.y;
                    }
                    else {
                        x = best_2.x;
                        y = best_2.y;
                    }
                    
                }
                else {
                    x = best_1.x;
                    y = best_1.y;
                }
            }
            else {
                int val = INT32_MIN;
                for(int i = 0; i < SIZE; i++) {
                    for(int j = 0; j < SIZE; j++) {
                        if(board[i][j] != EMPTY) continue;
                        val = max(val, alphabeta(Node(root, i, j, player), depth-1));
                    
                        if(val > root.alpha) {
                            x = i; y = j;
                            root.alpha = val;
                        }
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