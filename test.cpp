#include <bits./stdc++.h>
using namespace std;

const int N=4, M=4;
const int totPieces = N*M;
int cnt=0;

struct edge{
    int type, id;
};

struct piece{
    edge edges[4];      // L, U, R, D
    int rotated=0;
};

pair<int, int> idxToTypeId(int idx){
    return {idx/2+1, idx%2+1};
}

int typeIdToIdx(int type, int id){
    int idx = (type-1)*2 + (id==2);
    return idx;
}

pair<int, int> idxToRowCol(int idx, int colSize){
    int row = idx/colSize, col = idx%colSize;
    return {row, col};
}

int rowColToIdx(int row, int col, int colSize){
    return row*colSize+col;
}

int rotatedIdx(int id, int rotated){
    return (id+rotated)%4;
}

void printPiece(piece &p){
    cout << "Left = (" << p.edges[rotatedIdx(0, p.rotated)].type << ", " << p.edges[rotatedIdx(0, p.rotated)].id << ")\n";
    cout << "Up = (" << p.edges[rotatedIdx(1, p.rotated)].type << ", " << p.edges[rotatedIdx(1, p.rotated)].id << ")\n";
    cout << "Right = (" << p.edges[rotatedIdx(2, p.rotated)].type << ", " << p.edges[rotatedIdx(2, p.rotated)].id << ")\n";
    cout << "Down = (" << p.edges[rotatedIdx(3, p.rotated)].type << ", " << p.edges[rotatedIdx(3, p.rotated)].id << ")\n";
}

void printPuzzle(vector<piece> &puzzle){
    for (int i=0; i<totPieces; i++){
        cout << "Piece " << i << '\n';
        printPiece(puzzle[i]);
        cout << '\n';
    }
}

vector<piece> genPuzzle(vector<edge> &arrangement){
    int ptr=0;
    vector<piece> curPuzzle(totPieces);
    for (int i=0; i<totPieces; i++){
        pair<int, int> p = idxToRowCol(i, M);
        int row = p.first, col = p.second;

        if (col==0) // assign L
            curPuzzle[i].edges[0] = {0, 0};
        else
            curPuzzle[i].edges[0] = curPuzzle[i-1].edges[2];    // cur.left = prev.right
        
        if (row==0) // assign U
            curPuzzle[i].edges[1] = {0, 0};
        else
            curPuzzle[i].edges[1] = curPuzzle[i-M].edges[3];    // cur.up = prevRow.down
        
        if (col==M-1) // assign R
            curPuzzle[i].edges[2] = {0, 0};
        else
            curPuzzle[i].edges[2] = arrangement[ptr++];
        
        if (row==N-1) // assign D
            curPuzzle[i].edges[3] = {0, 0};
        else
            curPuzzle[i].edges[3] = arrangement[ptr++];
    }
    return curPuzzle;
}

bool chkPieceInPuzzle(vector<piece> &puzzle, piece &p, int cur){
    pair<int, int> rowCol = idxToRowCol(cur, M);
    int row = rowCol.first, col = rowCol.second;

    if (col==0){ // check L
        if (p.edges[rotatedIdx(0, p.rotated)].type != 0)
            return 0;
    }
    else{
        piece &lastPiece = puzzle[cur-1];
        bool matchEdge = (p.edges[rotatedIdx(0, p.rotated)].type == lastPiece.edges[rotatedIdx(2, lastPiece.rotated)].type);
        bool oppoId = (p.edges[rotatedIdx(0, p.rotated)].id + lastPiece.edges[rotatedIdx(2, lastPiece.rotated)].id == 3);
        if (!matchEdge || !oppoId)
            return 0;
    }
    
    if (row==0){ // check U
        if (p.edges[rotatedIdx(1, p.rotated)].type != 0)
            return 0;
    }
    else{
        piece &lastPiece = puzzle[cur-M];
        bool matchEdge = (p.edges[rotatedIdx(1, p.rotated)].type == lastPiece.edges[rotatedIdx(3, lastPiece.rotated)].type);
        bool oppoId = (p.edges[rotatedIdx(1, p.rotated)].id + lastPiece.edges[rotatedIdx(3, lastPiece.rotated)].id == 3);
        if (!matchEdge || !oppoId)
            return 0;
    }
    
    if (col==M-1){ // check R
        if (p.edges[rotatedIdx(2, p.rotated)].type != 0)
            return 0;
    }
    else{
        if (p.edges[rotatedIdx(2, p.rotated)].type == 0)
            return 0;
    }
    
    if (row==N-1){ // check D
        if (p.edges[rotatedIdx(3, p.rotated)].type != 0)
            return 0;
    }
    else{
        if (p.edges[rotatedIdx(3, p.rotated)].type == 0)
            return 0;
    }

    return 1;
}

void produceNewPuzzle(vector<piece> &puzzle, vector<piece> &curPuzzle, vector<bool> &used, int cur){
    if (cur==totPieces){  // found!!!
        cout << "ORIGINAL:\n";
        printPuzzle(puzzle);
        cout << '\n';
        cout << "NEW:\n";
        printPuzzle(curPuzzle);
        cout << "\n\n\n";
    }
    else{
        for (int i=1; i<totPieces; i++){
            if (!used[i]){
                for (int r=0; r<4; r++){
                    puzzle[i].rotated = r;
                    if (chkPieceInPuzzle(curPuzzle, puzzle[i], cur)){
                        curPuzzle[cur] = puzzle[i];
                        used[i]=1;
                        produceNewPuzzle(puzzle, curPuzzle, used, cur+1);
                        used[i]=0;
                    }
                    puzzle[i].rotated = 0;
                }
            }
        }
    }
}

bool chkValid(vector<piece> &puzzle){
    vector<piece> curPuzzle(totPieces);
    curPuzzle[0] = puzzle[0];
    vector<bool> used(totPieces);
    used[0] = 1;
    produceNewPuzzle(puzzle, curPuzzle, used, 1);
}

void genArrangements(vector<edge> &arrangement, vector<bool> &used, int totEdges){
    if (arrangement.size() == totEdges){     // found candidate layout/arrangement
        vector<piece> puzzle = genPuzzle(arrangement);
        // cout << "Current layout\n";
        // printPuzzle(puzzle);
        ++cnt;
        if (cnt%10000000==0)
            cout << cnt << " Done\n";
        chkValid(puzzle);
    }
    else{
        for (int i=0; i<totEdges; i++){
            if (!used[i]){
                pair<int, int> p = idxToTypeId(i);
                int type = p.first, id = p.second;
                bool id1 = (id==1 && (type==1 || used[typeIdToIdx(type-1, 1)]));    // {type-1, 1} used before {type, 1}
                bool id2 = (id==2 && used[typeIdToIdx(type, 1)]);                   // {type, 1} used before {type, 2}
                if (id1 || id2){
                    used[i]=1;
                    arrangement.push_back({type, id});
                    genArrangements(arrangement, used, totEdges);
                    arrangement.pop_back();
                    used[i]=0;
                }
            }
        }
    }
}

signed main(){
    // ios::sync_with_stdio(0);
    // cin.tie(0);
    // cout.tie(0);

    int totEdges = (M-1)*N + (N-1)*M;     // number of horizontal edges + number of vertical edges
    vector<edge> arrangement;
    vector<bool> used(totEdges);
    genArrangements(arrangement, used, totEdges);
}