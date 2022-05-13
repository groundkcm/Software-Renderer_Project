#include <vector>
#include <random>
#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"
#define RAILNUM 177
#define	FIREWORKS 10
#define SQUARNUM (RAILNUM + FIREWORKS)

CScene::CScene(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

CScene::~CScene()
{
}

std::random_device rd;
std::default_random_engine dre(rd());
std::vector<CGameObject*> v;

void CScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	float fHalfWidth = 100.0f, fHalfHeight = 100.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, 30);

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(150, 255, 150));
	m_pWallsObject->SetPosition(0.0f, 0.0f, 0.0f);

	CCubeMesh* pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f); 
	m_nObjects = SQUARNUM;// ÆøÁ× 0~9 ³ª¸ÓÁö ·¹ÀÏ
	m_ppObjects = new CGameObject * [m_nObjects];
	v.reserve(RAILNUM);

	std::uniform_int_distribution<int> iuid(0, 255);
	std::uniform_real_distribution<float> suid(0.1f, 0.5f);
	std::uniform_real_distribution<float> puid(-80.0f, 80.0f);
	std::uniform_real_distribution<float> muid(10.0f, 30.0f);
	for (int i = 0; i < FIREWORKS; ++i) {
		m_ppObjects[i] = new CExplosiveObject();
		m_ppObjects[i]->SetMesh(pCubeMesh);
		m_ppObjects[i]->Scale(suid(dre), suid(dre), suid(dre));
		m_ppObjects[i]->SetColor(RGB(iuid(dre), iuid(dre), iuid(dre)));
		m_ppObjects[i]->SetPosition(puid(dre), 0.0f, puid(dre));
		m_ppObjects[i]->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_ppObjects[i]->SetMovingSpeed(muid(dre));
	}

	for (int i = FIREWORKS; i < SQUARNUM; ++i) {
		m_ppObjects[i] = new CCubeObject();
		m_ppObjects[i]->SetMesh(pCubeMesh);
		m_ppObjects[i]->Scale(1.5f, 0.1f, 1.5f);
		m_ppObjects[i]->SetColor(RGB(255, 0, 0));
		static XMFLOAT3 prange{};
		if (i < 25) {		//¿Ã¶ó°¡±â
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), -40.0f);
			m_ppObjects[i]->SetPosition(-50.0f, prange.y + 4.0f, -50.0f + (i - 10) * 5.0f);
		}
		else if (i < 35) {		//È¸Àü
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 9.0f*(i-25));
			if (i == 25)
				m_ppObjects[i]->SetPosition(prange.x + 0.5f, prange.y + 2.0f, prange.z + 3.0f);
			else if (i<30)
				m_ppObjects[i]->SetPosition(prange.x + 0.5f, prange.y, prange.z + 3.0f);
			else
				m_ppObjects[i]->SetPosition(prange.x + 3.0f, prange.y, prange.z + 2.0f);
		}
		else if (i < 45) {		//³«ÇÏ 1
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 40.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 90.0f);
			m_ppObjects[i]->SetPosition(prange.x + 5.0f, prange.y - 3.0f, prange.z);
		}
		else if (i < 55) {		//È¸Àü + »ó½Â
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), -20.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 90.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -9.0f * (i - 45));
			if (i==45)
				m_ppObjects[i]->SetPosition(prange.x + 3.0f, prange.y, prange.z + 0.5f);
			else if (i < 50)
				m_ppObjects[i]->SetPosition(prange.x + 3.0f, prange.y + 1.0f, prange.z + 0.5f);
			else
				m_ppObjects[i]->SetPosition(prange.x + 2.0f, prange.y + 1.0f, prange.z + 3.0f);
		}
		else if (i < 65) {		//»ó½Â + Á¶±Ý ºñÆ´
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), -40.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), -5.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -9.0f * (i - 55));
			if (i < 60)
				m_ppObjects[i]->SetPosition(prange.x - 1.0f, prange.y + 3.0f, prange.z + 3.0f);
			else
				m_ppObjects[i]->SetPosition(prange.x - 3.0f, prange.y + 4.0f, prange.z + 3.0f);
		}
		else if (i < 95) {		//È¸Àü ³«ÇÏ
			if (i < 75) {
				m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), -12.0f*(i-65));
				m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 30.0f);
				m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -90.0f);
				if (i == 65)
					m_ppObjects[i]->SetPosition(prange.x - 2.0f, prange.y + 1.0f, prange.z + 1.0f);
				else
					m_ppObjects[i]->SetPosition(prange.x - 2.0f, prange.y - 1.5f, prange.z + 1.0f);
			}
			else if (i < 85) {
				m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), -12.0f * (i - 65));
				m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 30.0f);
				m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -90.0f);
				m_ppObjects[i]->SetPosition(prange.x - 2.0f, prange.y - 1.5f, prange.z - 1.0f);
			}
			else {
				m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), -12.0f * (i - 65));
				m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 30.0f);
				m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -90.0f);
				m_ppObjects[i]->SetPosition(prange.x - 2.0f, prange.y - 1.5f, prange.z + 1.0f);
			}
		}
		else if (i < 105) {		//È¸Àü
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), -20.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -90.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -9.0f * (i - 95));
			if (i == 95)
				m_ppObjects[i]->SetPosition(prange.x - 3.0f, prange.y, prange.z - 0.5f);
			else if (i < 100)
				m_ppObjects[i]->SetPosition(prange.x - 3.0f, prange.y + 1.0f, prange.z - 0.5f);
			else
				m_ppObjects[i]->SetPosition(prange.x - 2.0f, prange.y + 1.0f, prange.z - 3.0f);
		}
		else if (i < 115) {		//»ó½Â + Á¶±Ý ºñÆ´
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), -40.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 180.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), -5.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 9.0f * (i - 105));
			if (i < 110)
				m_ppObjects[i]->SetPosition(prange.x, prange.y + 3.0f, prange.z - 3.0f);
			else
				m_ppObjects[i]->SetPosition(prange.x + 0.5f, prange.y + 3.0f, prange.z - 3.0f);
		}
		else if (i < 130) {		//³«ÇÏ
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 40.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 180.0f);
			if (i==115)
				m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -9.0f);
			m_ppObjects[i]->SetPosition(prange.x, prange.y - 3.0f, prange.z - 5.0f);
		}
		else if (i < 140) {		//È¸Àü
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -180.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -9.0f * (i - 130));
			if (i == 130)
				m_ppObjects[i]->SetPosition(prange.x + 0.5f, prange.y - 2.0f, prange.z - 3.0f);
			else if (i < 135)
				m_ppObjects[i]->SetPosition(prange.x + 0.5f, prange.y - 1.0f, prange.z - 3.0f);
			else
				m_ppObjects[i]->SetPosition(prange.x + 3.0f, prange.y - 1.0f, prange.z - 2.0f);
		}
		else if (i < 145) {		//³«ÇÏ
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 20.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 90.0f);
			/*if (i==115)
				m_ppObjects[i]->SetPosition(prange.x, prange.y - 4.0f, prange.z - 5.0f);
			else*/
			m_ppObjects[i]->SetPosition(prange.x + 4.0f, prange.y - 2.0f, prange.z);
		}
		else if (i < 155) {		//È¸Àü
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 90.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), -2.0f * (i - 145));
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 9.0f * (i - 145));
			if (i == 145)
				m_ppObjects[i]->SetPosition(prange.x + 3.0f, prange.y - 2.0f, prange.z - 0.5f);
			else if (i < 150)
				m_ppObjects[i]->SetPosition(prange.x + 3.0f, prange.y, prange.z - 0.5f);
			else
				m_ppObjects[i]->SetPosition(prange.x + 2.0f, prange.y, prange.z - 3.0f);
		}
		else if (i < 165) {		//È¸Àü
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 180.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 0.0f, 1.0f), -20.0f + 2.0f * (i - 155));
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 9.0f * (i - 155));
			if (i < 160)
				m_ppObjects[i]->SetPosition(prange.x - 0.5f, prange.y, prange.z - 3.0f);
			else
				m_ppObjects[i]->SetPosition(prange.x - 3.0f, prange.y, prange.z - 2.0f);
		}
		else if (i < 167) {		//ÀÌµ¿
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -90.0f);
			m_ppObjects[i]->SetPosition(prange.x - 5.0f, prange.y, prange.z);
		}
		else if (i < 177) {		//È¸Àü
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), -90.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 9.0f * (i - 165));
			if (i < 172)
				m_ppObjects[i]->SetPosition(prange.x - 3.0f, prange.y, prange.z + 0.5f);
			else
				m_ppObjects[i]->SetPosition(prange.x - 2.0f, prange.y, prange.z + 3.0f);
		}
		else {
			m_ppObjects[i]->SetPosition(prange.x, prange.y, prange.z + 2.0f);
		}
		v.push_back(m_ppObjects[i]);
		prange = m_ppObjects[i]->GetPosition();
	}


