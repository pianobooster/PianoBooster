/*********************************************************************************/
/*!
@file           IColorPreference.h

@brief          Contains color preferences configuration Information.

@author         L. J. Barman and others

    Copyright (c)   2008-2020, L. J. Barman and others, all rights reserved

    This file is part of the PianoBooster application

    PianoBooster is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PianoBooster is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PianoBooster.  If not, see <http://www.gnu.org/licenses/>.

*/
/*********************************************************************************/


#ifndef __ICOLORPREF_H__
#define __ICOLORPREF_H__


class CColor
{
public:
    CColor() { red = green = blue = 0; alpha=1.0;}

    CColor(double r, double g, double b, double a = 1.0)
    {
        red = static_cast<float>(r);
        green = static_cast<float>(g);
        blue = static_cast<float>(b);
        alpha = static_cast<float>(a);
    }

    float red, green, blue, alpha;

    bool operator==(CColor color)
    {
        if (red == color.red && green == color.green && blue == color.blue && alpha == color.alpha)
            return true;
        return false;
    }
};


class IColorPreference
{
public:

    virtual CColor staveColor() = 0;
    virtual CColor staveDimColor() = 0;
    virtual CColor noteColor() = 0;
    virtual CColor noteDimColor() = 0;
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
