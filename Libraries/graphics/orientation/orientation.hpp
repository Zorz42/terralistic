#pragma once

namespace gfx {

class Orientation {
public:
    float x, y;
};

inline const Orientation TOP_LEFT =     {0 , 0 };
inline const Orientation TOP =          {.5, 0 };
inline const Orientation TOP_RIGHT =    {1 , 0 };
inline const Orientation LEFT =         {0 , .5};
inline const Orientation CENTER =       {.5, .5};
inline const Orientation RIGHT =        {1 , .5};
inline const Orientation BOTTOM_LEFT =  {0 , 1 };
inline const Orientation BOTTOM =       {.5, 1 };
inline const Orientation BOTTOM_RIGHT = {1 , 1 };

};