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
	static XMFLOAT3 tangle{ XMFLOAT3(-50.0f, 5.0f, -50.0f) };
	static XMVECTOR ptemp, vtemp, mtemp, pang, vang, mang;
	XMFLOAT3 temp, atemp;
	XMFLOAT4X4 rotatemat;
	float ang{};

	if (i > v.size() - 1) {
		aniswitch = false;
		i = 0;
	}

	if (aniswitch) {

		if (j == 0.0f) {
			temp = v[i]->GetPosition();
			atemp = v[i]->GetLook();

			ptemp = XMVectorSet(prange.x, prange.y, prange.z, NULL);
			vtemp = XMVectorSet(temp.x, temp.y, temp.z, NULL);

			pang = XMVectorSet(tangle.x, tangle.y, tangle.z, NULL);
			vang = XMVectorSet(atemp.x, atemp.y, atemp.z, NULL);

			m_xmf3Look = atemp;
			m_xmf3Right = v[i]->GetRight();
			m_xmf3Up = v[i]->GetUp();

			prange = v[i]->GetPosition();
			tangle = v[i]->GetLook();
			++i;
		}
		mtemp = XMVectorLerp(ptemp, vtemp, j);
		//mang = XMVectorLerp(pang, vang, j);

		m_xmf3Position = Vector3::XMVectorToFloat3(mtemp);
		//m_xmf3Look = Vector3::XMVectorToFloat3(mang);

		//SetCameraOffset(m_xmf3Position);
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
