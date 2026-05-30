// observer.h - base class for the observer pattern
// any class that wants to listen to game state changes should inherit this
#ifndef OBSERVER_H
#define OBSERVER_H

class Observer
{
public:
    Observer();
    virtual ~Observer() = default;

    // subclasses override this to react when something changes
    virtual void notify() = 0;
};

#endif // OBSERVER_H
