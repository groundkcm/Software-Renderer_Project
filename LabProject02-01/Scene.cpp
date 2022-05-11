#include <random>
#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"


CScene::CScene(CPlayer* pPlayer)
{
	m_pPlayer = pPlayer;
}

CScene::~CScene()
{
}

std::random_device rd;
std::default_random_engine dre(rd());

void CScene::BuildObjects()
{
	CExplosiveObject::PrepareExplosion();

	float fHalfWidth = 100.0f, fHalfHeight = 100.0f, fHalfDepth = 100.0f;
	CWallMesh* pWallCubeMesh = new CWallMesh(fHalfWidth * 2.0f, fHalfHeight * 2.0f, fHalfDepth * 2.0f, 30);

	m_pWallsObject = new CWallsObject();
	m_pWallsObject->SetMesh(pWallCubeMesh);
	m_pWallsObject->SetColor(RGB(150, 255, 150));
	m_pWallsObject->SetPosition(0.0f, 100.0f, 0.0f);
	m_pWallsObject->m_pxmffloorPlanes[0] = XMFLOAT4(+1.0f, 0.0f, 0.0f, fHalfWidth);
	m_pWallsObject->m_pxmffloorPlanes[1] = XMFLOAT4(-1.0f, 0.0f, 0.0f, fHalfWidth);		//바닥
	m_pWallsObject->m_pxmffloorPlanes[2] = XMFLOAT4(0.0f, +1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmffloorPlanes[3] = XMFLOAT4(0.0f, -1.0f, 0.0f, fHalfHeight);
	m_pWallsObject->m_pxmffloorPlanes[4] = XMFLOAT4(0.0f, 0.0f, +1.0f, fHalfDepth);
	m_pWallsObject->m_pxmffloorPlanes[5] = XMFLOAT4(0.0f, 0.0f, -1.0f, fHalfDepth);
	//m_pWallsObject->m_xmOOBBPlayerMoveCheck = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(fHalfWidth * 0.1f, fHalfHeight, fHalfDepth * 0.1f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	//m_pWallsObject->MoveUp(100.0f);

	//CCubeMesh* pRailMesh = new CCubeMesh(6.0f, 0.5f, 6.0f);
	CCubeMesh* pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);

	// 폭죽 0~9 나머지 레일
	m_nObjects = 110;
	m_ppObjects = new CGameObject * [m_nObjects];

	std::uniform_int_distribution<int> iuid(0, 255);
	std::uniform_int_distribution<int> choice(0, 1);
	std::uniform_real_distribution<float> suid(1.0f, 2.5f);
	std::uniform_real_distribution<float> puid(-90.0f, 90.0f);
	std::uniform_real_distribution<float> muid(0.0f, 45.0f);
	for (int i = 0; i < 10; ++i) {
		m_ppObjects[i] = new CExplosiveObject();
		m_ppObjects[i]->SetMesh(pCubeMesh);
		m_ppObjects[i]->Scale(suid(dre), suid(dre), suid(dre));
		m_ppObjects[i]->SetColor(RGB(iuid(dre), iuid(dre), iuid(dre)));
		m_ppObjects[i]->SetPosition(puid(dre), 0.0f, puid(dre));
		m_ppObjects[i]->SetRotationAxis(XMFLOAT3((float)choice(dre), (float)choice(dre), (float)choice(dre)));
		m_ppObjects[i]->SetRotationSpeed(muid(dre)*2);
		m_ppObjects[i]->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_ppObjects[i]->SetMovingSpeed(muid(dre));
	}

	std::uniform_real_distribution<float> uid(0.0f, 2.5f);
	std::uniform_real_distribution<float> miuid(0.0f, 1.0f);
	for (int i = 10; i < 110; ++i) {
		m_ppObjects[i] = new CExplosiveObject();
		m_ppObjects[i]->SetMesh(pCubeMesh);
		m_ppObjects[i]->Scale(1.5f, 0.1f, 1.5f);
		m_ppObjects[i]->SetColor(RGB(0, 0, 0));
		static XMFLOAT3 tangle{};		//조금씩 이동
		static XMFLOAT3 prange{};
		float randnum{};
		if (i < 25) {
			randnum = uid(dre);
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), -40.0f);
			m_ppObjects[i]->SetPosition(0.0f, prange.y + 4.0f, -100.0f + (i - 10) * 5.0f);
		}
		else if (i >= 25 && i < 35) {
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 9.0f*(i-25));
			m_ppObjects[i]->SetPosition(prange.x + 1.0f, prange.y, prange.z + 3.0f);
		}
		else if (i >= 35 && i < 50) {
			randnum = uid(dre);
			m_ppObjects[i]->Rotate(XMFLOAT3(1.0f, 0.0f, 0.0f), 40.0f);
			m_ppObjects[i]->Rotate(XMFLOAT3(0.0f, 1.0f, 0.0f), 90.0f);
			m_ppObjects[i]->SetPosition(prange.x + 5.0f, prange.y - 4.0f, prange.z);
		}
		else if (i >= 50 && i < 75) {

		}
		else if (i >= 75 && i < 90) {

		}
		else {
			m_ppObjects[i]->Rotate(-uid(dre), 0, 0);
		}
		prange = m_ppObjects[i]->GetPosition();

		/*XMFLOAT3 t{ m_ppObjects[i]->GetLook() };
		if(t.x != 0)
			if (t.x < 0)
				m_ppObjects[i]->SetPosition(-miuid(dre), 0.5f + i* miuid(dre), -100.0f + (i-10)*5.0f);
			else 
				m_ppObjects[i]->SetPosition(miuid(dre), 0.5f + i * miuid(dre), -100.0f + (i - 10) * 5.0f);
		else
			m_ppObjects[i]->SetPosition(0.0f, 0.5f + i * miuid(dre), -100.0f + (i - 10) * 5.0f);*/
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
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_ppObjects[int(wParam - '1')];
			pExplosiveObject->m_bBlowingUp = true;
			break;
		}
		case '0':
			for (int i = 0; i < m_nObjects; i++)
			{
				CExplosiveObject* pExplosiveObject = (CExplosiveObject*)m_ppObjects[i];
				pExplosiveObject->m_bBlowingUp = true;
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

CGameObject* CScene::PickObjectPointedByCursor(int xClient, int yClient, CCamera* pCamera)
{
	XMFLOAT3 xmf3PickPosition;
	xmf3PickPosition.x = (((2.0f * xClient) / (float)pCamera->m_Viewport.m_nWidth) - 1) / pCamera->m_xmf4x4PerspectiveProject._11;
	xmf3PickPosition.y = -(((2.0f * yClient) / (float)pCamera->m_Viewport.m_nHeight) - 1) / pCamera->m_xmf4x4PerspectiveProject._22;
	xmf3PickPosition.z = 1.0f;

	XMVECTOR xmvPickPosition = XMLoadFloat3(&xmf3PickPosition);
	XMMATRIX xmmtxView = XMLoadFloat4x4(&pCamera->m_xmf4x4View);

	int nIntersected = 0;
	float fNearestHitDistance = FLT_MAX;
	CGameObject* pNearestObject = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		float fHitDistance = FLT_MAX;
		nIntersected = m_ppObjects[i]->PickObjectByRayIntersection(xmvPickPosition, xmmtxView, &fHitDistance);
		if ((nIntersected > 0) && (fHitDistance < fNearestHitDistance))
		{
			fNearestHitDistance = fHitDistance;
			pNearestObject = m_ppObjects[i];
		}
	}
	return(pNearestObject);
}

