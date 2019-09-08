#include "manager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "enterwindow.h"
#include "grid.h"
#include "piece.h"
#include "network.h"
#include <QGraphicsItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QDialog>
#include <QRandomGenerator>
#include <QDialogButtonBox>


QString Manager::colorName[2] = {"white","black"};
QString Manager::pieceName[6] = {"king","queen","rook","bishop","knight","pawn"};

Manager::Manager(MainWindow* w,EnterWindow* ew):_w(w),_ew(ew)
{
    _scene = new QGraphicsScene;
    w->view->setScene(_scene);
    w->view->setSceneRect(QRectF(-_gridLenth*0.2,-_gridLenth*0.2,_gridLenth*8.4,_gridLenth*8.4));
    w->view->resize(_gridLenth*8.5,_gridLenth*8.5);
    w->view->move(0,30);
    w->resize(_gridLenth*8.5+150,_gridLenth*9+30);

    _background = new QGraphicsRectItem;
    _background->setPen(Qt::NoPen);
    _background->setBrush(QColor(205,110,52));
    _background->setRect(QRectF(-_gridLenth*0.2,-_gridLenth*0.2,_gridLenth*8.4,_gridLenth*8.4));

    setGrids();

    w->ui->startButton->setEnabled(false);
    w->ui->saveButton->setEnabled(false);
    w->ui->resignButton->setEnabled(false);

    connect(&_timer,&QTimer::timeout,this,&Manager::timeChange);

    connect(w->ui->newButton,&QPushButton::clicked,this,&Manager::newGame);
    connect(w->ui->actionxin,&QAction::triggered,this,&Manager::newGame);
    connect(w->ui->resignButton,&QPushButton::clicked,this,&Manager::resign);
    connect(w->ui->actionrenshu,&QAction::triggered,this,&Manager::resign);
    //connect(w->ui->sueButton,&QPushButton::clicked,this,&Manager::);
    connect(w->ui->saveButton,&QPushButton::clicked,this,&Manager::saveEndGame);
    connect(w->ui->actionbao,&QAction::triggered,this,&Manager::saveEndGame);
    connect(w->ui->loadButton,&QPushButton::clicked,this,&Manager::loadEndGame);
    connect(w->ui->actionzai,&QAction::triggered,this,&Manager::loadEndGame);
    connect(w->ui->startButton,&QPushButton::clicked,this,&Manager::start);
    connect(w->ui->actionkaishi,&QAction::triggered,this,&Manager::start);

    connect(ew,&EnterWindow::getClient,this,&Manager::setClient);
    connect(ew,&EnterWindow::getServer,this,&Manager::setServer);
}

qreal Manager::getPointX(int x)
{
    if(_meBlack){
        return _gridLenth*(8-x);
    }else{
        return _gridLenth*(x-1);
    }
}

qreal Manager::getPointY(int y)
{
    if(_meBlack){
        return _gridLenth*(y-1);
    }else{
        return _gridLenth*(8-y);
    }
}

