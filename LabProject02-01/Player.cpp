#include <vector>
#include "stdafx.h"
#include "Player.h"
#include "GameFramework.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::SetPosition(float x, float y, float z)
{
	m_xmf3Position = XMFLOAT3(x, y, z);

	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetCameraOffset(XMFLOAT3& xmf3CameraOffset)
{
	m_xmf3CameraOffset = xmf3CameraOffset;
	m_pCamera->SetLookAt(Vector3::Add(m_xmf3Position, m_xmf3CameraOffset), m_xmf3Position, m_xmf3Up);
	m_pCamera->GenerateViewMatrix();
}

void CPlayer::Move(DWORD dwDirection, float fDistance)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, true);
	}
}

void CPlayer::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(xmf3Shift, m_xmf3Position);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Move(float x, float y, float z)
{
	Move(XMFLOAT3(x, y, z), false);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll)
{
	m_pCamera->Rotate(fPitch, fYaw, fRoll);
	if (fPitch != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(fPitch));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
	}
	if (fYaw != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(fYaw));
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}
	if (fRoll != 0.0f)
	{
		XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(fRoll));
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, mtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, mtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Up, m_xmf3Look));
	m_xmf3Up = Vector3::Normalize(Vector3::CrossProduct(m_xmf3Look, m_xmf3Right));
}

void CPlayer::LookAt(XMFLOAT3& xmf3LookAt, XMFLOAT3& xmf3Up)
{
	XMFLOAT4X4 xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, xmf3LookAt, xmf3Up);
	m_xmf3Right = Vector3::Normalize(XMFLOAT3(xmf4x4View._11, xmf4x4View._21, xmf4x4View._31));
	m_xmf3Up = Vector3::Normalize(XMFLOAT3(xmf4x4View._12, xmf4x4View._22, xmf4x4View._32));
	m_xmf3Look = Vector3::Normalize(XMFLOAT3(xmf4x4View._13, xmf4x4View._23, xmf4x4View._33));
}

void CPlayer::Update(float fTimeElapsed)
{
	Move(m_xmf3Velocity, false);

	m_pCamera->Update(this, m_xmf3Position, fTimeElapsed);
	m_pCamera->GenerateViewMatrix();

	XMFLOAT3 xmf3Deceleration = Vector3::Normalize(Vector3::ScalarProduct(m_xmf3Velocity, -1.0f));
	float fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = m_fFriction * fTimeElapsed;
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Deceleration, fDeceleration);
}

extern std::vector<CGameObject*> v;

