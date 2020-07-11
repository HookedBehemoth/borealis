/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam

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

#include <borealis/platform_drivers/platform_driver.hpp>

namespace brls::drv
{

void PlatformDriver::quit()
{
    this->quitFlag = true;
}

bool PlatformDriver::isAnyKeyDown() const
{
    return this->gamepadDown > 0;
}

bool PlatformDriver::haveKeyStatesChanged() const
{
    return this->gamepadDown != this->gamepadDownOld;
}

unsigned long PlatformDriver::keysDown() const
{
    return (~this->gamepadDownOld) & this->gamepadDown;
}

unsigned long PlatformDriver::keysUp() const
{
    return this->gamepadDownOld & (~this->gamepadDown);
}

unsigned long PlatformDriver::keysHeld() const
{
    return this->gamepadDown;
}

std::pair<int, int> PlatformDriver::getTouchPosition() const
{
    return { 0, 0 };
}

int PlatformDriver::getTouchCount() const
{
    return 0;
}

} // namespace brls::drv