#ifdef _WITH_DRAW_AXIS
	m_pWorldAxis = new CGameObject();
	CAxisMesh* pAxisMesh = new CAxisMesh(0.5f, 0.5f, 0.5f);
	m_pWorldAxis->SetMesh(pAxisMesh);
#endif
}

void CScene::ReleaseObjects()
{
	if (CExplosiveObject::m_pExplosionMesh) CExplosiveObject::m_pExplosionMesh->Release();

	for (int i = 0; i < m_nObjects; i++) if (m_ppObjects[i]) delete m_ppObjects[i];
	if (m_ppObjects) delete[] m_ppObjects;

	if (m_pWallsObject) delete m_pWallsObject;

#ifdef _WITH_DRAW_AXIS
	if (m_pWorldAxis) delete m_pWorldAxis;
#endif
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	static int num1, num3, quarternum, resetnum;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
			if (++num1 % 2)
				m_pPlayer->character = false;
			else {
				m_pPlayer->character = true;
				m_pPlayer->behind = false;
				m_pPlayer->quarter = false;
			}
			break;
		case '3':
			if (++num3 % 2)
				m_pPlayer->behind = false;
			else {
				m_pPlayer->behind = true;
				m_pPlayer->character = false;
				m_pPlayer->quarter = false;
				m_pPlayer->reset = false;
			}
			break;
		case 'Q':
			if (++quarternum % 2)
				m_pPlayer->quarter = false;
			else {
				m_pPlayer->quarter = true;
				m_pPlayer->character = false;
				m_pPlayer->reset = false; 
				m_pPlayer->behind = false;
			}
			break;
		case 'R':
			if (++resetnum % 2)
				m_pPlayer->reset = false;
			else
				m_pPlayer->reset = true;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void CScene::FireWorks()
{
	XMFLOAT3 pcheck;
	std::uniform_real_distribution<float> uid(70.0f, 100.0f);

	for (int i{}; i < FIREWORKS; ++i) {
		pcheck = m_ppObjects[i]->GetPosition();
		if (pcheck.y > uid(dre))
			((CExplosiveObject*)m_ppObjects[i])->m_bBlowingUp = true;
	}
}

void CScene::Animate(float fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);

	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Animate(fElapsedTime);

	FireWorks();
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera)
{
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);

	CGraphicsPipeline::SetViewPerspectiveProjectTransform(&pCamera->m_xmf4x4ViewPerspectiveProject);
	m_pWallsObject->Render(hDCFrameBuffer, pCamera);
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);

	if (m_pPlayer) m_pPlayer->Render(hDCFrameBuffer, pCamera);

//UI
#ifdef _WITH_DRAW_AXIS
	CGraphicsPipeline::SetViewOrthographicProjectTransform(&pCamera->m_xmf4x4ViewOrthographicProject);
	m_pWorldAxis->SetRotationTransform(&m_pPlayer->m_xmf4x4World);
	m_pWorldAxis->Render(hDCFrameBuffer, pCamera);
#endif
}
