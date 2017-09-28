#include "scene_world.h"

SceneWorld::SceneWorld() : m_acceptInput(false)
{
	Init();
	SoundManager::GetInstance();
}

bool SceneWorld::Init()
{
	//Setup viewport to fit the window size
	glViewport(0, 0, (GLsizei)(glutGet(GLUT_WINDOW_WIDTH)), (GLsizei)(glutGet(GLUT_WINDOW_HEIGHT)));

	m_bloomEffect = true;

	m_camera = new Camera((float)glutGet(GLUT_WINDOW_WIDTH), (float)glutGet(GLUT_WINDOW_HEIGHT));
	m_camera->Pos.z = -25;

	m_World = new Transformation();

	BasicEffect::GetInstance();

	m_pause = false;
	m_acceptInput = true;

	m_mapHandler = new MapHandler();

	m_player = new Entity(true);
	m_player->Physics()->SetPosition(Vector3f(0, 2, 4.0f));
	m_player->Physics()->AbsolutePosition(Vector3f(0, 2, 4.0f));
	m_test3 = new Entity();
	m_test3->Physics()->SetPosition(Vector3f(7.f, 6.5f, 3.0f));
	m_test3->Physics()->AbsolutePosition(Vector3f(7.f, 6.5f, 3.0f));
	m_test2 = new Entity();
	m_test2->Physics()->SetPosition(Vector3f(4.f, 0.0f, 4.0f));
	m_test2->Physics()->AbsolutePosition(Vector3f(4.f, 0.0f, 4.0f));

	m_celist = std::vector<Entity*>();
	m_celist.push_back(m_player);
	m_celist.push_back(m_test3);
	m_celist.push_back(m_test2);

	m_font = new Font();
	m_font->SetText("IT WORKS", Vector3f(0, 0, 0));

	return true;
}

//Loads GL resources
void SceneWorld::LoadAllResources()
{
	//for (Drawable* x : *m_objList)
	//	x->LoadGLResources();

	//m_test->LoadGLResources();
	//m_mapHandler->Graphics()->LoadGLResources();

	m_resources_loaded = true;
}

void SceneWorld::UnloadAllResources()
{
	//for (Drawable* x : *m_objList)
	//	x->UnloadExternalResources();

	m_resources_loaded = false;
}

void SceneWorld::ManageInput()
{
	InputManager::GetInstance().SetupFrameKeys();

	if (InputManager::GetInstance().FrameKeyStatus('b', AnyRelease))
		m_bloomEffect = !m_bloomEffect;

	static float bloomint = 0.3f;
	if (InputManager::GetInstance().FrameKeyStatus('u')) {
		bloomint += 0.2f;
		CombineEffect::GetInstance().SetIntensity(bloomint);
	}
	if (InputManager::GetInstance().FrameKeyStatus('j')) {
		bloomint -= 0.2f;
		CombineEffect::GetInstance().SetIntensity(bloomint);
	}
}

Scene* SceneWorld::Act()
{
	ManageInput();

	//RENDER SETUP WITH FRAME BY FRAME
	if (!m_pause || m_numFrames > 0)
	{
		//If we pause to slowly pass frames...
		if (m_numFrames > 0)
		{
			m_numFrames--;
			//...then set the elapsedtime to the desired amount (in fps)
			ElapsedTime::GetInstance().SetBufferElapsedTime(60.f);
		}
		Update();
	}

	//DRAW
	Draw();

	return &SceneWorld::GetInstance();
}

void SceneWorld::Draw()
{
	//ShadowMapPass();
	RenderPass();
}

void SceneWorld::Interact()
{
	if (InputManager::GetInstance().FrameKeyStatus(' ')) {
		Vector3f pos = m_player->Physics()->GetCenter();
		PlayerGraphicsComponent* pgc = static_cast<PlayerGraphicsComponent*>(m_player->Graphics());
		Direction dir = pgc->GetDirection();

		float distance = 0.2f;
		if (dir == dir_Left || dir == dir_Right)
			distance += m_player->Physics()->GetSize().x / 2;
		else if (dir == dir_Up || dir == dir_Down)
			distance += m_player->Physics()->GetSize().y / 2;

		dir == dir_Up ? pos.y += distance : pos.y = pos.y;
		dir == dir_Down ? pos.y -= distance : pos.y = pos.y;
		dir == dir_Right ? pos.x += distance : pos.x = pos.x;
		dir == dir_Left ? pos.x -= distance : pos.x = pos.x;

		Entity* inter = NULL;
		for (auto x : m_celist)
		{
			if (Physics::Intersect2D(x->Physics()->GetBoundingBox(), pos))
			{
				if (x == m_player)
					break;
				inter = x;
				break;
			}
		}

		if (inter != NULL)
		{
			m_player->Communicate(inter->Input()->Interact());
		}
	}
}

