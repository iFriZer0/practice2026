#ifndef VIEW_H__

#define VIEW_H__

class View
{
public:
    virtual ~View() = default;

    virtual void show() = 0;
};

#endif
