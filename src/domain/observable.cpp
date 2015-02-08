#include "include/domain/observable.h"

void Observable::notify() {
    for (int i = 0; i < observers.count(); i++) {
        Observer *observer = observers.at(i);
        observer->update();
    }
}

void Observable::attach(Observer *observer) {
    observers.push_back(observer);
}

void Observable::detach(Observer *observer) {
    observers.removeOne(observer);
}
