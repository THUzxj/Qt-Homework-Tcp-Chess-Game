#include "grid.h"
#include "piece.h"
#include "manager.h"
#include <QDebug>

int Grid::knight_x[8] = {1,2,2,1,-1,-2,-2,-1};
int Grid::knight_y[8] = {2,1,-1,-2,-2,-1,1,2};
int Grid::king_x[8] = {0,0,1,-1,1,1,-1,-1};
int Grid::king_y[8] = {1,-1,0,0,1,-1,-1,1};

GridItem::GridItem(qreal x,qreal y,qreal a,bool isBlack,Grid* parent):_isBlack(isBlack),_parent(parent)
{
    setPen(Qt::NoPen);
    if(_isBlack){
        setBrush(QColor(148,90,42));
    }else{
        setBrush(QColor(255,206,158));
    }
    setRect(0,0,a,a);
    setPos(x,y);
}

void GridItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    _parent->gridClick();
}

Grid::Grid(int x,int y,qreal a,Manager *parent) : QObject(parent),_x(x),_y(y),_a(a),_parent(parent)
{
    _isBlack = (qAbs(_y-_x)+1)%2;

    _item = new GridItem(_parent->getPointX(x),_parent->getPointY(y),_a,_isBlack,this);
    _parent->_scene->addItem(_item);
}

Grid::~Grid(){
    _parent->_scene->removeItem(_item);
    delete _item;
}

bool Grid::hasPiece(){
    return _currentPiece != nullptr;
}

bool Grid::pieceColor(){
    return _currentPiece->color();
}

void Grid::setPiece(Piece * p){
    _currentPiece = p;
}

Piece* Grid::piece(){
    return _currentPiece;
}

void Grid::gridClick(){
    qDebug()<<"press "<<_x<<" "<<_y;
    emit gridClicked(_x,_y);
}

void Grid::select(){
    _item->setBrush(QColor(255,0,0));
    _item->update();
}

void Grid::deselect(){
    if(_isBlack){
        _item->setBrush(QColor(148,90,42));;
    }else{
        _item->setBrush(QColor(255,206,158));
    }
    _item->update();
}

bool Grid::isAttacked(bool meBlack){
    Grid* g;
    if(isOnBoard(_x-1,_y+1-2*meBlack)){//pawn
        g = _parent->grid(_x-1,_y+1-2*meBlack);
        if(g->hasPiece() && g->pieceColor() != meBlack && g->piece()->name()=="pawn"){
            return true;
        }
    }
    if(isOnBoard(_x+1,_y+1-2*meBlack)){//pawn
        g = _parent->grid(_x+1,_y+1-2*meBlack);
        if(g->hasPiece() && g->pieceColor() != meBlack && g->piece()->name()=="pawn"){
            return true;
        }
    }
    qDebug()<<"not pawn";
    for(int i=0;i<8;i++){//knight
        if(isOnBoard(_x+knight_x[i],_y+knight_y[i])){
            g = _parent->grid(_x+knight_x[i],_y+knight_y[i]);
            if(g->hasPiece() && g->pieceColor()!=meBlack && g->piece()->name()=="knight"){
                return true;
            }
        }
    }
    qDebug()<<"not knight";
    for(int i=0;i<8;i++){//king
        if(isOnBoard(_x+king_x[i],_y+king_y[i])){
            g = _parent->grid(_x+king_x[i],_y+king_y[i]);
            if(g->hasPiece() && g->pieceColor()!=meBlack && g->piece()->name()=="king"){
                return true;
            }
        }
    }
    qDebug()<<"not king";
    int x,y;

    for(int i=0;i<4;i++){
        x = _x+king_x[i];
        y = _y+king_y[i];
        while(true){
            if(!isOnBoard(x,y))break;
            g = _parent->grid(x,y);
            if(g->hasPiece()){
                if(g->pieceColor()!=meBlack && (g->piece()->name()=="queen" || g->piece()->name()=="rook"))
                    return true;
                break;
            }
            x+=king_x[i];
            y+=king_y[i];
        }
    }
    qDebug()<<"not rook";
    for(int i=4;i<8;i++){
        x = _x+king_x[i];
        y = _y+king_y[i];
        while(true){
            if(!isOnBoard(x,y))break;
            g = _parent->grid(x,y);
            if(g->hasPiece()){
                if(g->pieceColor()!=meBlack && (g->piece()->name()=="bishop" || g->piece()->name()=="queen"))
                    return true;
                break;
            }
            x+=king_x[i];
            y+=king_y[i];
        }
    }
    qDebug()<<"not bishop queen";
    return false;
}

bool Grid::isKingNoWay(){
    int color = _currentPiece->color();
    for(int i=0;i<8;i++){
        if(isOnBoard(_x+king_x[i],_y+king_y[i])){
            if(!_parent->grid(_x+king_x[i],_y+king_y[i])->isAttacked(color)){
                return false;
            }
        }else{
            continue;
        }
    }
    return true;
}

bool Grid::isOnBoard(int x, int y){
    return x>=1 && y>=1 && x<=8 && y<=8;
}
