/*
* このライブラリはとある先生がChatGPTを使って生成したものです。
* 取り扱いに注意してください。
*/

#pragma once
#define NOMINMAX

#include <DirectXMath.h>
#include <cmath>
#include <algorithm>
#include <vector>
#include <list>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) do{ if(p){ (p)->Release(); (p)=nullptr; } }while(0)
#endif

namespace yz
{
    using namespace DirectX;

    struct Vtx { XMFLOAT3 pos; XMFLOAT4 col; };

    // --------------------------------------------------
    // Math helpers
    // --------------------------------------------------
    struct F3 { float x, y, z; };
    static inline F3  f3(float x, float y, float z) { return { x,y,z }; }
    static inline F3  add(F3 a, F3 b) { return { a.x + b.x,a.y + b.y,a.z + b.z }; }
    static inline F3  sub(F3 a, F3 b) { return { a.x - b.x,a.y - b.y,a.z - b.z }; }
    static inline F3  mul(F3 a, float s) { return { a.x * s,a.y * s,a.z * s }; }
    static inline float dot(F3 a, F3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    static inline F3  cross(F3 a, F3 b) {
        return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
    }
    static inline float len(F3 a) { return std::sqrt(dot(a, a)); }
    static inline F3  norm(F3 a) { float L = len(a); return (L > 1e-6f) ? mul(a, 1.0f / L) : f3(0, 0, 0); }
    static inline F3  lerp(F3 a, F3 b, float t) { return add(a, mul(sub(b, a), t)); }
    static inline float clampf(float v, float lo, float hi) { return max(lo, min(hi, v)); }

    // --------------------------------------------------
    // TrailEmitter: stores world-space points; builds billboard ribbon
    // --------------------------------------------------
    class TrailEmitter {
    public:
        TrailEmitter(F3 startPos, XMFLOAT4 baseColor, float width = 0.25f)
            : pos(startPos), color(baseColor), widthBase(width) {
        }

        // Update emitter motion and append trail points at fixed interval
        void Update(float dt) {
            time += dt;
            timer += dt;

            // Motion: Lissajous-like + slow spin around Y
            float t = time * motionSpeed;
            float rx = radius * std::sin(t * 1.2f + phase);
            float ry = 0.8f * std::sin(t * 1.7f + phase * 0.7f);
            float rz = radius * std::cos(t * 0.9f + phase * 1.3f);
            pos = add(center, f3(rx, baseHeight + ry, 3.0f + rz));

            if (timer >= emitInterval) {
                timer = 0.0f;
                points.push_back(pos);
                if (points.size() > maxPoints) points.pop_front();
            }
        }

        // Expand to triangle strip vertices (camera-facing billboard)
        // viewDir: from camera to target (roughly forward vector)
        void BuildRibbon(const F3& viewDir, std::vector<Vtx>& out) const {
            out.clear();
            if (points.size() < 2) return;

            // precompute camera right-ish vector for fallback
            F3 vd = norm(viewDir);
            F3 up = f3(0, 1, 0);

            // Iterate segments, generating two verts per point (left/right)
            const size_t n = points.size();
            std::vector<F3> pts; pts.reserve(n);
            for (auto& p : points) pts.push_back(p);

            // Width taper & alpha fade to tail
            for (size_t i = 0; i < n; ++i) {
                size_t i0 = (i == 0) ? 0 : (i - 1);
                size_t i1 = (i + 1 < n) ? (i + 1) : i;
                F3 p = pts[i];
                F3 p0 = pts[i0];
                F3 p1 = pts[i1];

                F3 tangent = norm(sub(p1, p0));
                // normal in camera-facing plane
                F3 nrm = cross(vd, tangent);
                if (len(nrm) < 1e-6f) nrm = cross(up, tangent);
                nrm = norm(nrm);

                float t = (n > 1) ? float(i) / float(n - 1) : 0.0f;
                float w = widthBase * (1.0f - 0.6f * t);         // taper
                float a = color.w * (1.0f - t);                 // fade alpha

                F3 left = add(p, mul(nrm, -w));
                F3 right = add(p, mul(nrm, w));

                Vtx vl; vl.pos = XMFLOAT3(left.x, left.y, left.z);
                vl.col = XMFLOAT4(color.x, color.y, color.z, a);
                Vtx vr; vr.pos = XMFLOAT3(right.x, right.y, right.z);
                vr.col = XMFLOAT4(color.x, color.y, color.z, a);

                // Build strip order: alternating left/right per point
                // (triangle strip expects LR LR LR ...)
                out.push_back(vl);
                out.push_back(vr);
            }
        }

        void SetCenter(F3 c) { center = c; }
        void SetPhase(float ph) { phase = ph; }
        void SetRadius(float r) { radius = r; }
        void SetEmitInterval(float dt) { emitInterval = dt; }
        void SetMaxPoints(size_t m) { maxPoints = m; }
        void SetBaseHeight(float h) { baseHeight = h; }
        void SetMotionSpeed(float s) { motionSpeed = s; }
        void SetWidth(float w) { widthBase = w; }
        void SetColor(const XMFLOAT4& c) { color = c; }

        float TailLengthGuess() const { return float(points.size()) * emitInterval * motionSpeed * radius; }

    private:
        std::list<F3> points;
        F3  pos;
        F3  center = f3(0, 0, 0);
        float baseHeight = 0.5f;
        float radius = 2.2f;
        float motionSpeed = 1.0f;
        float emitInterval = 0.02f;
        size_t maxPoints = 200;
        float widthBase = 0.25f;
        float time = 0.0f;
        float timer = 0.0f;
        float phase = 0.0f;
        XMFLOAT4 color{ 0.3f,0.8f,1.0f,0.85f };
    };
}

#if 0

// DX11_TrailEmitterDemo_Full.cpp
// Single-file DirectX 11 demo: multiple fluttering trail "ribbons" (triangle strips)
// following moving emitters. No external libs besides DX11 + DirectXMath + stdlib.
//
// Build (Developer Command Prompt for VS):
//   cl /std:c++17 /EHsc /O2 DX11_TrailEmitterDemo_Full.cpp /I. /link d3d11.lib dxgi.lib d3dcompiler.lib user32.lib gdi32.lib
//
// Controls:
//   ESC : Exit
//   1/2 : Trail width - / +
//   3/4 : Tail length - / +
//   B   : Toggle debug bounding box
//
// Notes:
// - Fixed camera; emitters move; trails follow their world-space positions.
// - Triangle-strip billboard ribbons with per-vertex fading alpha.

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <list>
#include <random>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <cstring>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) do{ if(p){ (p)->Release(); (p)=nullptr; } }while(0)
#endif

// --------------------------------------------------
// Simple DX11 device/swapchain state
// --------------------------------------------------
struct DX11 {
    ID3D11Device* dev = nullptr;
    ID3D11DeviceContext* ctx = nullptr;
    IDXGISwapChain* sc = nullptr;
    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11Texture2D* dsvTx = nullptr;
    ID3D11DepthStencilView* dsv = nullptr;
    UINT width = 1280;
    UINT height = 720;
} gdx;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// --------------------------------------------------
// HLSL (very small)
// --------------------------------------------------
static const char* gHLSL = R"(
cbuffer CB : register(b0) { float4x4 gVP; }
struct VSIn  { float3 pos : POSITION; float4 col : COLOR; };
struct VSOut { float4 sv  : SV_POSITION; float4 col : COLOR; };
VSOut VSMain(VSIn i){ VSOut o; o.sv = mul(float4(i.pos,1), gVP); o.col = i.col; return o; }
float4 PSMain(VSOut i) : SV_Target { return i.col; }
)";