void CPlayer::Animate(float fElapsedTime)
{
	static int i;
	static float j;
	static XMFLOAT3 prange{ XMFLOAT3(-50.0f, 5.0f, -50.0f) };
	static XMFLOAT3 tangle{ v[0]->GetLook() };
	static XMFLOAT3 rangle{ v[0]->GetRight() };
	static XMFLOAT3 uangle{ v[0]->GetUp() };
	static XMVECTOR ptemp, vtemp, mtemp;
	XMFLOAT3 temp;

	if (reset) {
		reset = false;
		aniswitch = false;
		quarter = false;
		character = false;
		behind = false;
		i = 0, j = 0;
		m_xmf3Position = XMFLOAT3(-50.0f, 5.0f, -50.0f);
		m_xmf3Look = v[0]->GetLook();
		m_xmf3Right = v[0]->GetRight();
		m_xmf3Up = v[0]->GetUp();
		m_pCamera->SetView(m_xmf3Position);
	}
	if (quarter)
		m_pCamera->SetLookAt(XMFLOAT3(-100.0f, 150.0f, -100.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-100.0f, 200.0f, -100.0f));
	else if (character)
		m_pCamera->SetLookAt(XMFLOAT3(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z + 4.0f), v[i]->GetLook(), v[i]->GetUp());
	else if (behind)
		m_pCamera->SetLookAt(XMFLOAT3(m_xmf3Position.x - 10.0f, m_xmf3Position.y + 10.0f, m_xmf3Position.z - 20.0f), m_xmf3Position, XMFLOAT3(-100.0f, 200.0f, -100.0f));
	else
		m_pCamera->SetLookAt(XMFLOAT3(m_xmf3Position.x, m_xmf3Position.y + 10.0f, m_xmf3Position.z - 20.0f), m_xmf3Position, XMFLOAT3(0.0f, 200.0f, 0.0f));

	if (i > v.size() - 1) {
		aniswitch = false;
		quarter = false;
		character = false;
		behind = false;
		i = 0;
	}
	static XMVECTOR plook, vlook, mlook, pright, vright, mright, pup, vup, mup;
	XMFLOAT3 ltemp, rtemp, utemp;
	if (aniswitch) {

		if (j == 0.0f) {
			temp = v[i]->GetPosition();
			ltemp = v[i]->GetLook();
			rtemp = v[i]->GetRight();
			utemp = v[i]->GetUp();

			ptemp = XMVectorSet(prange.x, prange.y, prange.z, NULL);
			vtemp = XMVectorSet(temp.x, temp.y, temp.z, NULL);

			plook = XMVectorSet(tangle.x, tangle.y, tangle.z, NULL);
			vlook = XMVectorSet(ltemp.x, ltemp.y, ltemp.z, NULL);

			pright = XMVectorSet(rangle.x, rangle.y, rangle.z, NULL);
			vright = XMVectorSet(rtemp.x, rtemp.y, rtemp.z, NULL);

			pup = XMVectorSet(uangle.x, uangle.y, uangle.z, NULL);
			vup = XMVectorSet(utemp.x, utemp.y, utemp.z, NULL);

			prange = v[i]->GetPosition();
			tangle = v[i]->GetLook();
			rangle = v[i]->GetRight();
			uangle = v[i]->GetUp();
			++i;
		}
		mtemp = XMVectorLerp(ptemp, vtemp, j);
		mlook = XMVectorLerp(plook, vlook, j);
		mright = XMVectorLerp(pright, vright, j);
		mup = XMVectorLerp(pup, vup, j);

		m_xmf3Position = Vector3::XMVectorToFloat3(mtemp);
		m_xmf3Look = Vector3::XMVectorToFloat3(mlook);
		m_xmf3Right = Vector3::XMVectorToFloat3(mright);
		m_xmf3Up = Vector3::XMVectorToFloat3(mup);
		//m_pCamera->SetView(m_xmf3Position);
		//LookAt(m_xmf3Position, m_xmf3Up);


		if (i < 15)		//¿Ã¶ó°¡±â
			j += 0.05f;
		else if (i < 25) //È¸Àü
			j += 0.07f;
		else if (i < 35) //³«ÇÏ 1
			j += 0.15f;
		else if (i < 45) //È¸Àü + »ó½Â
			j += 0.15f;
		else if (i < 55) //»ó½Â + Á¶±Ý ºñÆ´
			j += 0.12f;
		else if (i < 85) //È¸Àü ³«ÇÏ
			j += 0.15f;
		else if (i < 95) //È¸Àü
			j += 0.15f;
		else if (i < 105) //»ó½Â + Á¶±Ý ºñÆ´
			j += 0.12f;
		else if (i < 120) //³«ÇÏ
			j += 0.15f;
		else if (i < 130) //È¸Àü
			j += 0.13f;
		else if (i < 135) //³«ÇÏ
			j += 0.15f;
		else if (i < 145) //È¸Àü
			j += 0.13f;
		else if (i < 155) //È¸Àü
			j += 0.12f;
		else if (i < 157) //ÀÌµ¿
			j += 0.1f;
		else if (i < 167) //È¸Àü
			j += 0.08f;
		else			  //ÀÌµ¿
			j += 0.07f;
		if (j > 1.0f) j = 0.0f;
	}

	OnUpdateTransform();

	CGameObject::Animate(fElapsedTime);
}

void CPlayer::OnUpdateTransform()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y; m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}

void CPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGameObject::Render(hDCFrameBuffer, pCamera);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
CRollerCosterPlayer::CRollerCosterPlayer()
{
}

CRollerCosterPlayer::~CRollerCosterPlayer()
{
}

void CRollerCosterPlayer::Animate(float fElapsedTime)
{
	CPlayer::Animate(fElapsedTime);
}

void CRollerCosterPlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
}

void CRollerCosterPlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CPlayer::Render(hDCFrameBuffer, pCamera);
}