void Manager::gridSelect(int x, int y)
{
    if(!_isOngame ||_currentTurn!=_meBlack){//不是本方回合
        return ;
    }

    if(_selectedPiece){
        //if(!_meCheck || (_meCheck && _selectedPiece==king[_meBlack])){
        if(_selectedPiece->name()=="king"){
            int flag = static_cast<King*>(_selectedPiece)->canMove2(x,y);
            if(flag==0){
                if(_grids[x][y]->hasPiece() && _grids[x][y]->pieceColor()==_meBlack){//select a another piece;
                    _grids[_selectedPiece->x()][_selectedPiece->y()]->deselect();
                    _grids[x][y]->select();
                    _selectedPiece=_grids[x][y]->piece();

                }else{//cancel the select
                    _grids[_selectedPiece->x()][_selectedPiece->y()]->deselect();
                    _selectedPiece = nullptr;
                }
            }else if(flag==1){
                QList<int> args;
                args.push_back(_selectedPiece->x());
                args.push_back(_selectedPiece->y());
                _grids[_selectedPiece->x()][_selectedPiece->y()]->deselect();
                if(_grids[x][y]->hasPiece()){
                    _grids[x][y]->piece()->ate();
                    removePiece(_grids[x][y]->piece());
                }
                _selectedPiece->move(x,y);//此时x,y已经修改
                _selectedPiece = nullptr;
                args.push_back(x);
                args.push_back(y);
                writeMessage("move",args);
                youTurn();
                return ;
            }else{
                //_grids[_selectedPiece->x()][_selectedPiece->y()]->deselect();
                _selectedPiece = nullptr;//取消选择
                return;
            }

        }else{
            if(_selectedPiece->canMove(x,y)){//move the piece
                QList<int> args;
                args.push_back(_selectedPiece->x());
                args.push_back(_selectedPiece->y());
                _grids[_selectedPiece->x()][_selectedPiece->y()]->deselect();
                if(_grids[x][y]->hasPiece()){
                    _grids[x][y]->piece()->ate();
                    removePiece(_grids[x][y]->piece());
                }
                _selectedPiece->move(x,y);//此时x,y已经修改
                _selectedPiece = nullptr;
                args.push_back(x);
                args.push_back(y);
                writeMessage("move",args);
                youTurn();
                return ;
            }else{
                if(_grids[x][y]->hasPiece() && _grids[x][y]->pieceColor()==_meBlack){//select a another piece;
                    _grids[_selectedPiece->x()][_selectedPiece->y()]->deselect();
                    _grids[x][y]->select();
                    _selectedPiece=_grids[x][y]->piece();

                }else{//cancel the select
                    _grids[_selectedPiece->x()][_selectedPiece->y()]->deselect();
                    _selectedPiece = nullptr;
                }
            }
        }



    }else{//select a piece
        if(_grids[x][y]->hasPiece() && _grids[x][y]->pieceColor()==_meBlack){
            _selectedPiece = _grids[x][y]->piece();
            _grids[x][y]->select();
        }
    }
}

void Manager::readMessage(QString str, QList<int> args){
    qDebug()<<str;
    for(int i:args){
        qDebug()<<i;
    }
    if(str=="move"){
        passive_move(args);
    }else if(str=="start"){
        passive_start();
    }else if(str=="new game"){
        passive_newGame(args[0]);
    }else if(str=="load"){
        passive_loadEndGame(args);
    }else if(str=="resign OK"){
        passive_resignOK();
    }else if(str=="resign cancel"){
        passive_resignCancel();
    }else if(str=="resign"){
        passive_resignSelect();
    }else if(str=="time out"){
        passive_timeOut();
    }else if(str=="translocation"){
        passive_translocation(args);
    }else if(str=="end"){
        passive_checkMate(args[0]);
    }else if(str=="stalemate"){
        stalemate();
    }else if(str=="pawn promotion"){
        passive_pawnPromotion(args);
    }
}

void Manager::writeMessage(QString str, QList<int> args){
    if(_networkMode){
        _client->sendMessage(str,args);
    }else{
        _server->sendMessage(str,args);
    }
}

void Manager::writeMessage(QString str){
    if(_networkMode){
        _client->sendMessage(str);
    }else{
        _server->sendMessage(str);
    }
}

void Manager::meTurn(){
    _timer.stop();
    updateYouCheck();
    updateMeCheck();
    if(isNoWay()){
        if(_meCheck){//将死
            checkMate(_meBlack);
        }else{//逼和
            writeMessage("stalemate");
            stalemate();

        }
    }
    _currentTurn = _meBlack;
    _w->ui->radioButton->setChecked(false);
    _w->ui->radioButton_2->setChecked(true);
    resetLCD();
    _timer.start(1000);
}