struct Vtx { XMFLOAT3 pos; XMFLOAT4 col; };

ID3D11VertexShader* gVS = nullptr;
ID3D11PixelShader* gPS = nullptr;
ID3D11InputLayout* gIL = nullptr;
ID3D11Buffer* gVB = nullptr; // dynamic big buffer reused
ID3D11Buffer* gCB = nullptr;
ID3D11BlendState* gBS = nullptr;
ID3D11DepthStencilState* gDSS = nullptr;
ID3D11RasterizerState* gRS = nullptr;

// --------------------------------------------------
// Math helpers
// --------------------------------------------------
struct F3 { float x, y, z; };
static inline F3  f3(float x, float y, float z) { return { x,y,z }; }
static inline F3  add(F3 a, F3 b) { return { a.x + b.x,a.y + b.y,a.z + b.z }; }
static inline F3  sub(F3 a, F3 b) { return { a.x - b.x,a.y - b.y,a.z - b.z }; }
static inline F3  mul(F3 a, float s) { return { a.x * s,a.y * s,a.z * s }; }
static inline float dot(F3 a, F3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline F3  cross(F3 a, F3 b) {
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}
static inline float len(F3 a) { return std::sqrt(dot(a, a)); }
static inline F3  norm(F3 a) { float L = len(a); return (L > 1e-6f) ? mul(a, 1.0f / L) : f3(0, 0, 0); }
static inline F3  lerp(F3 a, F3 b, float t) { return add(a, mul(sub(b, a), t)); }
static inline float clampf(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }

// --------------------------------------------------
// TrailEmitter: stores world-space points; builds billboard ribbon
// --------------------------------------------------
class TrailEmitter {
public:
    TrailEmitter(F3 startPos, XMFLOAT4 baseColor, float width = 0.25f)
        : pos(startPos), color(baseColor), widthBase(width) {
    }

    // Update emitter motion and append trail points at fixed interval
    void Update(float dt) {
        time += dt;
        timer += dt;

        // Motion: Lissajous-like + slow spin around Y
        float t = time * motionSpeed;
        float rx = radius * std::sin(t * 1.2f + phase);
        float ry = 0.8f * std::sin(t * 1.7f + phase * 0.7f);
        float rz = radius * std::cos(t * 0.9f + phase * 1.3f);
        pos = add(center, f3(rx, baseHeight + ry, 3.0f + rz));

        if (timer >= emitInterval) {
            timer = 0.0f;
            points.push_back(pos);
            if (points.size() > maxPoints) points.pop_front();
        }
    }

    // Expand to triangle strip vertices (camera-facing billboard)
    // viewDir: from camera to target (roughly forward vector)
    void BuildRibbon(const F3& viewDir, std::vector<Vtx>& out) const {
        out.clear();
        if (points.size() < 2) return;

        // precompute camera right-ish vector for fallback
        F3 vd = norm(viewDir);
        F3 up = f3(0, 1, 0);

        // Iterate segments, generating two verts per point (left/right)
        const size_t n = points.size();
        std::vector<F3> pts; pts.reserve(n);
        for (auto& p : points) pts.push_back(p);

        // Width taper & alpha fade to tail
        for (size_t i = 0; i < n; ++i) {
            size_t i0 = (i == 0) ? 0 : (i - 1);
            size_t i1 = (i + 1 < n) ? (i + 1) : i;
            F3 p = pts[i];
            F3 p0 = pts[i0];
            F3 p1 = pts[i1];

            F3 tangent = norm(sub(p1, p0));
            // normal in camera-facing plane
            F3 nrm = cross(vd, tangent);
            if (len(nrm) < 1e-6f) nrm = cross(up, tangent);
            nrm = norm(nrm);

            float t = (n > 1) ? float(i) / float(n - 1) : 0.0f;
            float w = widthBase * (1.0f - 0.6f * t);         // taper
            float a = color.w * (1.0f - t);                 // fade alpha

            F3 left = add(p, mul(nrm, -w));
            F3 right = add(p, mul(nrm, w));

            Vtx vl; vl.pos = XMFLOAT3(left.x, left.y, left.z);
            vl.col = XMFLOAT4(color.x, color.y, color.z, a);
            Vtx vr; vr.pos = XMFLOAT3(right.x, right.y, right.z);
            vr.col = XMFLOAT4(color.x, color.y, color.z, a);

            // Build strip order: alternating left/right per point
            // (triangle strip expects LR LR LR ...)
            out.push_back(vl);
            out.push_back(vr);
        }
    }

    void SetCenter(F3 c) { center = c; }
    void SetPhase(float ph) { phase = ph; }
    void SetRadius(float r) { radius = r; }
    void SetEmitInterval(float dt) { emitInterval = dt; }
    void SetMaxPoints(size_t m) { maxPoints = m; }
    void SetBaseHeight(float h) { baseHeight = h; }
    void SetMotionSpeed(float s) { motionSpeed = s; }
    void SetWidth(float w) { widthBase = w; }
    void SetColor(const XMFLOAT4& c) { color = c; }

    float TailLengthGuess() const { return float(points.size()) * emitInterval * motionSpeed * radius; }

private:
    std::list<F3> points;
    F3  pos;
    F3  center = f3(0, 0, 0);
    float baseHeight = 0.5f;
    float radius = 2.2f;
    float motionSpeed = 1.0f;
    float emitInterval = 0.02f;
    size_t maxPoints = 200;
    float widthBase = 0.25f;
    float time = 0.0f;
    float timer = 0.0f;
    float phase = 0.0f;
    XMFLOAT4 color{ 0.3f,0.8f,1.0f,0.85f };
};

// --------------------------------------------------
// App state
// --------------------------------------------------
struct Bounds {
    float xMin = -4.0f, xMax = 4.0f;
    float yMin = -2.5f, yMax = 2.5f;
    float zMin = 0.8f, zMax = 7.0f;
};

struct AppState {
    // camera (fixed)
    XMFLOAT3 camPos{ 0.0f, 1.2f, -7.5f };
    XMFLOAT3 camTarget{ 0.0f, 0.6f,  2.5f };
    XMFLOAT3 camUp{ 0.0f, 1.0f,  0.0f };

    // trails
    std::vector<TrailEmitter> trails;
    float trailWidth = 0.30f;
    size_t trailMaxPoints = 220;
    bool showDebugBox = false;

    Bounds box;
} gapp;

// --------------------------------------------------
// DX11 helpers
// --------------------------------------------------
HRESULT Compile(const char* src, const char* entry, const char* profile, ID3DBlob** blob) {
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ID3DBlob* err = nullptr;
    HRESULT hr = D3DCompile(src, std::strlen(src), nullptr, nullptr, nullptr, entry, profile, flags, 0, blob, &err);
    if (FAILED(hr) && err) OutputDebugStringA((char*)err->GetBufferPointer());
    SAFE_RELEASE(err);
    return hr;
}

bool CreateDeviceAndSwapchain(HWND hwnd, UINT w, UINT h) {
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = w;
    sd.BufferDesc.Height = h;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    D3D_FEATURE_LEVEL fl;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &sd, &gdx.sc, &gdx.dev, &fl, &gdx.ctx
    );
    return SUCCEEDED(hr);
}

