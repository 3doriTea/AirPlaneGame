#pragma once
class RenderResource
{
public:
	virtual void Initialize();
	virtual void SetResource();
};

class DXGIResource : public RenderResource
{
	void Initialize() override;
	void SetResource() override;
};

class Direct3DResource : public RenderResource
{
	void Initialize() override;
	void SetResource() override;
};

class Direct2DResource : public RenderResource
{
	void Initialize() override;
	void SetResource() override;
};
