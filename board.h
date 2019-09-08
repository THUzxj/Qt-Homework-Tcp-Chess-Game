#ifndef BOARD_H
#define BOARD_H

#include <QObject>

class Grid;

class Board : public QObject
{
    Q_OBJECT
private:
    Grid* _grids[9][9];
    int _gridLenth;


public:
    explicit Board(QObject *parent = nullptr);

signals:

public slots:
};

#endif // BOARD_H
