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

#include <nanovg_dk.h>

#include <borealis/application.hpp>
#include <borealis/logger.hpp>
#include <borealis/platform_drivers/platform_driver_dk.hpp>

extern "C" u32 __nx_applet_exit_mode;

namespace brls::drv
{

namespace
{

    void OutputDkDebug(void* userData, const char* context, DkResult result, const char* message)
    {
        Logger::error("[DK:%d] %s: %s", result, context, message);

        if (result != DkResult_Success)
        {
            ErrorSystemConfig config;
            errorSystemCreate(&config, context, message);
            errorSystemSetResult(&config, result);
            errorSystemShow(&config);

            __nx_applet_exit_mode = 1;
            exit(EXIT_FAILURE);
        }
    }

}

bool PlatformDriverDK::initialize(const std::string& title, unsigned int windowWidth, unsigned int windowHeight)
{
    switch (appletGetOperationMode())
    {
        case AppletOperationMode_Docked:
            this->FramebufferWidth  = 1920;
            this->FramebufferHeight = 1080;
            break;
        case AppletOperationMode_Handheld:
            this->FramebufferWidth  = 1280;
            this->FramebufferHeight = 720;
            break;
    }

    appletHook(&this->cookie, &PlatformDriverDK::AppletCallback, this);

    // Create the deko3d device
    device = dk::DeviceMaker {}.setCbDebug(OutputDkDebug).create();

    // Create the main queue
    queue = dk::QueueMaker { device }.setFlags(DkQueueFlags_Graphics).create();

    // Create the memory pools
    pool_images.emplace(device, DkMemBlockFlags_GpuCached | DkMemBlockFlags_Image, 16 * 1024 * 1024);
    pool_code.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code, 128 * 1024);
    pool_data.emplace(device, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, 1 * 1024 * 1024);

    // Create the static command buffer and feed it freshly allocated memory
    cmdbuf                  = dk::CmdBufMaker { device }.create();
    CMemPool::Handle cmdmem = pool_data->allocate(StaticCmdSize);
    cmdbuf.addMemory(cmdmem.getMemBlock(), cmdmem.getOffset(), cmdmem.getSize());

    // Create the framebuffer resources
    createFramebufferResources();

    this->renderer.emplace(FramebufferWidth, FramebufferHeight, this->device, this->queue, *this->pool_images, *this->pool_code, *this->pool_data);
    this->vg = nvgCreateDk(&*this->renderer, NVG_ANTIALIAS | NVG_STENCIL_STROKES);

    Application::onWindowSizeChanged(FramebufferWidth, FramebufferHeight);

    return true;
}

bool PlatformDriverDK::exit()
{
    appletUnhook(&this->cookie);

    // Destroy the framebuffer resources. This should be done first.
    destroyFramebufferResources();

    // Cleanup vg. This needs to be done first as it relies on the renderer.
    nvgDeleteDk(vg);

    // Destroy the renderer
    this->renderer.reset();

    return true;
}

bool PlatformDriverDK::update()
{
    do
    {
        if (!appletMainLoop() || this->quitFlag)
            return false;
    } while (focused != AppletFocusState_Focused);

    this->gamepadDownOld = this->gamepadDown;

    // Gamepad
    this->gamepadDown = hidKeysHeld(CONTROLLER_P1_AUTO);

    return true;
}

void PlatformDriverDK::frame()
{
    // Acquire a framebuffer from the swapchain (and wait for it to be available)
    this->slot = queue.acquireImage(swapchain);

    // Run the command list that attaches said framebuffer to the queue
    queue.submitCommands(framebuffer_cmdlists[slot]);

    // Run the main rendering command list
    queue.submitCommands(render_cmdlist);
}

void PlatformDriverDK::swapBuffers()
{
    queue.presentImage(swapchain, slot);
}

void PlatformDriverDK::createFramebufferResources()
{
    // Create layout for the depth buffer
    dk::ImageLayout layout_depthbuffer;
    dk::ImageLayoutMaker { device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_S8)
        .setDimensions(FramebufferWidth, FramebufferHeight)
        .initialize(layout_depthbuffer);

    // Create the depth buffer
    depthBuffer_mem = pool_images->allocate(layout_depthbuffer.getSize(), layout_depthbuffer.getAlignment());
    depthBuffer.initialize(layout_depthbuffer, depthBuffer_mem.getMemBlock(), depthBuffer_mem.getOffset());

    // Create layout for the framebuffers
    dk::ImageLayout layout_framebuffer;
    dk::ImageLayoutMaker { device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_UsagePresent | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(FramebufferWidth, FramebufferHeight)
        .initialize(layout_framebuffer);

    // Create the framebuffers
    std::array<DkImage const*, NumFramebuffers> fb_array;
    uint64_t fb_size  = layout_framebuffer.getSize();
    uint32_t fb_align = layout_framebuffer.getAlignment();
    for (unsigned i = 0; i < NumFramebuffers; i++)
    {
        // Allocate a framebuffer
        framebuffers_mem[i] = pool_images->allocate(fb_size, fb_align);
        framebuffers[i].initialize(layout_framebuffer, framebuffers_mem[i].getMemBlock(), framebuffers_mem[i].getOffset());

        // Generate a command list that binds it
        dk::ImageView colorTarget { framebuffers[i] }, depthTarget { depthBuffer };
        cmdbuf.bindRenderTargets(&colorTarget, &depthTarget);
        framebuffer_cmdlists[i] = cmdbuf.finishList();

        // Fill in the array for use later by the swapchain creation code
        fb_array[i] = &framebuffers[i];
    }

    // Create the swapchain using the framebuffers
    swapchain = dk::SwapchainMaker { device, nwindowGetDefault(), fb_array }.create();
}

void PlatformDriverDK::destroyFramebufferResources()
{
    // Return early if we have nothing to destroy
    if (!swapchain)
        return;

    // Make sure the queue is idle before destroying anything
    queue.waitIdle();

    // Clear the static cmdbuf, destroying the static cmdlists in the process
    cmdbuf.clear();

    // Destroy the swapchain
    swapchain.destroy();

    // Destroy the framebuffers
    for (unsigned i = 0; i < NumFramebuffers; i++)
        framebuffers_mem[i].destroy();

    // Destroy the depth buffer
    depthBuffer_mem.destroy();
}

void PlatformDriverDK::AppletCallback(AppletHookType hook, void* param)
{
    PlatformDriverDK* ptr = static_cast<PlatformDriverDK*>(param);

    switch (hook)
    {
        case AppletHookType_OnFocusState:
            ptr->focused = appletGetFocusState();
            break;
        case AppletHookType_OnOperationMode:
        {
            // Destroy the framebuffer resources
            ptr->destroyFramebufferResources();

            // Choose framebuffer size
            switch (appletGetOperationMode())
            {
                case AppletOperationMode_Docked:
                    ptr->FramebufferWidth  = 1920;
                    ptr->FramebufferHeight = 1080;
                    break;
                case AppletOperationMode_Handheld:
                    ptr->FramebufferWidth  = 1280;
                    ptr->FramebufferHeight = 720;
                    break;
            }

            ptr->renderer->UpdateViewport(ptr->FramebufferWidth, ptr->FramebufferHeight);

            ptr->createFramebufferResources();

            Application::onWindowSizeChanged(ptr->FramebufferWidth, ptr->FramebufferHeight);
        }
        default:
            break;
    }
}

}