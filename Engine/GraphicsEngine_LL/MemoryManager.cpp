
#include "MemoryManager.hpp"
#include "../GraphicsApi_LL/Exception.hpp"
#include "../GraphicsApi_LL/IGraphicsApi.hpp"
#include "../GraphicsApi_LL/Common.hpp"

#include "MemoryObject.hpp"

#include <algorithm>
#include <cassert>


namespace inl {
namespace gxeng {


MemoryManager::MemoryManager(gxapi::IGraphicsApi* graphicsApi) :
	m_graphicsApi(graphicsApi),
	m_criticalHeap(graphicsApi),
	m_uploadHeap(graphicsApi),
	m_constBufferHeap(graphicsApi)
{}


void MemoryManager::LockResident(const std::vector<MemoryObject*>& resources) {
	LockResident(resources.begin(), resources.end());
}


void MemoryManager::UnlockResident(const std::vector<MemoryObject*>& resources) {
	UnlockResident(resources.begin(), resources.end());
}


UploadManager& MemoryManager::GetUploadHeap() {
	return m_uploadHeap;
}


VolatileConstBuffer MemoryManager::CreateVolatileConstBuffer(void* data, uint32_t size) {
	return m_constBufferHeap.CreateVolatileBuffer(data, size);
}


PersistentConstBuffer MemoryManager::CreatePersistentConstBuffer(void * data, uint32_t size) {
	return m_constBufferHeap.CreatePersistentBuffer(data, size);
}


VertexBuffer MemoryManager::CreateVertexBuffer(eResourceHeapType heap, size_t size) {
	MemoryObjectDescriptor desc = AllocateResource(heap, gxapi::ResourceDesc::Buffer(size));

	VertexBuffer result(desc);
	return result;
}


IndexBuffer MemoryManager::CreateIndexBuffer(eResourceHeapType heap, size_t size, size_t indexCount) {
	MemoryObjectDescriptor desc = AllocateResource(heap, gxapi::ResourceDesc::Buffer(size));

	IndexBuffer result(desc, indexCount);
	return result;
}


Texture1D MemoryManager::CreateTexture1D(eResourceHeapType heap, uint64_t width, gxapi::eFormat format, gxapi::eResourceFlags flags, uint16_t arraySize) {
	if (arraySize < 1) {
		throw gxapi::InvalidArgument("\"count\" should not be at least one.");
	}

	MemoryObjectDescriptor desc = AllocateResource(heap, gxapi::ResourceDesc::Texture1DArray(width, format, arraySize, flags));

	Texture1D result(desc);
	return result;
}


Texture2D MemoryManager::CreateTexture2D(eResourceHeapType heap, uint64_t width, uint32_t height, gxapi::eFormat format, gxapi::eResourceFlags flags, uint16_t arraySize) {
	if (arraySize < 1) {
		throw gxapi::InvalidArgument("\"count\" should not be at least one.");
	}

	MemoryObjectDescriptor desc = AllocateResource(heap, gxapi::ResourceDesc::Texture2DArray(width, height, format, arraySize, flags));

	Texture2D result(desc);
	return result;
}


Texture3D MemoryManager::CreateTexture3D(eResourceHeapType heap, uint64_t width, uint32_t height, uint16_t depth, gxapi::eFormat format, gxapi::eResourceFlags flags) {
	MemoryObjectDescriptor desc = AllocateResource(heap, gxapi::ResourceDesc::Texture3D(width, height, depth, format));

	Texture3D result(desc);
	return result;
}


TextureCube MemoryManager::CreateTextureCube(eResourceHeapType heap, uint64_t width, uint32_t height, gxapi::eFormat format, gxapi::eResourceFlags flags) {
	MemoryObjectDescriptor desc = AllocateResource(heap, gxapi::ResourceDesc::CubeMap(width, height, format));

	TextureCube result(desc);
	return result;
}


MemoryObjectDescriptor MemoryManager::AllocateResource(eResourceHeapType heap, const gxapi::ResourceDesc& desc) {

	gxapi::ClearValue* pClearValue = nullptr;
	gxapi::ClearValue clearValue(desc.textureDesc.format, 1, 0);
	if (desc.type == gxapi::eResourceType::TEXTURE && desc.textureDesc.flags & gxapi::eResourceFlags::ALLOW_DEPTH_STENCIL) {
		pClearValue = &clearValue;
	}

	switch(heap) {
	case eResourceHeapType::CRITICAL: 
		return m_criticalHeap.Allocate(desc, pClearValue);
		break;
	default:
		assert(false);
	}

	return MemoryObjectDescriptor();
}


} // namespace gxeng
} // namespace inl
