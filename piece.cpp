#include "piece.h"
#include "grid.h"
#include "manager.h"
#include "pawndialog.h"
#include <QDebug>

void Piece::setNewGamePieces(Manager* parent,QList<Piece*>* ps)
{
    Piece* p;
    //QList<Piece*> ps;
    for(int i=0;i<2;i++){//i 0 for white 1 for black
        int k = 2*i-1;
        p = new Rook(1,9*i-k,i,parent);
        ps[i].push_back(p);
        parent->_grids[1][9*i-k]->setPiece(p);

        p = new Rook(8,9*i-k,i,parent);
        ps[i].push_back(p);
        parent->_grids[8][9*i-k]->setPiece(p);

        p = new Knight(2,9*i-k,i,parent);
        ps[i].push_back(p);
        parent->_grids[2][9*i-k]->setPiece(p);

        p = new Knight(7,9*i-k,i,parent);
        ps[i].push_back(p);
        parent->_grids[7][9*i-k]->setPiece(p);

        p = new Bishop(3,9*i-k,i,parent);
        ps[i].push_back(p);
        parent->_grids[3][9*i-k]->setPiece(p);

        p = new Bishop(6,9*i-k,i,parent);
        ps[i].push_back(p);
        parent->_grids[6][9*i-k]->setPiece(p);

        p = new Queen(4,9*i-k,i,parent);
        ps[i].push_back(p);
        parent->_grids[4][9*i-k]->setPiece(p);

        p = new King(5,9*i-k,i,parent);
        ps[i].push_back(p);
        parent->_grids[5][9*i-k]->setPiece(p);
        parent->king[i] = p;

        for(int j=1;j<=8;j++){
            p = new Pawn(j,9*i-2*k,i,parent);
            ps[i].push_back(p);
            parent->_grids[j][9*i-2*k]->setPiece(p);
        }
    }
}

Piece::Piece(int x,int y,bool color,Manager* parent):QObject (parent),_x(x),_y(y),_color(color),_parent(parent)
{

}

bool Piece::tryToMove(int x2,int y2){
    Piece* p = _parent->_grids[x2][y2]->piece();
    _parent->_grids[_x][_y]->setPiece(nullptr);
    _parent->_grids[x2][y2]->setPiece(this);
    bool ans;
    if(_name=="king"){
        ans = !_parent->grid(x2,y2)->isAttacked(_parent->meKing()->color());
    }else
    ans = !_parent->grid(_parent->meKing()->x(),_parent->meKing()->y())->isAttacked(_parent->meKing()->color());
    _parent->_grids[_x][_y]->setPiece(this);
    _parent->_grids[x2][y2]->setPiece(p);

    return ans;
}

Piece::~Piece(){
    if(_pixItem)delete _pixItem;
}

bool Piece::canMove(int x2, int y2){//己方旗子
    if(_parent->_grids[x2][y2]->hasPiece() && _parent->_grids[x2][y2]->pieceColor()==_parent->_meBlack){
        return false;
    }
    return true;
}

void Piece::move(int x2,int y2)
{
    _parent->_grids[_x][_y]->setPiece(nullptr);
    _x = x2;
    _y = y2;
    movePic();
    _parent->_grids[_x][_y]->setPiece(this);
}

void Piece::ate()
{
    _parent->_scene->removeItem(_pixItem);
}

void Piece::clean(){
    _parent->_scene->removeItem(_pixItem);
}

void Piece::setPic(){
    QString str(":/pic/");
    QString cstr = _color? "black_":"white_";
    str.append(cstr).append(_name).append(".png");
    QPixmap pic(str);
    _pixItem = new QGraphicsPixmapItem;
    _pixItem->setTransformationMode(Qt::SmoothTransformation);
    _pixItem->setPos(_parent->getPointX(_x),_parent->getPointY(_y));
    _pixItem->setPixmap(pic.scaled(_parent->_gridLenth,_parent->_gridLenth));
    _parent->_scene->addItem(_pixItem);
}

void Piece::movePic(){
    _pixItem->setPos(_parent->getPointX(_x),_parent->getPointY(_y));
}

bool Piece::color()
{
    return _color;
}

int Piece::x(){
    return _x;
}

int Piece::y(){
    return _y;
}

QString Piece::name(){
    return _name;
}

int Piece::abstractID(){
    return _abstractID;
}

King::King(int x,int y,bool color,Manager *parent):Piece(x,y,color,parent){
    _name = "king";
    _abstractID = 0;
    setPic();
    if(!(x==5 && y==7*parent->meBlack()+1)){
        isOrigin = 0;
    }
    connect(this,&King::checkMate,parent,&Manager::checkMate);
    connect(this,&King::translocation,parent,&Manager::translocation);
}