bool CreateRTVAndDSV(UINT w, UINT h) {
    ID3D11Texture2D* backTex = nullptr;
    if (FAILED(gdx.sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex))) return false;
    if (FAILED(gdx.dev->CreateRenderTargetView(backTex, nullptr, &gdx.rtv))) { SAFE_RELEASE(backTex); return false; }
    SAFE_RELEASE(backTex);

    SAFE_RELEASE(gdx.dsv);
    SAFE_RELEASE(gdx.dsvTx);

    D3D11_TEXTURE2D_DESC td{};
    td.Width = w; td.Height = h; td.MipLevels = 1; td.ArraySize = 1;
    td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    if (FAILED(gdx.dev->CreateTexture2D(&td, nullptr, &gdx.dsvTx))) return false;
    if (FAILED(gdx.dev->CreateDepthStencilView(gdx.dsvTx, nullptr, &gdx.dsv))) return false;

    gdx.ctx->OMSetRenderTargets(1, &gdx.rtv, gdx.dsv);

    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0; vp.TopLeftY = 0;
    vp.Width = (float)w; vp.Height = (float)h; vp.MinDepth = 0; vp.MaxDepth = 1;
    gdx.ctx->RSSetViewports(1, &vp);
    return true;
}

bool CreatePipeline() {
    ID3DBlob* vsb = nullptr; ID3DBlob* psb = nullptr;
    if (FAILED(Compile(gHLSL, "VSMain", "vs_5_0", &vsb))) return false;
    if (FAILED(Compile(gHLSL, "PSMain", "ps_5_0", &psb))) { SAFE_RELEASE(vsb); return false; }
    if (FAILED(gdx.dev->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &gVS))) { SAFE_RELEASE(vsb); SAFE_RELEASE(psb); return false; }
    if (FAILED(gdx.dev->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, &gPS))) { SAFE_RELEASE(vsb); SAFE_RELEASE(psb); return false; }

    D3D11_INPUT_ELEMENT_DESC il[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, (UINT)offsetof(Vtx,pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (UINT)offsetof(Vtx,col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    if (FAILED(gdx.dev->CreateInputLayout(il, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), &gIL))) { SAFE_RELEASE(vsb); SAFE_RELEASE(psb); return false; }
    SAFE_RELEASE(vsb); SAFE_RELEASE(psb);

    // Big dynamic vertex buffer (reused every frame)
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = (UINT)(sizeof(Vtx) * 262144);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(gdx.dev->CreateBuffer(&bd, nullptr, &gVB))) return false;

    D3D11_BUFFER_DESC cbd{};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = 64;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    if (FAILED(gdx.dev->CreateBuffer(&cbd, nullptr, &gCB))) return false;

    D3D11_BLEND_DESC bs{};
    bs.RenderTarget[0].BlendEnable = TRUE;
    bs.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bs.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bs.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bs.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bs.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    bs.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    if (FAILED(gdx.dev->CreateBlendState(&bs, &gBS))) return false;

    D3D11_DEPTH_STENCIL_DESC dsd{};
    dsd.DepthEnable = TRUE;
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    if (FAILED(gdx.dev->CreateDepthStencilState(&dsd, &gDSS))) return false;

    D3D11_RASTERIZER_DESC rs{};
    rs.FillMode = D3D11_FILL_SOLID;
    rs.CullMode = D3D11_CULL_NONE;
    rs.DepthClipEnable = TRUE;
    if (FAILED(gdx.dev->CreateRasterizerState(&rs, &gRS))) return false;

    return true;
}

