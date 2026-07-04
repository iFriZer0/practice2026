#ifndef CREATOR_H__

#define CREATOR_H__

#include <memory>

template<typename Base, typename... Arguments>
class Creator
{
public:
    virtual ~Creator() = default;

    virtual std::unique_ptr<Base> create(Arguments&& ...arguments) const = 0;
};

#endif
