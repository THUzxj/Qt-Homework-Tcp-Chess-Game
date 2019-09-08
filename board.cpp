#include "board.h"
#include "grid.h"

Board::Board(QObject *parent) : QObject(parent)
{
    for(int i = 1;i<=8;i++){
        for(int j = 1;j<=8;j++){
            _grids[i][j]=new Grid(i,j,_gridLenth,this);
        }
    }
}