void Manager::youTurn(){
    qDebug()<<"youturn0";
    _timer.stop();
    updateMeCheck();
    qDebug()<<"youturn1";
    updateYouCheck();
    qDebug()<<"youturn2";
    _currentTurn = !_meBlack;
    _w->ui->radioButton->setChecked(true);
    _w->ui->radioButton_2->setChecked(false);
    resetLCD();
    _timer.start(1000);
}

void Manager::timeChange(){
    if(!_isOngame)return;
    if(_currentTurn == _meBlack){
        if(_meTime>0){
            _meTime--;
            _w->ui->meLCD->display(_meTime);
        }else{
            timeOut();
        }
    }else{
        if(_youTime>0){
            _youTime--;
            _w->ui->youLCD->display(_youTime);
        }
    }
}

void Manager::clearPieces(){
    for(Piece* p : pieces[0]){
        p->clean();
    }
    for(Piece* p : pieces[1]){
        p->clean();
    }

    pieces[0].clear();
    pieces[1].clear();
}

void Manager::clearGrids(){
    if(_grids[1][1]==nullptr){
        qDebug()<<"delete no grid";
        return ;
    }
    for(int i=1;i<=8;i++){
        for(int j=1;j<=8;j++){
            delete _grids[i][j];
        }
    }
}

void Manager::setGrids(){
    for(int i=1;i<=8;i++){
        for(int j=1;j<=8;j++){
            _grids[i][j] = new Grid(i,j,_gridLenth,this);
        }
    }

    for(int i=1;i<=8;i++){
        for(int j=1;j<=8;j++){
            connect(_grids[i][j],&Grid::gridClicked,this,&Manager::gridSelect);
        }
    }
}

void Manager::updateMeCheck(){
    _meCheck = _grids[king[_meBlack]->x()][king[_meBlack]->y()]->isAttacked(_meBlack);
    if(_meCheck == 0){
        _w->ui->radioButton_2->setText("ME");
    }else{
        _w->ui->radioButton_2->setText("CHECK");
    /*    if(isKingNoWay()){
            checkMate(_meBlack);
        }*/
    }
}

void Manager::updateYouCheck(){
    _youCheck = _grids[king[!_meBlack]->x()][king[!_meBlack]->y()]->isAttacked(!_meBlack);
    if(_youCheck == 0){
        _w->ui->radioButton->setText("OPPONENT");
    }else{
        _w->ui->radioButton->setText("CHECK");
    }
}

bool Manager::isKingNoWay(){
    return _grids[king[_meBlack]->x()][king[_meBlack]->y()]->isKingNoWay();
}

bool Manager::isNoWay(){
    for(auto p : pieces[_meBlack]){
        for(int i = 1;i<9;i++){
            for(int j=1;j<9;j++){
                if(p->canMove(i,j)){
                    return false;
                }
            }
        }
    }
    return true;
}

void Manager::removePiece(Piece *p){
    pieces[p->color()].removeOne(p);
}

void Manager::resetLCD(){
    _meTime = 60;
    _w->ui->meLCD->display(_meTime);
    _youTime = 60;
    _w->ui->youLCD->display(_youTime);
}

void Manager::newGame(){

    bool oppositeBlack = QRandomGenerator::global()->bounded(0,1);
    QList<int> l;
    l.push_back(oppositeBlack);
    _meBlack = !oppositeBlack;
    _currentTurn = 0;
    clearGrids();
    setGrids();

    clearPieces();
    Piece::setNewGamePieces(this,pieces);
    _w->ui->startButton->setEnabled(true);
    _w->ui->saveButton->setEnabled(true);

    writeMessage("new game",l);
}

void Manager::start(){
    _isOngame = 1;
    updateMeCheck();
    updateYouCheck();
    if(_currentTurn == _meBlack){
        _w->ui->radioButton->setChecked(false);
        _w->ui->radioButton_2->setChecked(true);
    }else{
        _w->ui->radioButton->setChecked(true);
        _w->ui->radioButton_2->setChecked(false);
    }
    resetLCD();
    _w->ui->resignButton->setEnabled(true);
    _timer.start(1000);
    writeMessage("start");
}

