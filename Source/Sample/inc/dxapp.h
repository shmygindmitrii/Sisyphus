#include "base_vectors.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include "win/d3dx12.h"
#include "wrl.h"
#include <vector>

namespace mwrl = Microsoft::WRL;
namespace sybase = Sisyphus::Base;

class DxApplication {
public:
  DxApplication(UINT width, UINT height, std::wstring title);
  void
  on_init();
  void
  on_render();
  void
  on_close();

private:
  static const UINT FRAME_COUNT = 2;

  struct Vertex {
    sybase::vec3_t position;
    sybase::vec4_t color;
  };
  UINT         m_width;
  UINT         m_height;
  std::wstring m_title;
  // Pipeline objects.
  CD3DX12_VIEWPORT                        m_viewport;
  CD3DX12_RECT                            m_scissor_rect;
  mwrl::ComPtr<IDXGISwapChain3>           m_swap_chain;
  mwrl::ComPtr<ID3D12Device>              m_device;
  mwrl::ComPtr<ID3D12Resource>            m_render_targets[FRAME_COUNT];
  mwrl::ComPtr<ID3D12CommandAllocator>    m_command_allocator;
  mwrl::ComPtr<ID3D12CommandQueue>        m_command_queue;
  mwrl::ComPtr<ID3D12RootSignature>       m_root_signature;
  mwrl::ComPtr<ID3D12DescriptorHeap>      m_rtv_heap;
  mwrl::ComPtr<ID3D12PipelineState>       m_pipeline_state;
  mwrl::ComPtr<ID3D12GraphicsCommandList> m_command_list;
  UINT                                    m_rtv_descriptor_size;

  // App resources.
  mwrl::ComPtr<ID3D12Resource> m_vertex_buffer;
  D3D12_VERTEX_BUFFER_VIEW     m_vertex_buffer_view;

  // Synchronization objects.
  UINT                      m_frame_index;
  HANDLE                    m_fence_event;
  mwrl::ComPtr<ID3D12Fence> m_fence;
  UINT64                    m_fence_value;

  void
  load_pipeline();
  void
  load_assets();
  void
  populate_command_list();
  void
  wait_for_previous_frame();
};