bool King::canMove(int x2, int y2)
{
    if(!Piece::canMove(x2,y2))return false;
    if(isOrigin){
        if(qAbs(x2-_x)>2)return false;
        if(qAbs(x2-_x)==2){
            qDebug()<<"translocation OK0";
            if(qAbs(y2-_y)>0)return false;
            int x3;
            int k = (x2-_x)/qAbs(x2-_x);
            if(x2-_x<0){
                x3=1;
            }else{
                x3=8;
            }
            qDebug()<<"translocation OK1";
            if(!_parent->grid(x3,_y)->hasPiece() || _parent->grid(x3,_y)->piece()->name()!="rook"){
                return false;
            }
            if(!static_cast<Rook*>(_parent->grid(x3,_y)->piece())->origin()){
                return false;
            }

            for(int i=1;i<qAbs(x3-_x);i++){
                if(_parent->grid(_x+k*i,_y)->hasPiece()){//路上无子
                    return false;
                }
            }
            qDebug()<<"translocation OK2";
            if(_parent->grid(_x,_y)->isAttacked(_color)||_parent->grid(_x+k,_y)->isAttacked(_color)||_parent->grid(_x+2*k,_y)->isAttacked(_color))//不被攻击,没有被将军
                return false;
            qDebug()<<"translocation OK3";
            //emit translocation(_x,_y,x3,k);
            return true;
        }
    }

    if(qAbs(x2-_x)>1 || qAbs(y2-_y)>1)return false;
    if(_parent->grid(x2,y2)->isAttacked(_color))return false;
    if(!tryToMove(x2,y2))return false;
    return true;
}

int King::canMove2(int x2,int y2){
    if(!Piece::canMove(x2,y2))return false;
    if(isOrigin){
        if(qAbs(x2-_x)>2)return 0;
        if(qAbs(x2-_x)==2){
            qDebug()<<"translocation OK0";
            if(qAbs(y2-_y)>0)return 0;
            int x3;
            int k = (x2-_x)/qAbs(x2-_x);
            if(x2-_x<0){
                x3=1;
            }else{
                x3=8;
            }
            qDebug()<<"translocation OK1";
            if(!_parent->grid(x3,_y)->hasPiece() || _parent->grid(x3,_y)->piece()->name()!="rook"){
                return 0;
            }
            if(!static_cast<Rook*>(_parent->grid(x3,_y)->piece())->origin()){
                return 0;
            }

            for(int i=1;i<qAbs(x3-_x);i++){
                if(_parent->grid(_x+k*i,_y)->hasPiece()){//路上无子
                    return 0;
                }
            }
            qDebug()<<"translocation OK2";
            if(_parent->grid(_x,_y)->isAttacked(_color)||_parent->grid(_x+k,_y)->isAttacked(_color)||_parent->grid(_x+2*k,_y)->isAttacked(_color))//不被攻击,没有被将军
                return 0;
            qDebug()<<"translocation OK3";
            emit translocation(_x,_y,x3,k);
            return 2;
        }
    }

    if(qAbs(x2-_x)>1 || qAbs(y2-_y)>1)return 0;
    if(_parent->grid(x2,y2)->isAttacked(_color))return 0;
    if(!tryToMove(x2,y2))return 0;
    return 1;
}

void King::ate(){
    Piece::ate();
    emit checkMate(_color);
}

void King::move(int x2, int y2){
    isOrigin = 0;
    Piece::move(x2,y2);
}

Queen::Queen(int x,int y,bool color,Manager *parent):Piece(x,y,color,parent){
    _name = "queen";
    _abstractID = 1;
    setPic();
}

