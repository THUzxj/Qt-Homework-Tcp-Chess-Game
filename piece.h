#ifndef PIECE_H
#define PIECE_H

#include <QObject>
#include <QGraphicsItem>

class Manager;

class Piece : public QObject
{
    Q_OBJECT
protected:
    int _x,_y;
    bool _isSelected = 0;
    bool _color;
    QString _name;
    QGraphicsPixmapItem *_pixItem;
    Manager* _parent;
    int _abstractID;
public:
    explicit Piece(int x,int y,bool color,Manager *parent = nullptr);
    ~Piece();
    virtual bool canMove(int x2,int y2);
    virtual void move(int x2,int y2);
    virtual void ate();
    void setPic();
    void movePic();
    bool color();
    void clean();
    bool tryToMove(int,int);

    int x();
    int y();
    QString name();
    int abstractID();

    static void setNewGamePieces(Manager * parent,QList<Piece*>* ps);
};

class King : public Piece
{
    Q_OBJECT
private:
    bool isOrigin = 1;
public:
    King(int x,int y,bool color,Manager *parent = nullptr);
    bool canMove(int x2,int y2);
    int canMove2(int x2,int y2);
    void ate();
    void move(int x2,int y2);
signals:
    void checkMate(bool color);
    void translocation(int x,int y,int x0_car,int k);
};

class Queen : public Piece
{
public:
    Queen(int x,int y,bool color,Manager *parent = nullptr);
    bool canMove(int x2,int y2);
};

class Rook : public Piece
{
private:
    bool isOrigin = 1;
public:
    Rook(int x,int y,bool color,Manager *parent = nullptr);
    bool canMove(int x2,int y2);
    void move(int x2,int y2);
    bool origin();
};

class Bishop : public Piece
{
public:
    Bishop(int x,int y,bool color,Manager *parent = nullptr);
    bool canMove(int x2,int y2);
};

class Knight : public Piece
{
public:
    Knight(int x,int y,bool color,Manager *parent = nullptr);
    bool canMove(int x2,int y2);
};

class Pawn : public Piece
{
    Q_OBJECT
private:
    bool isOrigin = 1;
public:
    Pawn(int x,int y,bool color,Manager *parent = nullptr);
    bool canMove(int x2,int y2);
    void move(int x2,int y2);
signals:
    void pawnPromotion(int,int,int,int,int,int);
};

#endif // PIECE_H
