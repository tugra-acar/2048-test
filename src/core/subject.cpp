#include "core/subject.h"
#include "core/observer.h"

Subject::Subject()
{
}

// go through all observers and let them know something changed
void Subject::notifyObservers()
{
    for (Observer* o : observers)
        o->notify();
}

void Subject::registerObserver(Observer* observer)
{
    observers.push_back(observer);
}
