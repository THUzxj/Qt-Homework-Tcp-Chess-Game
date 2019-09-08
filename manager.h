#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QTextStream>
#include <QTimer>

class MainWindow;
class EnterWindow;
class Grid;
class Piece;
class Client;
class Server;

struct Point{
    int x,y;
    Point()=default;
    Point(int _x,int _y):x(_x),y(_y){}
    bool operator==(Point b){return x==b.x && y==b.y;}
};

struct Command{
    QString str;
    QList<int> args;
};

class Manager : public QObject
{
    Q_OBJECT
private:
    //控件
    QGraphicsScene* _scene;
    QGraphicsRectItem * _background;
    MainWindow* _w;
    EnterWindow* _ew;
    QTimer _timer;

    //属性
    bool _meBlack = 0;    //0 for white,  1 for black
    bool _currentTurn = 0;//0 for white,  1 for black
    bool _networkMode;//    0 for server, 1 for client
    bool _isOngame = 0;
    bool _meCheck = 0;
    bool _youCheck = 0;

    qreal _gridLenth = 80;

    int _meTime,_youTime;

    //对象
    Piece* _selectedPiece = nullptr;
    Client* _client;
    Server* _server;
    QList<Piece*> pieces[2];
    Piece* king[2];
    Grid* _grids[9][9];

    //静态数据
    static QString colorName[2];
    static QString pieceName[6];

    template<class T>
        void addPiece(QTextStream&,int,QList<int>&);
    void removePiece(Piece* p);
    void resetLCD();
public:
    Manager(MainWindow * w,EnterWindow* ew);
    qreal getPointX(int x);
    qreal getPointY(int y);
    void gridSelect(int x,int y);
    void readMessage(QString str,QList<int> args);
    void writeMessage(QString str,QList<int> args);
    void writeMessage(QString str);
    void meTurn();
    void youTurn();
    void timeChange();
    void clearPieces();
    void clearGrids();
    void setGrids();
    void updateMeCheck();
    void updateYouCheck();
    bool isKingNoWay();
    bool isNoWay();

    //本地操作 主动动作
    void newGame();
    void start();
    void resign();
    void saveEndGame();
    void loadEndGame();
    void timeOut();
    void checkMate(int color);
    void setPawnPromotion(int,int,int,int,int,int);
    void translocation(int,int,int,int);
    void stalemate();

    //对方信息 被动动作
    void passive_newGame(int);
    void passive_start();
    void passive_loadEndGame(QList<int>);
    void passive_resignOK();
    void passive_resignSelect();
    void passive_resignCancel();
    void passive_timeOut();
    void passive_move(QList<int>);
    void passive_translocation(QList<int>);
    void passive_checkMate(int);
    void passive_pawnPromotion(QList<int>);

    void setClient(Client* c);
    void setServer(Server* s);
    void initNewBoard();

    Grid* grid(int x,int y);
    bool meBlack();
    bool meCheck();
    Piece* meKing();

signals:

    friend class Piece;
    friend class Grid;
};

#endif // MANAGER_H
