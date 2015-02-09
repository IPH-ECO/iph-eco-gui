#ifndef OBSERVER_H
#define OBSERVER_H

#include "observable.h"

class Observable;

class Observer {
protected:
    Observable *observable;

public:
    Observer(Observable *_observable);
    ~Observer();

    virtual void update() = 0;
};

#endif // OBSERVER_H