void SceneWorld::Update()
{
	Animation::AnimationCounter((float)ElapsedTime::GetInstance().GetElapsedTime());

	for (auto it : m_celist)
		it->Physics()->DesiredMove();

	//Collision
	Physics_2D::Collision(&m_celist, m_mapHandler);

	//Update
	for (auto it : m_celist)
		it->Update();

	m_mapHandler->Update();

	Interact();
	SetAudioPosition();
	//SoundManager::GetInstance().SetListenerOrientation(((PlayerGraphicsComponent*)m_player->Graphics())->GetDirection());

	//std::cout << m_player->GetDirection() << std::endl;
	//std::cout << /*m_player->Position().x << ", " << m_player->Position().y << ", " <<*/ m_player->Physics()->Position().z << std::endl;// << ", " << m_clist.at(1)->GetMoveBoundingBox().Get(AABB::Down) << ", " << m_clist.at(1)->GetMoveBoundingBox().Get(AABB::Close) << std::endl;

	m_World->Follow(m_player->Physics()->Position(), Vector3f(32, 18, 0));
	//m_camera->Update(m_player->Position());//this needs to change LOLOLOLOL

	m_font->SetText(std::string(std::to_string(ElapsedTime::GetInstance().GetFPS())), Vector3f(0, 0, 0));

	Renderer::GetInstance().Empty();
}

void SceneWorld::RenderPass()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static bool drawinited = false;
	if (!drawinited)
	{
		//m_camera->Up = Vector3f(0, 1, 0);
		//m_World->SetCamera(*m_camera);
		m_World->SetOrthoProj(&OrthoProjInfo::GetRegularInstance());
		m_World->SetTranslation(OrthoProjInfo::GetRegularInstance().Left, OrthoProjInfo::GetRegularInstance().Bottom, 0);
		//m_World->SetRotation(m_camAngle, 0.0f, 0.0f);

		BasicEffect::GetInstance().Enable();
		BasicEffect::GetInstance().SetWorldPosition(*m_World->GetWOTrans().m);
		BlurEffect::GetInstance().Enable();
		BlurEffect::GetInstance().SetWorldPosition(*m_World->GetWOTrans().m);
		BloomEffect::GetInstance().Enable();
		BloomEffect::GetInstance().SetWorldPosition(*m_World->GetWOTrans().m);
		BloomEffect::GetInstance().Enable(BloomEffect::GetInstance().GetDark());
		BloomEffect::GetInstance().SetWorldPosition(*m_World->GetWOTrans().m);
		CombineEffect::GetInstance().Enable();
		CombineEffect::GetInstance().SetWorldPosition(*m_World->GetWOTrans().m);
		TransparencyEffect::GetInstance().Enable();
		TransparencyEffect::GetInstance().SetWorldPosition(*m_World->GetWOTrans().m);
		HeightEffect::GetInstance().Enable();
		HeightEffect::GetInstance().SetWorldPosition(*m_World->GetWOTrans().m);

		drawinited = true;
	}

	//m_World->SetTranslation(-m_player->Position().x, -m_player->Position().y, 0);

	//BasicEffect::GetInstance().Enable();
	//Effect::SetWorldPosition(*m_World->GetWOTrans().m);

	BasicEffect::GetInstance().Enable();
	Effect::SetWorldPosition(*m_World->GetWOTrans().m);

	if (!m_bloomEffect)
	{
		//m_mapHandler->Draw();
		m_mapHandler->SetRender();

		for (auto it : m_celist)
			//it->Draw();
			it->SetRender();
		Renderer::GetInstance().Draw();
	}
	else
	{
		m_bloom.Begin();

		//HeightEffect::GetInstance().Enable();
		//HeightEffect::GetInstance().SetPlayerPos(m_player->Physics()->Position());

		m_font->Draw();

		//m_mapHandler->Draw();
		m_mapHandler->SetRender();

		for (auto it : m_celist)
			it->SetRender();
		//it->Draw();
		Renderer::GetInstance().Draw();

		bool darkBloom = true;
		m_bloom.End(darkBloom);

	}

	//Debug tile outline drawing
	if (Globals::DEBUG_DRAW_TILE_OUTLINES)
	{
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		for (int x = 1; x < 32; x++)
		{
			glVertex3f((GLfloat)x, (GLfloat)0, (GLfloat)-8);
			glVertex3f((GLfloat)x, (GLfloat)18, (GLfloat)-8);
		}
		for (int y = 1; y < 18; y++)
		{
			glVertex3f((GLfloat)0, (GLfloat)y, (GLfloat)-8);
			glVertex3f((GLfloat)32, (GLfloat)y, (GLfloat)-8);
		}
		glEnd();
	}

	GLUTBackendSwapBuffers();
}

void SceneWorld::SetAudioPosition()
{
	if (m_player != NULL)
		SoundManager::GetInstance().SetListenerPosition(m_player->Physics()->Position(), m_player->Physics()->Velocity());
	else
		SoundManager::GetInstance().SetListenerPosition();
}