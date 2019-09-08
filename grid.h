#ifndef GRID_H
#define GRID_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class Piece;
class Manager;
class Grid;

class GridItem: public QGraphicsRectItem
{
private:
    bool _isBlack;
    Grid* _parent;
public:
    GridItem(qreal x,qreal y,qreal a,bool isBlack,Grid *parent);
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void setSelectedMode(bool mode);
};

class Grid : public QObject
{
    Q_OBJECT
private:
    int _x,_y;
    qreal _a;
    Piece* _currentPiece = nullptr;
    GridItem* _item;
    Manager* _parent;
    bool _isBlack;
    bool _isSelected;

    static int knight_x[8],knight_y[8];
    static int king_x[8],king_y[8];

public:
    explicit Grid(int x,int y,qreal a,Manager *parent = nullptr);
    ~Grid();

    bool hasPiece();
    bool pieceColor();// -1 null 0 white 1 black
    void setPiece(Piece*);
    void gridClick();
    void deselect();
    void select();
    bool isAttacked(bool meBlack);
    bool isKingNoWay();

    Piece* piece();

    static bool isOnBoard(int x,int y);

signals:
    void gridClicked(int x,int y);
public slots:
};

#endif // GRID_H
