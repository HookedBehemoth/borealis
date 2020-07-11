/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  WerWolv

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>
#include <utility>

struct NVGcontext;

namespace brls
{

enum Key
{
    KEY_A      = (1 << 0),
    KEY_B      = (1 << 1),
    KEY_X      = (1 << 2),
    KEY_Y      = (1 << 3),
    KEY_LSTICK = (1 << 4),
    KEY_RSTICK = (1 << 5),
    KEY_L      = (1 << 6),
    KEY_R      = (1 << 7),
    KEY_PLUS   = (1 << 8),
    KEY_MINUS  = (1 << 9),
    KEY_DLEFT  = (1 << 10),
    KEY_DUP    = (1 << 11),
    KEY_DRIGHT = (1 << 12),
    KEY_DDOWN  = (1 << 13),
};

}

namespace brls::drv
{

class PlatformDriver
{
  public:
    virtual ~PlatformDriver() = default;

    virtual bool initialize(const std::string& title, unsigned int windowWidth, unsigned int windowHeight) = 0;
    virtual bool exit()                                                                                    = 0;

    void quit();

    virtual bool update()      = 0;
    virtual void frame()       = 0;
    virtual void swapBuffers() = 0;

    bool isAnyKeyDown() const;
    bool haveKeyStatesChanged() const;
    unsigned long keysDown() const;
    unsigned long keysUp() const;
    unsigned long keysHeld() const;

    std::pair<int, int> getTouchPosition() const;
    int getTouchCount() const;

    NVGcontext* getNVGContext()
    {
        return this->vg;
    }

  protected:
    NVGcontext* vg;
    bool quitFlag = false;

    unsigned long gamepadDown = 0, gamepadDownOld = 0;
};

} // namespace brls::drv
