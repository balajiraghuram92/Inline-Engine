#include "ResourceView.hpp"

#include "MemoryObject.hpp"
#include "HostDescHeap.hpp"


namespace inl {
namespace gxeng {


VertexBufferView::VertexBufferView(const VertexBuffer& resource, uint32_t stride, uint32_t size) :
	m_resource(resource),
	m_stride(stride),
	m_size(size)
{}


const VertexBuffer& VertexBufferView::GetResource() {
	return m_resource;
}


ConstBufferView::ConstBufferView(const VolatileConstBuffer& resource, PersistentResViewHeap& heap) :
	ResourceViewBase(resource, &heap)
{
	gxapi::ConstantBufferViewDesc desc;
	desc.gpuVirtualAddress = resource.GetVirtualAddress();
	desc.sizeInBytes = resource.GetSize();

	m_descRef.reset(new DescriptorReference(heap.CreateCBV(desc)));
}


ConstBufferView::ConstBufferView(const PersistentConstBuffer& resource, PersistentResViewHeap& heap) :
	ResourceViewBase(resource)
{
	gxapi::ConstantBufferViewDesc desc;
	desc.gpuVirtualAddress = resource.GetVirtualAddress();
	desc.sizeInBytes = resource.GetSize();

	m_descRef.reset(new DescriptorReference(heap.CreateCBV(desc)));
}


RenderTargetView::RenderTargetView(
	const Texture2D& resource,
	RTVHeap& heap,
	gxapi::RtvTexture2DArray desc
):
	ResourceViewBase(resource)
{
	gxapi::RenderTargetViewDesc RTVdesc;
	RTVdesc.format = resource.GetFormat();
	RTVdesc.dimension = gxapi::eRtvDimension::TEXTURE2DARRAY;
	RTVdesc.tex2DArray = desc;

	m_desc = RTVdesc;

	m_descRef.reset(new DescriptorReference(heap.Create(m_resource, RTVdesc)));
}


RenderTargetView::RenderTargetView(
	const Texture2D& resource,
	DescriptorReference&& handle,
	gxapi::RenderTargetViewDesc desc
):
	ResourceViewBase(resource),
	m_desc(desc)
{
	m_descRef.reset(new DescriptorReference(std::move(handle)));
}

/*
RenderTargetView::RenderTargetView(
	RenderTargetView&& other,
	const Texture2D& resource
):
	ResourceViewBase(resource),
	m_desc(std::move(other.m_desc))
{
	m_descRef = std::move(other.m_descRef);

	other.m_descRef.reset();
}


RenderTargetView::RenderTargetView(
	const RenderTargetView& other,
	const Texture2D& resource
):
	ResourceViewBase(resource, other.m_descRef),
	m_desc(other.m_desc)
{}
*/


gxapi::RenderTargetViewDesc RenderTargetView::GetDescription() const {
	return m_desc;
}


DepthStencilView::DepthStencilView(
	const Texture2D& resource,
	DSVHeap& heap,
	gxapi::eFormat format,
	gxapi::DsvTexture2DArray desc
):
	ResourceViewBase(resource)
{
	gxapi::DepthStencilViewDesc DSVdesc;
	DSVdesc.format = format;
	DSVdesc.dimension = gxapi::eDsvDimension::TEXTURE2DARRAY;
	DSVdesc.tex2DArray = desc;

	m_desc = DSVdesc;

	m_descRef.reset(new DescriptorReference(heap.Create(m_resource, DSVdesc)));
}



gxapi::DepthStencilViewDesc DepthStencilView::GetDescription() const {
	return m_desc;
}


BufferSRV::BufferSRV(
	const LinearBuffer& resource,
	PersistentResViewHeap& heap,
	gxapi::eFormat format,
	gxapi::SrvBuffer desc
):
	ResourceViewBase(resource),
	m_format(format),
	m_srvDesc(desc)
{
	gxapi::ShaderResourceViewDesc fullSrvDesc;
	fullSrvDesc.format = format;
	fullSrvDesc.dimension = gxapi::eSrvDimension::BUFFER;
	fullSrvDesc.buffer = desc;

	m_descRef.reset(new DescriptorReference(heap.CreateSRV(m_resource, fullSrvDesc)));
}


gxapi::eFormat BufferSRV::GetFormat() {
	return m_format;
}


const gxapi::SrvBuffer& BufferSRV::GetDescription() const {
	return m_srvDesc;
}


Texture1DSRV::Texture1DSRV(
	const Texture1D& resource,
	PersistentResViewHeap& heap,
	gxapi::eFormat format,
	gxapi::SrvTexture1DArray srvDesc
):
	ResourceViewBase(resource),
	m_format(format),
	m_srvDesc(srvDesc)
{
	gxapi::ShaderResourceViewDesc fullSrvDesc;
	fullSrvDesc.format = format;
	fullSrvDesc.dimension = gxapi::eSrvDimension::TEXTURE1DARRAY;
	fullSrvDesc.tex1DArray = srvDesc;

	m_descRef.reset(new DescriptorReference(heap.CreateSRV(m_resource, fullSrvDesc)));
}


gxapi::eFormat Texture1DSRV::GetFormat() {
	return m_format;
}


const gxapi::SrvTexture1DArray& Texture1DSRV::GetDescription() const {
	return m_srvDesc;
}


Texture2DSRV::Texture2DSRV(
	const Texture2D& resource,
	PersistentResViewHeap & heap,
	gxapi::eFormat format,
	gxapi::SrvTexture2DArray srvDesc
):
	ResourceViewBase(resource),
	m_format(format),
	m_srvDesc(srvDesc)
{
	gxapi::ShaderResourceViewDesc fullSrvDesc;
	fullSrvDesc.format = format;
	fullSrvDesc.dimension = gxapi::eSrvDimension::TEXTURE2DARRAY;
	fullSrvDesc.tex2DArray = srvDesc;

	m_descRef.reset(new DescriptorReference(heap.CreateSRV(m_resource, fullSrvDesc)));
}


gxapi::eFormat Texture2DSRV::GetFormat() {
	return m_format;
}


const gxapi::SrvTexture2DArray& Texture2DSRV::GetDescription() const {
	return m_srvDesc;
}


Texture3DSRV::Texture3DSRV(
	const Texture3D& resource,
	PersistentResViewHeap& heap,
	gxapi::eFormat format,
	gxapi::SrvTexture3D srvDesc
):
	ResourceViewBase(resource),
	m_format(format),
	m_srvDesc(srvDesc)
{
	gxapi::ShaderResourceViewDesc fullSrvDesc;
	fullSrvDesc.format = format;
	fullSrvDesc.dimension = gxapi::eSrvDimension::TEXTURE3D;
	fullSrvDesc.tex3D = srvDesc;

	m_descRef.reset(new DescriptorReference(heap.CreateSRV(m_resource, fullSrvDesc)));
}


gxapi::eFormat Texture3DSRV::GetFormat() {
	return m_format;
}


const gxapi::SrvTexture3D& Texture3DSRV::GetDescription() const {
	return m_srvDesc;
}


TextureCubeSRV::TextureCubeSRV(
	const TextureCube& resource,
	PersistentResViewHeap& heap,
	gxapi::eFormat format,
	gxapi::SrvTextureCube srvDesc
) :
	ResourceViewBase(resource),
	m_format(format),
	m_srvDesc(srvDesc)
	{
	gxapi::ShaderResourceViewDesc fullSrvDesc;
	fullSrvDesc.format = format;
	fullSrvDesc.dimension = gxapi::eSrvDimension::TEXTURECUBE;
	fullSrvDesc.texCube = srvDesc;

	m_descRef.reset(new DescriptorReference(heap.CreateSRV(m_resource, fullSrvDesc)));
}


gxapi::eFormat TextureCubeSRV::GetFormat() {
	return m_format;
}


const gxapi::SrvTextureCube & TextureCubeSRV::GetDescription() const {
	return m_srvDesc;
}


} // namespace gxeng
} // namespace inl

