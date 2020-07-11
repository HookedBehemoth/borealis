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

#include <switch.h>

#include <borealis/platform_drivers/platform_driver.hpp>
#include <deko3d.hpp>
#include <nanovg/dk_renderer.hpp>

namespace brls::drv
{

class PlatformDriverDK : public PlatformDriver
{
  public:
    virtual bool initialize(const std::string& title, unsigned int windowWidth, unsigned int windowHeight);
    virtual bool exit();

    virtual bool update();
    virtual void frame();
    virtual void swapBuffers();

  private:
    void createFramebufferResources();
    void destroyFramebufferResources();
    void updateResolution();
    static void AppletCallback(AppletHookType hook, void* param);

  private:
    static constexpr const unsigned NumFramebuffers = 2;
    static constexpr const unsigned StaticCmdSize   = 0x1000;
    static constexpr const unsigned DynCmdSize      = 0x1000;

    unsigned FramebufferWidth;
    unsigned FramebufferHeight;

    AppletHookCookie cookie;

    dk::UniqueDevice device;
    dk::UniqueQueue queue;

    std::optional<CMemPool> pool_images;
    std::optional<CMemPool> pool_code;
    std::optional<CMemPool> pool_data;

    dk::UniqueCmdBuf cmdbuf;

    CMemPool::Handle depthBuffer_mem;
    CMemPool::Handle framebuffers_mem[NumFramebuffers];

    dk::Image depthBuffer;
    dk::Image framebuffers[NumFramebuffers];
    DkCmdList framebuffer_cmdlists[NumFramebuffers];
    dk::UniqueSwapchain swapchain;

    DkCmdList render_cmdlist;

    std::optional<nvg::DkRenderer> renderer;

    int slot                 = -1;
    AppletFocusState focused = AppletFocusState_Focused;
};

} // namespace brls::drv
