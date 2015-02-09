#ifndef OBSERVABLE_H
#define OBSERVABLE_H

#include <QList>

#include "observer.h"

class Observer;

class Observable {
private:
    QList<Observer*> observers;

public:
    void notify();
    void attach(Observer *observer);
    void detach(Observer *observer);
};

#endif // OBSERVABLE_H
