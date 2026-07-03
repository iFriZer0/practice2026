#ifndef ERROR_H__

#define ERROR_H__

#include <any>
#include <cstddef>
#include <exception>
#include <typeinfo>

class Error : public std::exception
{
public:
    Error(const char *const message, const std::type_info &first_error, const std::any &data) noexcept;

    const char *what() const noexcept;

    std::any get_data() const noexcept;
private:
    static constexpr std::size_t MAXIMUM_SIZE{100};

    char message[MAXIMUM_SIZE + 1]{""};

    const std::type_info &first_error;

    std::any data;
};

#endif
