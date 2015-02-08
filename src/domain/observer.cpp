#include "include/domain/observer.h"

Observer::Observer(Observable *_observable) : observable(_observable) {
    this->observable->attach(this);
}

Observer::~Observer() {
    this->observable->detach(this);
}
