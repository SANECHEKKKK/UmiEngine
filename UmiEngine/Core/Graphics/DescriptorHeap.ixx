module;
#include <Graphics/d3dx12.h>
#include <vector>
#include <wrl/client.h>
export module DescriptorHeap;

using Microsoft::WRL::ComPtr;

export namespace Umi
{
    class DescriptorHeap 
    {
    private:
        ComPtr<ID3D12DescriptorHeap> heap;
        ID3D12Device* device = nullptr;
        D3D12_DESCRIPTOR_HEAP_TYPE type{};
        D3D12_DESCRIPTOR_HEAP_FLAGS flags{};
        UINT descriptorSize = 0;
        UINT nextFreeSlot = 0;
        UINT capacity = 0;


        HRESULT Allocate(UINT newCapacity) 
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc{};
            desc.NumDescriptors = newCapacity;
            desc.Type = type;
            desc.Flags = flags;
            auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
            capacity = newCapacity;
            return result;
        }

        HRESULT Reallocate(UINT newCapacity) 
        {
            ComPtr<ID3D12DescriptorHeap> oldHeap = heap;
            auto result = Allocate(newCapacity);
            if (FAILED(result))
                return result;

            device->CopyDescriptorsSimple(
                nextFreeSlot,
                heap->GetCPUDescriptorHandleForHeapStart(),
                oldHeap->GetCPUDescriptorHandleForHeapStart(),
                type
            );
            return S_OK;
        }

    public:
        HRESULT Init(ID3D12Device* device,
            D3D12_DESCRIPTOR_HEAP_TYPE type,
            D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
            UINT initialCapacity = 32) 
        {
            this->device = device;
            this->type = type;
            this->flags = flags;
            descriptorSize = device->GetDescriptorHandleIncrementSize(type);
            return Allocate(initialCapacity);
        }

        HRESULT Reserve(UINT capacity) 
        {
            if (capacity > this->capacity)
                return Reallocate(capacity);
            return S_OK;
        }

        UINT Add() 
        {
            if (nextFreeSlot >= capacity)
                Reallocate(capacity * 2);
            return nextFreeSlot++;
        }

        UINT Size()     const { return nextFreeSlot; }
        UINT Capacity() const { return capacity; }

        D3D12_CPU_DESCRIPTOR_HANDLE GetCPU(UINT slot) const 
        {
            auto handle = heap->GetCPUDescriptorHandleForHeapStart();
            handle.ptr += slot * descriptorSize;
            return handle;
        }

        D3D12_GPU_DESCRIPTOR_HANDLE GetGPU(UINT slot) const 
        {
            auto handle = heap->GetGPUDescriptorHandleForHeapStart();
            handle.ptr += slot * descriptorSize;
            return handle;
        }

        ID3D12DescriptorHeap* Get() const { return heap.Get(); }

        DescriptorHeap() = default;

        D3D12_CPU_DESCRIPTOR_HANDLE operator[] (UINT slot) const { return GetCPU(slot); }
    };
}