void DestroyPipeline() {
    SAFE_RELEASE(gVS); SAFE_RELEASE(gPS); SAFE_RELEASE(gIL);
    SAFE_RELEASE(gVB); SAFE_RELEASE(gCB);
    SAFE_RELEASE(gBS); SAFE_RELEASE(gDSS); SAFE_RELEASE(gRS);
}

void DestroyRTVAndDSV() {
    SAFE_RELEASE(gdx.rtv); SAFE_RELEASE(gdx.dsv); SAFE_RELEASE(gdx.dsvTx);
}

void Resize(UINT w, UINT h) {
    if (!gdx.sc) return;
    gdx.width = w; gdx.height = h;
    DestroyRTVAndDSV();
    gdx.ctx->OMSetRenderTargets(0, nullptr, nullptr);
    gdx.sc->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
    CreateRTVAndDSV(w, h);
}

// --------------------------------------------------
// Rendering helpers
// --------------------------------------------------
size_t AppendRibbonToMappedBuffer(const std::vector<Vtx>& ribbon, size_t startOffset, D3D11_MAPPED_SUBRESOURCE& ms) {
    if (ribbon.empty()) return startOffset;
    size_t maxCount = (size_t)(262144); // based on gVB allocation
    size_t remain = maxCount - startOffset;
    size_t copyN = std::min(remain, ribbon.size());
    std::memcpy((Vtx*)ms.pData + startOffset, ribbon.data(), sizeof(Vtx) * copyN);
    return startOffset + copyN;
}

