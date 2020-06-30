#pragma once

#include "recycler_adapter.hpp"

class ExampleAdapter : public RecyclerAdapter
{
  public:
    ExampleAdapter();

    virtual size_t getItemCount() override final;
    virtual brls::View* createView() override final;
    virtual void bindViewHolder(brls::View* view, int index) override final;
};
