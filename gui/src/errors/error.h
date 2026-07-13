#ifndef ERROR_H__

#define ERROR_H__

#include <cstddef>
#include <exception>
#include <typeinfo>

class Error : public std::exception
{
public:
    Error(const char *const message, const std::type_info &first_error) noexcept;

    const char *what() const noexcept override;

    const std::type_info &get_first_error() const noexcept;

    virtual const void *get_data() const noexcept = 0;
private:
    static constexpr std::size_t MAXIMUM_SIZE{100};

    char message[MAXIMUM_SIZE + 1]{""};

    const std::type_info &first_error;
};

#endif
