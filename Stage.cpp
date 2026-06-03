#include "Stage.h"
#include <string>
#include <vector>
#include "Engine//Model.h"
#include "resource.h"
#include <cassert>
#include "Engine/camera.h"
#include "Engine/Input.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"



Stage::Stage(GameObject* parent)
	:GameObject(parent, "Stage"),  pConstantBuffer_(nullptr)
{
	//Initialize()でモデルを読み込むまでは、モデル番号は-1にしておく
	hball_ = -1;
	hRoom_ = -1;
	hGround_ = -1;
	hDonut_ = -1;

	
}

Stage::~Stage()
{
}


void Stage::InitConstantBuffer()
{
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANTBUFFER_STAGE);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// コンスタントバッファの作成
	HRESULT hr;
	hr = Direct3D::pDevice->CreateBuffer(&cb, nullptr, &pConstantBuffer_);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"コンスタントバッファの作成に失敗しました", L"エラー", MB_OK);
	}
}

void Stage::Initialize()
{
	InitConstantBuffer();
	hball_ = Model::Load("ball.fbx");
	assert(hball_ >= 0);
	hRoom_ = Model::Load("room.fbx");
	assert(hRoom_ >= 0);
	hGround_ = Model::Load("plane3.fbx");
	assert(hGround_ >= 0);
	hDonut_ = Model::Load("Donut_phong.fbx");
	assert(hDonut_ >= 0);
	//pMelbourne_ = new Sprite(L"Assets\\melbourne.png");
	Camera::SetPosition({ 0, 0.8, -2.8 });
	Camera::SetTarget({ 0,0.8,0 });

    // サンプラーステート作成
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;  // トライリニアフィルタ
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;     // リピート
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.BorderColor[0] = 1.0f;
	sd.BorderColor[1] = 1.0f;
	sd.BorderColor[2] = 1.0f;
	sd.BorderColor[3] = 1.0f;
    sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

	ID3D11SamplerState* pShadowSampler = nullptr;
    HRESULT hr = Direct3D::pDevice->CreateSamplerState(&sd, &pShadowSampler);
	Direct3D::pContext->PSSetSamplers(1, 1, &pShadowSampler);	//スロット1にシャドウマップ用サンプラーをセット
	SAFE_RELEASE(pShadowSampler);
}

void Stage::Update()
{
    transform_.rotate_.y += 0.5f;
    if (Input::IsKey(DIK_A))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x - 0.01f,p.y, p.z,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_D))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x + 0.01f,p.y, p.z,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_W))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x,p.y, p.z + 0.01f,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_S))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x ,p.y, p.z - 0.01f,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_UP))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x,p.y + 0.01f, p.z,p.w };
        Direct3D::SetLightPos(p);
    }
    if (Input::IsKey(DIK_DOWN))
    {
        XMFLOAT4 p = Direct3D::GetLightPos();
        p = { p.x ,p.y - 0.01f, p.z,p.w };
        Direct3D::SetLightPos(p);
    }

    //コンスタントバッファの設定と、シェーダーへのコンスタントバッファのセットを書くよ
    CONSTANTBUFFER_STAGE cb;
    cb.lightPosition = Direct3D::GetLightPos();
    XMStoreFloat4(&cb.eyePosition, Camera::GetPosition());

	//ライトのビュー射影行列は、ライトの位置と向きから計算する
    XMMATRIX lightV = Direct3D::GetLightViewMatrix();
	XMMATRIX lightP = Direct3D::GetLightProjectionMatrix();
	XMMATRIX lightVP = lightV * lightP;
	XMStoreFloat4x4(&cb.matLightVP, lightVP);

    D3D11_MAPPED_SUBRESOURCE pdata;
    Direct3D::pContext->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPUからのデータアクセスを止める
    memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// データを値を送る
    Direct3D::pContext->Unmap(pConstantBuffer_, 0);	//再開

    //コンスタントバッファ
    Direct3D::pContext->VSSetConstantBuffers(1, 1, &pConstantBuffer_);	//頂点シェーダー用	
    Direct3D::pContext->PSSetConstantBuffers(1, 1, &pConstantBuffer_);	//ピクセルシェーダー用
}

void Stage::Draw()
{
    Transform ltr;
    ltr.position_ = { Direct3D::GetLightPos().x,Direct3D::GetLightPos().y,Direct3D::GetLightPos().z };
    ltr.scale_ = { 0.1,0.1,0.1 };
    Model::SetTransform(hball_, ltr);


    Transform tr;
    tr.position_ = { 0, 0, 0 };
    tr.rotate_ = { 0, 180, 0 };
    //Model::SetTransform(hGround, tr);
    //Model::Draw(hGround);

    Model::SetTransform(hRoom_, tr);

    static Transform tDonut;
    tDonut.scale_ = { 0.3f, 0.3f, 0.3f };
    tDonut.position_ = { 0, 0.5, 0.5 };
    tDonut.rotate_.y += 0.1;
    Model::SetTransform(hDonut_, tDonut);

	//1回目の描画でシャドウマップを作る
	Direct3D::BeginShadowPass();
	Model::DrawShadowMap(hDonut_);
	Direct3D::EndShadowPass();

	ID3D11ShaderResourceView* pShadowSRV = Direct3D::GetShadowMapSRV();
	Direct3D::pContext->PSSetShaderResources(1, 1, &pShadowSRV);	//スロット1にシャドウマップをセット

	//2回目の描画で通常描画
	Model::Draw(hball_);
	Model::Draw(hRoom_);
	Model::Draw(hDonut_);

	ID3D11ShaderResourceView* nullSRV = nullptr;
	Direct3D::pContext->PSSetShaderResources(1, 1, &nullSRV);	//スロット1にnullをセットしてシャドウマップの使用を止める

    //Imgui
	ImGui::Text("Stage Class rot:%lf", tDonut.rotate_.z);
}

void Stage::Release()
{
}



