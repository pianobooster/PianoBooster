

#ifndef __ICOLORPREF_H__
#define __ICOLORPREF_H__


class CColor
{
public:
    CColor() { red = green = blue = 0; }

    CColor(double r, double g, double b)
    {
        red = static_cast<float>(r);
        green = static_cast<float>(g);
        blue = static_cast<float>(b);
    }
    float red, green, blue;

    bool operator==(CColor color)
    {
        if (red == color.red && green == color.green && blue == color.blue)
            return true;
        return false;
    }
};


class IColorPreference
{
public:

    virtual CColor menuColor() = 0;
    virtual CColor menuSelectedColor() = 0;

    virtual CColor staveColor() = 0;
    virtual CColor staveColorDim() = 0;
    virtual CColor noteColor() = 0;
    virtual CColor noteColorDim() = 0;
    //static CColor playedGoodColor()    {return CColor(0.4, 0.4, 0.0);}
    virtual CColor playedGoodColor() = 0;
    virtual CColor playedBadColor()  = 0;
    virtual CColor playedStoppedColor() = 0;
    virtual CColor backgroundColor()  = 0;
    virtual CColor barMarkerColor() = 0;
    virtual CColor beatMarkerColor() = 0;
    virtual CColor pianoGoodColor() = 0;
    virtual CColor pianoBadColor() = 0;
    virtual CColor noteNameColor() = 0;

    virtual CColor playZoneAreaColor() = 0;
    virtual CColor playZoneEndColor() = 0;
    virtual CColor playZoneMiddleColor() = 0;

};

#endif  // __ICOLORPREF_H__