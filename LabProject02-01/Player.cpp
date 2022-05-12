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
	static int i{};
	static float j{};
	static XMFLOAT3 prange{ XMFLOAT3(-50.0f, 5.0f, -100.0f) };
	static XMFLOAT3 tangle{ XMFLOAT3(-50.0f, 5.0f, -100.0f) };
	static XMVECTOR ptemp, vtemp, mtemp, pang, vang, mang;
	XMFLOAT3 temp, atemp;
	XMFLOAT4X4 rotatemat;
	float ang{};

	if (i > 104) aniswitch = false;

	if (aniswitch) {

		if (j == 0.0f) {
			temp = v[i]->GetPosition();
			atemp = v[i]->GetLook();

			ptemp = XMVectorSet(prange.x, prange.y, prange.z, NULL);
			vtemp = XMVectorSet(temp.x, temp.y, temp.z, NULL);

			pang = XMVectorSet(tangle.x, tangle.y, tangle.z, NULL);
			vang = XMVectorSet(atemp.x, atemp.y, atemp.z, NULL);

			mang = XMVector3Dot(pang, vang);
			ang += XMVectorGetX(mang);
			ang += XMVectorGetY(mang);
			ang += XMVectorGetZ(mang);

			rotatemat = Matrix4x4::RotationAxis(tangle, ang);
			m_xmf4x4World = Matrix4x4::Multiply(rotatemat, m_xmf4x4World);

			prange = v[i]->GetPosition();
			tangle = v[i]->GetLook();

			++i;
		}
		mtemp = XMVectorLerp(ptemp, vtemp, j);

		m_xmf3Position = Vector3::XMVectorToFloat3(mtemp);
		//SetCameraOffset(m_xmf3Position);
		if (i < 15)		//�ö󰡱�
			j += 0.03f;
		else if (i < 25) //ȸ��
			j += 0.04f;
		else if (i < 35) //���� 1
			j += 0.15f;
		else if (i < 45) //ȸ�� + ���
			j += 0.1f;
		else if (i < 55) //��� + ���� ��ƴ
			j += 0.1f;
		else if (i < 85) //ȸ�� ����
			j += 0.15f;
		else if (i < 95) //ȸ��
			j += 0.1f;
		else
			j += 0.05f;
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
CAirplanePlayer::CAirplanePlayer()		//�Ѿ� ��������
{
	CCubeMesh* pBulletMesh = new CCubeMesh(1.0f, 4.0f, 1.0f);
	for (int i = 0; i < BULLETS; i++)
	{
		m_ppBullets[i] = new CBulletObject(m_fBulletEffectiveRange);
		m_ppBullets[i]->SetMesh(pBulletMesh);
		m_ppBullets[i]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_ppBullets[i]->SetRotationSpeed(360.0f);
		m_ppBullets[i]->SetMovingSpeed(120.0f);
		m_ppBullets[i]->SetActive(false);
	}
}

CAirplanePlayer::~CAirplanePlayer()
{
}

void CAirplanePlayer::Animate(float fElapsedTime)
{
	CPlayer::Animate(fElapsedTime);
}

void CAirplanePlayer::OnUpdateTransform()
{
	CPlayer::OnUpdateTransform();

	m_xmf4x4World = Matrix4x4::Multiply(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f), m_xmf4x4World);
}

void CAirplanePlayer::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CPlayer::Render(hDCFrameBuffer, pCamera);

	for (int i = 0; i < BULLETS; i++) if (m_ppBullets[i]->m_bActive) m_ppBullets[i]->Render(hDCFrameBuffer, pCamera);
}

//void CAirplanePlayer::FireBullet(CGameObject* pLockedObject)
//{
///*
//	if (pLockedObject) 
//	{
//		LookAt(pLockedObject->GetPosition(), XMFLOAT3(0.0f, 1.0f, 0.0f));
//		OnUpdateTransform();
//	}
//*/
//
//	CBulletObject* pBulletObject = NULL;
//	for (int i = 0; i < BULLETS; i++)
//	{
//		if (!m_ppBullets[i]->m_bActive)
//		{
//			pBulletObject = m_ppBullets[i];
//			break;
//		}
//	}
//
//	if (pBulletObject)
//	{
//		XMFLOAT3 xmf3Position = GetPosition();
//		XMFLOAT3 xmf3Direction = GetUp();
//		XMFLOAT3 xmf3FirePosition = Vector3::Add(xmf3Position, Vector3::ScalarProduct(xmf3Direction, 6.0f, false));
//
//		pBulletObject->m_xmf4x4World = m_xmf4x4World;
//
//		pBulletObject->SetFirePosition(xmf3FirePosition);
//		pBulletObject->SetMovingDirection(xmf3Direction);
//		pBulletObject->SetColor(RGB(255, 0, 0));
//		pBulletObject->SetActive(true);
//
//		if (pLockedObject)
//		{
//			pBulletObject->m_pLockedObject = pLockedObject;
//			pBulletObject->SetColor(RGB(0, 0, 255));
//		}
//	}
//}
