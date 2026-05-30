// subject.h - the other half of the observer pattern
// both Board and Game inherit from this so they can notify the GUI
#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>

class Observer;

class Subject
{
public:
    Subject();
    virtual ~Subject() = default;

    void notifyObservers();    // calls notify() on all registered observers
    void registerObserver(Observer* observer);

private:
    std::vector<Observer*> observers;
};

#endif // SUBJECT_H
