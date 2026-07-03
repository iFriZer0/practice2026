#ifndef CREATOR_MAKER_H__

#define CREATOR_MAKER_H__

#include <memory>
#include "creator.h"

template<typename Base, typename... Arguments>
class CreatorMaker
{
public:
    virtual ~CreatorMaker() = default;

    virtual std::unique_ptr<Creator<Base, Arguments...>> make() const = 0;
};

#endif
