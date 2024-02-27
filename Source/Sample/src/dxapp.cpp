#include "dxapp.h"

DxApplication::DxApplication(UINT width, UINT height, std::wstring title)
    : m_width(width)
    , m_height(height)
    , m_title(title)
    , m_frame_index(0)
    , m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height))
    , m_scissor_rect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
    , m_rtv_descriptor_size(0)
{
    // nothing to do
}

void
DxApplication::on_init()
{
    this->load_pipeline();
    this->load_assets();
}

void
DxApplication::on_render()
{

}

void
DxApplication::on_close()
{

}

//

void
DxApplication::load_pipeline()
{

}

void
DxApplication::load_assets()
{

}

void
DxApplication::populate_command_list()
{

}

void
DxApplication::wait_for_previous_frame()
{

}