void CScene::CheckObjectByObjectCollisions()
{
	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->m_pObjectCollided = NULL;
	for (int i = 0; i < m_nObjects; i++)
	{
		for (int j = (i + 1); j < m_nObjects; j++)
		{
			if (m_ppObjects[i]->m_xmOOBB.Intersects(m_ppObjects[j]->m_xmOOBB))
			{
				m_ppObjects[i]->m_pObjectCollided = m_ppObjects[j];
				m_ppObjects[j]->m_pObjectCollided = m_ppObjects[i];
			}
		}
	}
	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i]->m_pObjectCollided)
		{
			XMFLOAT3 xmf3MovingDirection = m_ppObjects[i]->m_xmf3MovingDirection;
			float fMovingSpeed = m_ppObjects[i]->m_fMovingSpeed;
			m_ppObjects[i]->m_xmf3MovingDirection = m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection;
			m_ppObjects[i]->m_fMovingSpeed = m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_xmf3MovingDirection = xmf3MovingDirection;
			m_ppObjects[i]->m_pObjectCollided->m_fMovingSpeed = fMovingSpeed;
			m_ppObjects[i]->m_pObjectCollided->m_pObjectCollided = NULL;
			m_ppObjects[i]->m_pObjectCollided = NULL;
		}
	}
}


void CScene::CheckPlayerByWallCollision()
{
	BoundingOrientedBox xmOOBBPlayerMoveCheck;
	m_pWallsObject->m_xmOOBBPlayerMoveCheck.Transform(xmOOBBPlayerMoveCheck, XMLoadFloat4x4(&m_pWallsObject->m_xmf4x4World));
	XMStoreFloat4(&xmOOBBPlayerMoveCheck.Orientation, XMQuaternionNormalize(XMLoadFloat4(&xmOOBBPlayerMoveCheck.Orientation)));

	if (!xmOOBBPlayerMoveCheck.Intersects(m_pPlayer->m_xmOOBB)) m_pWallsObject->SetPosition(m_pPlayer->m_xmf3Position);	//이게 원위치인듯
}


void CScene::Animate(float fElapsedTime)
{
	m_pWallsObject->Animate(fElapsedTime);

	for (int i = 0; i < m_nObjects; i++) m_ppObjects[i]->Animate(fElapsedTime);

	//CheckPlayerByWallCollision();

	//CheckObjectByWallCollisions();

	//CheckObjectByObjectCollisions();

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