void Manager::resign(){
    if(QMessageBox::question(nullptr,"认输","确定要认输吗？")==QMessageBox::Yes){
        writeMessage("resign");
        QMessageBox::about(_w,"认输","认输请求已发送");
    }
}

void Manager::timeOut(){
    _isOngame = 0;
    writeMessage("time out");
    QMessageBox::critical(_w,"超时","你已超时！你输了。");
    _timer.stop();
}

void Manager::checkMate(int color){
    _isOngame = 0;
    _timer.stop();
    QList<int> l;
    l.push_back(color);
    writeMessage("end",l);
    if(color == _meBlack){   
        QMessageBox::about(_w,"结束","你输了。");
    }else{
        QMessageBox::about(_w,"结束","你赢了!");
    }

}

void Manager::setPawnPromotion(int x0,int y0,int x, int y, int color, int targetID){
    _grids[x][y]->piece()->ate();
    removePiece(_grids[x][y]->piece());
    Piece* p;
    switch(targetID){
    case 1: p = new Queen(x,y,color,this);break;
    case 2: p = new Rook(x,y,color,this);break;
    case 3: p = new Bishop(x,y,color,this);break;
    case 4: p = new Knight(x,y,color,this);break;
    }
    pieces[color].push_back(p);
    _grids[x][y]->setPiece(p);
    QList<int> args;
    args.push_back(x0);
    args.push_back(y0);
    args.push_back(x);
    args.push_back(y);
    args.push_back(targetID);
    writeMessage("pawn promotion",args);
}

void Manager::translocation(int x,int y,int x3,int k){
    QList<int> args;
    args.push_back(x);
    args.push_back(y);
    args.push_back(x3);
    args.push_back(k);
    _grids[x][y]->piece()->move(x+2*k,y);
    _grids[x3][y]->piece()->move(x+k,y);
    _grids[x][y]->deselect();
    _selectedPiece = nullptr;
    youTurn();
    writeMessage("translocation",args);
    qDebug()<<"trans6";
}

void Manager::stalemate(){
    _isOngame = 0;
    _timer.stop();
    QMessageBox::about(_w,"结束","逼和。");
}

template<class T>
void Manager::addPiece(QTextStream& in,int nowColor,QList<int>& a){//用于loadEndGame
    int num;
    in>>num;
    for(int i=0;i<num;i++){
        QString loc;
        in>>loc;
        int x = loc[0].toLatin1()-'a'+1;
        int y = loc[1].toLatin1()-'0';
        Piece* p = new T(x,y,nowColor,this);
        pieces[nowColor].push_back(p);
        _grids[x][y]->setPiece(p);
        a.push_back(x);
        a.push_back(y);
        a.push_back(p->abstractID());
        a.push_back(nowColor);
    }
}

void Manager::loadEndGame(){
    QString path = QFileDialog::getOpenFileName(_w,"读入残局",".","文本文档(*.txt)");
    if(!path.isEmpty()){
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
              QMessageBox::warning(nullptr, tr("读入残局"),
                                        tr("无法打开文件:\n%1").arg(path));
              return;
         }

        _w->ui->startButton->setEnabled(true);
        _w->ui->saveButton->setEnabled(true);

        clearPieces();
        _isOngame = 0;
        QTextStream in(&file);
        QString ct;
        QList<int> args;
        int nowColor;
        in>>ct;
         if(ct=="black"){
             _currentTurn = 1;
             _meBlack = 1;
             nowColor = 1;
             args.push_back(0);//对方执白
         }else if(ct=="white"){
             _currentTurn = 0;
             _meBlack = 0;
             nowColor = 0;
             args.push_back(1);
         }else {
             qDebug()<<"no white black";
             return ;
         }
         clearGrids();
         setGrids();
         while(!in.atEnd()){
             QString command;
             in>>command;
             if(command=="black"){
                 nowColor=1;
             }else if(command=="white"){
                 nowColor=0;
             }else if(command=="king"){
                 int num;
                 in>>num;
                 for(int i=0;i<num;i++){
                     QString loc;
                     in>>loc;
                     int x = loc[0].toLatin1()-'a'+1;
                     int y = loc[1].toLatin1()-'0';
                     Piece* p = new King(x,y,nowColor,this);
                     king[nowColor] = p;
                     pieces[nowColor].push_back(p);
                     _grids[x][y]->setPiece(p);
                     args.push_back(x);
                     args.push_back(y);
                     args.push_back(p->abstractID());
                     args.push_back(nowColor);
                 }
             }else if(command=="queen"){
                 addPiece<Queen>(in,nowColor,args);
             }else if(command=="rook"){
                 addPiece<Rook>(in,nowColor,args);
             }else if(command=="bishop"){
                 addPiece<Bishop>(in,nowColor,args);
             }else if(command=="knight"){
                 addPiece<Knight>(in,nowColor,args);
             }else if(command=="pawn"){
                 addPiece<Pawn>(in,nowColor,args);
             }
         }
         file.close();
         writeMessage("load",args);
    }
}

