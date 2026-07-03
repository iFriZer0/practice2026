#ifndef SOLUTION_H__

#define SOLUTION_H__

#include <functional>
#include <initializer_list>
#include <utility>

template<typename Base, typename Identifier, typename... Arguments>
class Solution final
{
public:
    explicit Solution() = default;
    explicit Solution(const Solution<Base, Arguments...> &other);
    explicit Solution(Solution<Base, Arguments...> &&other);
    // explicit Solution(std::initializer_list<std::pair<Identifier, std::function<>>>)

    ~Solution() = default;

    Solution<Base, Arguments...> &operator=(const Solution<Base, Arguments...> &other);
    Solution<Base, Arguments...> &operator=(Solution<Base, Arguments...> &&other);
};

#endif
