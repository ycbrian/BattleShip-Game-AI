#pragma once

using GETAIFUNC   =  void* (*)(void);

struct Content
{
    GETAIFUNC getai;
    void *handle; // place for dlopen handle
};