void Manager::saveEndGame(){
    QString path = QFileDialog::getSaveFileName(_w,"保存残局",".","文本文档(*.txt)");
    if(!path.isEmpty()){
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::critical(nullptr,"错误",QString("创建文件失败:\n%1").arg(path));
            return;
        }
        QTextStream out(&file);
        int a,i,k;
        if(_currentTurn==0){
            a=1;
            i=0;
            k=0;
        }else{
            a=-1;
            i=1;
            k=0;
        }
        for(;k<2;i+=a,k++){
            out<<colorName[i]<<"\n";
            QList<Piece*> pieceAll = pieces[i];
            QList<Piece*> pieceKind[6];
            for(int j = 0;j<6;j++){
                for(Piece* p :pieceAll){
                    if(p->name()==pieceName[j]){
                        pieceKind[j].push_back(p);
                    }
                }
            }
            for(int j=0;j<6;j++){
                if(pieceKind[j].isEmpty()){
                    continue;
                }
                out<<pieceName[j]<<" "<<pieceKind[j].size()<<" ";
                for(Piece* p:pieceKind[j]){
                    char px = 'a'+ p->x()-1;
                    out<<px<<p->y()<<" ";
                }
                out<<endl;
            }
        }
        file.close();
    }

}

void Manager::passive_newGame(int m){
    _meBlack = m;
    _currentTurn = 0;
    clearGrids();
    setGrids();
    clearPieces();
    Piece::setNewGamePieces(this,pieces);
    _w->ui->startButton->setEnabled(true);
    _w->ui->saveButton->setEnabled(true);
}

void Manager::passive_start(){
    _isOngame = 1;
    _meTime = 60;
    _w->ui->meLCD->display(_meTime);
    _youTime = 60;
    _w->ui->youLCD->display(_youTime);
    _w->ui->resignButton->setEnabled(true);
    if(_currentTurn == _meBlack){
        _w->ui->radioButton->setChecked(false);
        _w->ui->radioButton_2->setChecked(true);
    }else{
        _w->ui->radioButton->setChecked(true);
        _w->ui->radioButton_2->setChecked(false);
    }
    _timer.start(1000);
}

void Manager::passive_loadEndGame(QList<int> args){
    _w->ui->startButton->setEnabled(true);
    _w->ui->saveButton->setEnabled(true);
    _meBlack = args[0];
    _currentTurn = !args[0];
    clearGrids();
    setGrids();
    clearPieces();
    _isOngame = 0;

    for(int i=1;i<args.size();i+=4){
        int x = args[i],y = args[i+1], kind = args[i+2],color = args[i+3];
        Piece* p;
        switch(kind){
        case 0: p = new King(x,y,color,this);king[color] = p;break;
        case 1: p = new Queen(x,y,color,this);break;
        case 2: p = new Rook(x,y,color,this);break;
        case 3: p = new Bishop(x,y,color,this);break;
        case 4: p = new Knight(x,y,color,this);break;
        case 5: p = new Pawn(x,y,color,this);break;
        }
        pieces[color].push_back(p);
        _grids[x][y]->setPiece(p);
    }
}

