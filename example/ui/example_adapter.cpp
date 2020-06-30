#include "example_adapter.hpp"

#define FMT_HEADER_ONLY
#include <fmt/core.h>

#include "sane_image.hpp"

using namespace std::literals::string_literals;

namespace
{
/*
constexpr static const std::array ImageList = {
    "guydra_gore.png",
    "Help.png",
    "hignsight.png",
    "hoot.jpg",
    "huh.png",
    "Screenshot from 2019-09-03 14-50-31.png",
    "Screenshot from 2019-09-12 02-41-22.png",
    "Screenshot from 2019-09-28 16-17-13.png",
    "Screenshot from 2020-03-28 03-41-21.png",
    "Screenshot from 2020-03-30 21-53-44.png",
    "Screenshot from 2020-04-14 05-11-33.png",
    "Screenshot from 2020-04-14 06-03-32.png",
    "Screenshot from 2020-04-15 12-15-12.png",
    "Screenshot from 2020-04-22 21-32-32.png",
    "Screenshot from 2020-04-29 02-34-46.png",
    "Screenshot from 2020-04-29 02-46-50.png",
    "Screenshot from 2020-04-30 00-57-45.png",
    "Screenshot from 2020-05-02 15-09-46.png",
    "Screenshot from 2020-05-08 14-23-42.png",
    "Screenshot from 2020-05-08 14-27-47.png",
    "Screenshot from 2020-05-10 14-15-04.png",
    "Screenshot from 2020-05-10 20-12-13.png",
    "Screenshot from 2020-05-10 20-44-51.png",
    "Screenshot from 2020-05-10 20-51-33.png",
    "Screenshot from 2020-05-12 22-18-16.png",
    "Screenshot from 2020-05-13 20-12-02.png",
    "Screenshot from 2020-05-18 14-33-14.png",
    "Screenshot from 2020-05-19 21-21-32.png",
    "Screenshot from 2020-05-21 22-03-28.png",
    "Screenshot from 2020-05-22 23-52-50.png",
    "Screenshot from 2020-05-25 00-44-52.png",
    "Screenshot from 2020-06-18 22-40-27.png",
    "Screenshot from 2020-06-19 01-25-27.png",
    "leolaw.png",
    "Ninty.png",
    "nowwhat.png",
    "nwhat.png",
    "owo.jpg",
    "patreon.png",
};
*/
std::vector<std::string> ImageList;

}
#include <filesystem>

ExampleAdapter::ExampleAdapter() {
    for (auto &entry : std::filesystem::directory_iterator("/home/behemoth/Pictures/"))
        if (entry.path().extension() == ".png")
            ImageList.push_back(entry.path().filename());
}

size_t ExampleAdapter::getItemCount()
{
    return ImageList.size();
}

brls::View* ExampleAdapter::createView()
{
    return new brls::SaneImage();
}

void ExampleAdapter::bindView(brls::View* view, int index)
{
    auto img  = static_cast<brls::SaneImage*>(view);
    auto path = fmt::format("/home/behemoth/Pictures/{}", ImageList[index]);
    img->setImage(path);
}
