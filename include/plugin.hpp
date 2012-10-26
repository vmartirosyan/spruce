#ifndef PLUGIN_HPP
#define PLUGIN_HPP


class Plugin
{     
public:
    virtual void run() = 0;
    virtual ~Plugin() {}
};

#endif
