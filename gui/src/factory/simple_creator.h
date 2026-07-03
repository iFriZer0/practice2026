#ifndef SIMPLE_CREATOR_H__

#define SIMPLE_CREATOR_H__

#include "creator.h"

template<typename Base, typename Derived, typename... Arguments>
class SimpleCreator : public Creator<Base, Arguments...>
{
public:
    std::unique_ptr<Base> create(Arguments&& ...arguments) const override;
};

#include "simple_creator.hpp"

#endif
