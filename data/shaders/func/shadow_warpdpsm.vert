
////////////////////////////////////////////////////////////////////////////////
//-- Warp dpsm - vertex shader                                                  

uniform mat4 coeffsX;
uniform mat4 coeffsY;

const float SCREEN_X = 128.0;
const float SCREEN_Y = 128.0;

const int INSIDE = 0;	//0x0000;
const int TOP = 1;		//0x0001;
const int BOTTOM = 2;	//0x0010;
const int RIGHT = 4;	//0x0100;
const int LEFT = 8;		//0x1000;

int computeCode(vec2 _p, vec4 range)
{
        int ret;
 
        ret = INSIDE;       
 
        if (_p.x < range.x)           // to the left of clip window
                ret |= LEFT;
        else if (_p.x > range.y)      // to the right of clip window
                ret |= RIGHT;
        if (_p.y < range.z)           // below the clip window
                ret |= BOTTOM;
        else if (_p.y > range.w)      // above the clip window
                ret |= TOP;
 
        return ret;
}

////////////////////////////////////////////////////////////////////////////////
