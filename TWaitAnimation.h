#pragma once


class TWaitAnimation
{
public:
    virtual ~TWaitAnimation() {};
    
    virtual void Start() { return; }
    
    char * Name = "Unknown";
};