bool Queen::canMove(int x2, int y2)
{
    if(!Piece::canMove(x2,y2))return false;
    int delta_x = x2-_x, delta_y = y2-_y;
    if(delta_x == 0){
        int k = delta_y/qAbs(delta_y);
        for(int i=1;i<qAbs(delta_y);i++){
            if(_parent->grid(_x,_y+k*i)->hasPiece()){
                return false;
            }
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else if(delta_y == 0){
        int k = delta_x/qAbs(delta_x);
        for(int i=1;i<qAbs(delta_x);i++){
            if(_parent->grid(_x+k*i,_y)->hasPiece()){
                return false;
            }
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else if(delta_x == delta_y){
        int k = delta_x/qAbs(delta_x);
        for(int i=1;i<qAbs(delta_x);i++){
            if(_parent->grid(_x+k*i,_y+k*i)->hasPiece()){
                return false;
            }
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else if(delta_x == -delta_y){
        int k = delta_x/qAbs(delta_x);
        for(int i=1;i<qAbs(delta_x);i++){
            if(_parent->grid(_x+k*i,_y-k*i)->hasPiece()){
                return false;
            }
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else{
        return false;
    }

}

Rook::Rook(int x,int y,bool color,Manager *parent):Piece(x,y,color,parent){
    _name = "rook";
    _abstractID = 2;
    setPic();
    if(!((x==1 ||x==8) && y==parent->meBlack()*7+1)){
        isOrigin = 0;
    }
}

bool Rook::canMove(int x2, int y2)
{
    if(!Piece::canMove(x2,y2))return false;
    int delta_x = x2-_x, delta_y = y2-_y;
    if(delta_x == 0){
        int k = delta_y/qAbs(delta_y);
        for(int i=1;i<qAbs(delta_y);i++){
            if(_parent->grid(_x,_y+k*i)->hasPiece()){
                return false;
            }
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else if(delta_y == 0){
        int k = delta_x/qAbs(delta_x);
        for(int i=1;i<qAbs(delta_x);i++){
            if(_parent->grid(_x+k*i,_y)->hasPiece()){
                return false;
            }
        }

        if(!tryToMove(x2,y2))return false;
        return true;
    }else{
        return false;
    }

}

void Rook::move(int x2,int y2){
    isOrigin = 0;
    Piece::move(x2,y2);
}

bool Rook::origin(){
    return isOrigin;
}

Bishop::Bishop(int x,int y,bool color,Manager *parent):Piece(x,y,color,parent){
    _name = "bishop";
    _abstractID = 3;
    setPic();
}

bool Bishop::canMove(int x2, int y2)
{
    if(!Piece::canMove(x2,y2))return false;
    int delta_x = x2-_x, delta_y = y2-_y;
    if(delta_x == delta_y){
        int k = delta_x/qAbs(delta_x);
        for(int i=1;i<qAbs(delta_x);i++){
            if(_parent->grid(_x+k*i,_y+k*i)->hasPiece()){
                return false;
            }
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else if(delta_x == -delta_y){
        int k = delta_x/qAbs(delta_x);
        for(int i=1;i<qAbs(delta_x);i++){
            if(_parent->grid(_x+k*i,_y-k*i)->hasPiece()){
                return false;
            }
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else{
        return false;
    }
}

Knight::Knight(int x,int y,bool color,Manager *parent):Piece(x,y,color,parent){
    _name = "knight";
    _abstractID = 4;
    setPic();
}

bool Knight::canMove(int x2, int y2)
{
    if(!Piece::canMove(x2,y2))return false;
    int delta_x = x2-_x, delta_y = y2-_y;
    if(qAbs(delta_x)==2){
        if(qAbs(delta_y)!=1){
            return false;
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else if(qAbs(delta_x)==1){
        if(qAbs(delta_y)!=2){
            return false;
        }
        if(!tryToMove(x2,y2))return false;
        return true;
    }else{
        return false;
    }
}

Pawn::Pawn(int x,int y,bool color,Manager *parent):Piece(x,y,color,parent){
    _name = "pawn";
    _abstractID = 5;
    setPic();
    if(y!=5*parent->meBlack()+2){
        isOrigin = 0;
    }
    connect(this,&Pawn::pawnPromotion,parent,&Manager::setPawnPromotion);
}

bool Pawn::canMove(int x2, int y2)
{
    if(!Piece::canMove(x2,y2))return false;
    //吃过路兵 不实现
    if(x2!=_x)//斜走
    {
        if(qAbs(x2-_x)>1)return false;
        if(_parent->meBlack()){
            if(_y-y2>1 || _y-y2<=0)return false;
        }else{
            if(y2-_y>1 || y2-_y<=0)return false;
        }

        if(!_parent->grid(x2,y2)->hasPiece()){
            return false;
        }
    }else{//直走
        int limit = isOrigin ? 2 : 1;

        if(_parent->meBlack()){
            if(_y-y2>limit || _y-y2<=0)return false;
        }else{
            if(y2-_y>limit || y2-_y<=0)return false;
        }

        if(_parent->grid(x2,y2)->hasPiece()){
            return false;
        }
        if(isOrigin ==1 && qAbs(y2-_y)==2 && _parent->grid(_x,(_y+y2)/2)->hasPiece()){//不能越子
            return false;
        }
    }
    if(!tryToMove(x2,y2))return false;
    return true;
}

void Pawn::move(int x2,int y2){
    int x0 = _x,y0 = _y;
    Piece::move(x2,y2);
    isOrigin = 0;//相当于返回true一定会执行移动
    if(y2==8-7*_parent->meBlack()){//兵升变
        PawnDialog* dialog = new PawnDialog;
        int target = 1;
        if(dialog->exec()){
            target = dialog->value();
        }
        //qDebug()<<target<<" promotion";
        emit pawnPromotion(x0,y0,x2,y2,_color,target);
        delete dialog;
    }
}