void DrawDebugBox(const Bounds& b) {
    // Simple line list box, 12 edges => 24 verts
    float xs[2] = { b.xMin,b.xMax }, ys[2] = { b.yMin,b.yMax }, zs[2] = { b.zMin,b.zMax };
    int edges[12][2][3] = {
        {{0,0,0},{1,0,0}}, {{0,1,0},{1,1,0}}, {{0,0,1},{1,0,1}}, {{0,1,1},{1,1,1}},
        {{0,0,0},{0,1,0}}, {{1,0,0},{1,1,0}}, {{0,0,1},{0,1,1}}, {{1,0,1},{1,1,1}},
        {{0,0,0},{0,0,1}}, {{1,0,0},{1,0,1}}, {{0,1,0},{0,1,1}}, {{1,1,0},{1,1,1}},
    };
    std::vector<Vtx> v; v.reserve(24);
    for (int e = 0; e < 12; ++e) {
        for (int k = 0; k < 2; ++k) {
            float x = xs[edges[e][k][0]];
            float y = ys[edges[e][k][1]];
            float z = zs[edges[e][k][2]];
            v.push_back({ XMFLOAT3(x,y,z), XMFLOAT4(0.8f,0.85f,0.95f,0.3f) });
        }
    }
    D3D11_MAPPED_SUBRESOURCE ms{};
    if (SUCCEEDED(gdx.ctx->Map(gVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
        std::memcpy(ms.pData, v.data(), sizeof(Vtx) * v.size());
        gdx.ctx->Unmap(gVB, 0);
        UINT stride = sizeof(Vtx), offset = 0;
        gdx.ctx->IASetVertexBuffers(0, 1, &gVB, &stride, &offset);
        gdx.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        gdx.ctx->IASetInputLayout(gIL);
        gdx.ctx->Draw((UINT)v.size(), 0);
    }
}

// --------------------------------------------------
// Frame update/draw
// --------------------------------------------------
void Update(float dt) {
    for (auto& tr : gapp.trails) {
        tr.SetWidth(gapp.trailWidth);
        tr.SetMaxPoints(gapp.trailMaxPoints);
        tr.Update(dt);
    }
}

void RenderFrame() {
    float clear[4] = { 0.10f,0.12f,0.16f,1.0f };
    gdx.ctx->ClearRenderTargetView(gdx.rtv, clear);
    gdx.ctx->ClearDepthStencilView(gdx.dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    gdx.ctx->VSSetShader(gVS, nullptr, 0);
    gdx.ctx->PSSetShader(gPS, nullptr, 0);
    gdx.ctx->RSSetState(gRS);
    float bf[4] = { 0,0,0,0 };
    gdx.ctx->OMSetBlendState(gBS, bf, 0xffffffff);
    gdx.ctx->OMSetDepthStencilState(gDSS, 0);

    // Camera
    XMMATRIX V = XMMatrixLookAtLH(XMLoadFloat3(&gapp.camPos), XMLoadFloat3(&gapp.camTarget), XMLoadFloat3(&gapp.camUp));
    float aspect = gdx.height ? (float)gdx.width / (float)gdx.height : 1.0f;
    XMMATRIX P = XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), aspect, 0.1f, 200.0f);
    XMMATRIX VP = XMMatrixTranspose(V * P);
    gdx.ctx->UpdateSubresource(gCB, 0, nullptr, &VP, 0, 0);
    gdx.ctx->VSSetConstantBuffers(0, 1, &gCB);

    // Build all ribbons into a single mapped buffer and draw one by one
    // (We reuse the dynamic buffer; simplest is map-per-trail for clarity)
    for (auto& tr : gapp.trails) {
        // viewDir from camera to target
        F3 viewDir = f3(gapp.camTarget.x - gapp.camPos.x,
            gapp.camTarget.y - gapp.camPos.y,
            gapp.camTarget.z - gapp.camPos.z);
        std::vector<Vtx> ribbon;
        tr.BuildRibbon(viewDir, ribbon);
        if (ribbon.size() < 4) continue;

        D3D11_MAPPED_SUBRESOURCE ms{};
        if (SUCCEEDED(gdx.ctx->Map(gVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms))) {
            size_t count = ribbon.size();
            std::memcpy(ms.pData, ribbon.data(), sizeof(Vtx) * count);
            gdx.ctx->Unmap(gVB, 0);

            UINT stride = sizeof(Vtx), offset = 0;
            gdx.ctx->IASetVertexBuffers(0, 1, &gVB, &stride, &offset);
            gdx.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            gdx.ctx->IASetInputLayout(gIL);
            gdx.ctx->Draw((UINT)count, 0);
        }
    }

    if (gapp.showDebugBox) DrawDebugBox(gapp.box);

    gdx.sc->Present(1, 0);
}

// --------------------------------------------------
// WinMain
// --------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    // Window
    WNDCLASSEX wc{}; wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc; wc.hInstance = hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DX11TrailEmitterDemoFull";
    RegisterClassEx(&wc);
    RECT rc{ 0,0,(LONG)gdx.width,(LONG)gdx.height }; AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"DX11 Trail Emitter Demo (Full)",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInst, nullptr);

    if (!CreateDeviceAndSwapchain(hwnd, gdx.width, gdx.height)) return 0;
    if (!CreateRTVAndDSV(gdx.width, gdx.height)) return 0;
    if (!CreatePipeline()) return 0;
    ShowWindow(hwnd, SW_SHOW);

    // App init: multiple trails with different phases/colors
    {
        TrailEmitter a(f3(0, 0, 3), XMFLOAT4(0.30f, 0.80f, 1.00f, 0.90f), 0.32f);
        a.SetPhase(0.0f); a.SetRadius(2.2f); a.SetMotionSpeed(1.00f); a.SetBaseHeight(0.4f); a.SetEmitInterval(0.018f);
        TrailEmitter b(f3(0, 0, 3), XMFLOAT4(1.00f, 0.55f, 0.25f, 0.85f), 0.26f);
        b.SetPhase(1.8f); b.SetRadius(2.6f); b.SetMotionSpeed(0.92f); b.SetBaseHeight(0.6f); b.SetEmitInterval(0.020f);
        TrailEmitter c(f3(0, 0, 3), XMFLOAT4(0.70f, 0.60f, 1.00f, 0.80f), 0.22f);
        c.SetPhase(3.3f); c.SetRadius(2.0f); c.SetMotionSpeed(1.12f); c.SetBaseHeight(0.8f); c.SetEmitInterval(0.016f);
        gapp.trails = { a,b,c };
        gapp.trailWidth = 0.30f;
        gapp.trailMaxPoints = 240;
    }

    auto tp = std::chrono::high_resolution_clock::now();
    MSG msg{}; bool running = true;
    while (running) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) running = false;
            TranslateMessage(&msg); DispatchMessage(&msg);
        }
        if (!running) break;

        auto tn = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(tn - tp).count();
        tp = tn;
        if (dt > (1.0f / 30.0f)) dt = 1.0f / 30.0f;

        Update(dt);
        RenderFrame();
    }

    DestroyPipeline();
    DestroyRTVAndDSV();
    SAFE_RELEASE(gdx.sc); SAFE_RELEASE(gdx.ctx); SAFE_RELEASE(gdx.dev);
    return 0;
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_SIZE:
        if (gdx.sc) {
            UINT ww = LOWORD(l), hh = HIWORD(l);
            if (ww && hh) Resize(ww, hh);
        }
        return 0;
    case WM_KEYDOWN:
        if (w == VK_ESCAPE) DestroyWindow(h);
        else if (w == 'B') gapp.showDebugBox = !gapp.showDebugBox;
        else if (w == '1') gapp.trailWidth = std::max(0.05f, gapp.trailWidth * 0.9f);
        else if (w == '2') gapp.trailWidth = std::min(1.2f, gapp.trailWidth * 1.1f);
        else if (w == '3') gapp.trailMaxPoints = std::max<size_t>(40, gapp.trailMaxPoints - 10);
        else if (w == '4') gapp.trailMaxPoints = std::min<size_t>(800, gapp.trailMaxPoints + 10);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0); return 0;
    }
    return DefWindowProc(h, m, w, l);
}
#endif