void Manager::passive_resignOK(){
    _isOngame = 0;
    _timer.stop();
    QMessageBox::about(_w,"结束","你输了。");
}

void Manager::passive_resignCancel(){
    QMessageBox::about(_w,"认输","对方拒绝了你的认输请求。");
}

void Manager::passive_resignSelect(){
    auto reply = QMessageBox::question(_w,"认输","对手请求认输，是否同意？");
    if(reply == QMessageBox::Yes){
        _isOngame = 0;
        _timer.stop();
        writeMessage("resign OK");
        QMessageBox::about(_w,"结束","你赢了！");
    }else if(reply == QMessageBox::No){
        writeMessage("resign cancel");
    }

}

void Manager::passive_timeOut(){
    _isOngame = 0;
    QMessageBox::critical(_w,"超时","对手超时！你赢了！");
    _timer.stop();
}

void Manager::passive_move(QList<int> args){
    int x1 = args[0],y1=args[1],x2=args[2],y2=args[3];

    Piece* thePiece = _grids[x1][y1]->piece();

    if(_grids[x2][y2]->hasPiece()){
        _grids[x2][y2]->piece()->ate();
        removePiece(_grids[x2][y2]->piece());
    }
    thePiece->move(x2,y2);
    meTurn();
}

void Manager::passive_translocation(QList<int> arg){
    int x = arg[0],y=arg[1],x3=arg[2],k=arg[3];

    _grids[x][y]->piece()->move(x+2*k,y);
    _grids[x3][y]->piece()->move(x+k,y);
    meTurn();
}

void Manager::passive_checkMate(int color){
    _isOngame = 0;
    _timer.stop();
    if(color == _meBlack){
        QMessageBox::about(_w,"结束","你输了。");
    }else{
        QMessageBox::about(_w,"结束","你赢了!");
    }
}

void Manager::passive_pawnPromotion(QList<int> args){
    int x0 = args[0];
    int y0 = args[1];
    int x = args[2];
    int y = args[3];
    int targetID = args[4];


    Piece* thePiece = _grids[x0][y0]->piece();

    if(_grids[x][y]->hasPiece()){
        _grids[x][y]->piece()->ate();
        removePiece(_grids[x][y]->piece());
    }
    thePiece->move(x,y);

    int color = _grids[x][y]->piece()->color();
    _grids[x][y]->piece()->ate();
    removePiece(_grids[x][y]->piece());
    Piece* p;
    switch(targetID){
    case 1: p = new Queen(x,y,color,this);break;
    case 2: p = new Rook(x,y,color,this);break;
    case 3: p = new Bishop(x,y,color,this);break;
    case 4: p = new Knight(x,y,color,this);break;
    }
    pieces[color].push_back(p);
    _grids[x][y]->setPiece(p);
    meTurn();
}

void Manager::setClient(Client *c){
    _client = c;
    _networkMode = 1;
    connect(_client,&Client::getMessage,this,&Manager::readMessage);
    connect(_client,&Client::readyNewNetGame,this,&Manager::initNewBoard);
}

void Manager::setServer(Server *s){
    _server = s;
    _networkMode = 0;
    connect(_server,&Server::getMessage,this,&Manager::readMessage);
    connect(_server,&Server::readyNewNetGame,this,&Manager::initNewBoard);
}

void Manager::initNewBoard(){//bool clientBlack
    //_meBlack = (clientBlack == _networkMode);
    _w->show();
    _ew->close();
}

Grid* Manager::grid(int x,int y){
    return _grids[x][y];
}

bool Manager::meBlack(){
    return _meBlack;
}

bool Manager::meCheck(){
    return _meCheck;
}

Piece* Manager::meKing(){
    return king[_meBlack];
}
