#include "AppClass.h"
using namespace Simplex;

//Benjamin Morgan
//references: https://github.com/hjweide/a-star

void Application::InitVariables(void)
{
	currentLevel = std::string( "0" );
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 5.0f, 10.0f), //Position
		playerLocation,	//Target
		AXIS_Y);					//Up

	//level manager load first level, hard coded for now
	m_pLevelMngr->SetLevel(currentLevel);

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	m_pEntityMngr->AddEntity("Minecraft\\Steve.obj", "Steve");
	m_pEntityMngr->AddEntity( "Minecraft\\Zombie.obj", "Zombie" );

	positions = m_pLevelMngr->GetBlockPositions();

	objectMap = m_pLevelMngr->getObjectMap();

	weights = std::vector<float>(objectMap.size());


	int nHoles = 0;
	
	for( int i = 0; i < objectMap.size(); i++ )
	{
		if( objectMap[i] == 'H' )
		{
			nHoles++;
			weights[i] = INF;
		}
		else
		{
			weights[i] = 1;
		}

		if( objectMap[i] == 'G' )
		{
			goalIndex = i;
		}
		else if( objectMap[i] == 'Z' )
		{
			zombieSpawnIndex = i;
		}
	}
	
	//no clue if this is right every time but hopefully is
	int index = ((int)(m_pLevelMngr->GetLevelWidth() / 2)) * m_pLevelMngr->GetLevelWidth() + (int)m_pLevelMngr->GetLevelHeight() / 2;
	float playerHeight = 2.0f;
	float playerWidth = 0.5f;
	steveID = m_pEntityMngr->GetEntityIndex("Steve");
	steveID = m_pEntityMngr->GetEntityIndex( "Zombie" );
	//puts player at middle of the map height offset by its dimensions

	int spawnIndex = m_pLevelMngr->GetSpawnIndex();
	playerIndex = m_pLevelMngr->GetSpawnIndex();

	//v3Position.x = -levelWidth / 2 + (float)(index % levelWidth);
	//v3Position.z = -levelHeight / 2 + (float)(index / levelWidth);

	spawnLocation = vector3();
	spawnLocation.x =-m_pLevelMngr->GetLevelWidth()/2 + (spawnIndex % m_pLevelMngr->GetLevelWidth()) + playerWidth;
	spawnLocation.y = positions[spawnIndex].y + (0.5f*playerHeight);
	spawnLocation.z = -m_pLevelMngr->GetLevelHeight()/2 + (spawnIndex / m_pLevelMngr->GetLevelWidth()) + playerWidth;
	playerLocation = spawnLocation;

	goalPos = vector3();
	goalPos.x = -m_pLevelMngr->GetLevelWidth() / 2 + (goalIndex % m_pLevelMngr->GetLevelWidth()) + playerWidth;
	goalPos.y = positions[goalIndex].y + (0.5f*playerHeight);
	goalPos.z = -m_pLevelMngr->GetLevelHeight() / 2 + (goalIndex / m_pLevelMngr->GetLevelWidth()) + playerWidth;
	
	zombiePosition = vector3();
	zombiePosition.x = -m_pLevelMngr->GetLevelWidth() / 2 + (zombieSpawnIndex % m_pLevelMngr->GetLevelWidth()) + playerWidth;
	zombiePosition.y = positions[zombieSpawnIndex].y + (0.5f*playerHeight);
	zombiePosition.z = -m_pLevelMngr->GetLevelHeight() / 2 + (zombieSpawnIndex / m_pLevelMngr->GetLevelWidth()) + playerWidth;

	//m_pMeshMngr
	m_pEntityMngr->SetModelMatrix(glm::translate(spawnLocation.x,spawnLocation.y,spawnLocation.z) * glm::rotate(IDENTITY_M4, 180.0f, AXIS_Y), "Steve");
	m_pEntityMngr->SetModelMatrix( glm::translate( zombiePosition.x, zombiePosition.y, zombiePosition.z ) * glm::rotate( IDENTITY_M4, 0.0f, AXIS_Y ), "Zombie" );
	
	for (int i = 0; i < positions.size() - nHoles; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube_" + std::to_string(i));
		
	}
	int adjustedIndex = 0;

	for( int index = 0; index < positions.size(); index++ )
	{
		if( objectMap[index] == 'H' )
		{
			
		}
		else
		{
			m_pEntityMngr->SetModelMatrix( glm::translate( positions[index] ), "Cube_" + std::to_string( adjustedIndex ) );
			adjustedIndex++;
		}
	}
	// "allocating space"
	for( int index = 0; index < positions.size(); index++ )
	{
		paths.push_back( -1 );
	}

	if( !astar( weights, m_pLevelMngr->GetLevelHeight(), m_pLevelMngr->GetLevelWidth(), zombieSpawnIndex, spawnIndex, paths ) )
	{
		printf( "Astar couldn't find a path" );
		//failure variable here

	}
	int currentIndex = 1;
	//using player spawn index as the goal here 

	finalPath.push_back( spawnIndex );
	currentIndex = spawnIndex;
	while( true )
	{
		if( paths[currentIndex] != -1 )
		{
			finalPath.push_back( paths[currentIndex] );
			currentIndex = paths[currentIndex];
		}
		else
		{
			break;
		}
	}

	std::reverse( finalPath.begin(), finalPath.end() );

	zombiePathIndex = 0;

	for( int index = 0; index < finalPath.size(); index++ )
	{
		printf( "\n%d", finalPath[index] );
	}

}
void Application::Recalculate()
{

	paths.clear();

	for( int index = 0; index < positions.size(); index++ )
	{
		paths.push_back( -1 );
	}

	if( !astar( weights, m_pLevelMngr->GetLevelHeight(), m_pLevelMngr->GetLevelWidth(), finalPath[zombiePathIndex], playerIndex, paths ) )
	{
		printf( "Astar couldn't find a path" );
		//failure variable here

	}

	finalPath.clear();
	int currentIndex = 1;
	//using player spawn index as the goal here 

	finalPath.push_back( playerIndex );
	currentIndex = playerIndex;
	while( true )
	{
		if( paths[currentIndex] != -1 )
		{
			finalPath.push_back( paths[currentIndex] );
			currentIndex = paths[currentIndex];
		}
		else
		{
			break;
		}
	}

	std::reverse( finalPath.begin(), finalPath.end() );

	zombiePathIndex = 0;

	for( int index = 0; index < finalPath.size(); index++ )
	{
		printf( "\n%d", finalPath[index] );
	}
}

bool Application::astar(
	const std::vector<float> weights, const int height, const int width,
	const int start, const int goal,
	std::vector<int> &paths )
{

	const float INF = std::numeric_limits<float>::infinity();

	Node start_node( start, 0. );
	Node goal_node( goal, 0. );

	float* costs = new float[height * width];
	for( int i = 0; i < height * width; ++i )
		costs[i] = INF;
	costs[start] = 0.;

	std::priority_queue<Node> nodes_to_visit;
	nodes_to_visit.push( start_node );

	int* neighbors = new int[4];

	bool solution_found = false;
	while( !nodes_to_visit.empty() )
	{
		// .top() doesn't actually remove the node
		Node cur = nodes_to_visit.top();

		if( cur == goal_node )
		{
			solution_found = true;
			break;
		}

		nodes_to_visit.pop();

		// check bounds and find up to four neighbors
		neighbors[0] = (cur.idx / width > 0) ? (cur.idx - width) : -1;
		neighbors[1] = (cur.idx % width > 0) ? (cur.idx - 1) : -1;
		neighbors[2] = (cur.idx / width + 1 < height) ? (cur.idx + width) : -1;
		neighbors[3] = (cur.idx % width + 1 < width) ? (cur.idx + 1) : -1;

		for( int i = 0; i < 4; ++i )
		{
			if( neighbors[i] >= 0 )
			{
				// the sum of the cost so far and the cost of this move
				float new_cost = costs[cur.idx] + weights[neighbors[i]];
				if( new_cost < costs[neighbors[i]] )
				{
					costs[neighbors[i]] = new_cost;
					float priority = new_cost + Node::heuristic( neighbors[i] / width,
						neighbors[i] % width,
						goal / width,
						goal % width );
					// paths with lower expected cost are explored first
					nodes_to_visit.push( Node( neighbors[i], priority ) );
					paths[neighbors[i]] = (cur.idx);
				}
			}
		}
	}

	delete[] costs;
	delete[] neighbors;

	return solution_found;
}

void Application::Update(void)
{
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer = m_pSystem->GetDeltaTime( uClock ); //get the delta time for that timer

	std::vector<vector3> positions = m_pLevelMngr->GetBlockPositions();

	int levelHeight = m_pLevelMngr->GetLevelHeight();
	int levelWidth = m_pLevelMngr->GetLevelWidth();

	//MeshManager::GetInstance()->AddWireCubeToRenderList( glm::translate( IDENTITY_M4, origin ) *
	//	glm::scale( vector3( halfDimension.x * 2, halfDimension.y * 2, halfDimension.z * 2 ) ), vector3( 50, 0, 255 ), RENDER_WIRE );

	if(keyPressedLastFrame == false )
	{
		//where is keyreleased?
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Up ) )
		{
			if( playerIndex / levelWidth > 0 && objectMap[playerIndex - levelWidth] != 'H' )
			{ 
				playerIndex = (playerIndex - levelWidth); 
				playerLocation.z -= 1.0f;
				keyPressedLastFrame = true;
				Recalculate();
			}
		
		}
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Down ) )
		{
			//is height usage correct?
			if( playerIndex / levelWidth + 1 < levelHeight && objectMap[playerIndex + levelWidth] != 'H' )
			{
				playerIndex = playerIndex + levelWidth;
				playerLocation.z += 1.0f;
				keyPressedLastFrame = true;
				Recalculate();
			}

		}
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Left ) )
		{
			if( playerIndex % levelWidth > 0 && objectMap[playerIndex - 1] != 'H' )
			{
				playerIndex=playerIndex - 1;
				playerLocation.x -= 1.0f;
				keyPressedLastFrame = true;
				Recalculate();
			}
		}
		if( sf::Keyboard::isKeyPressed( sf::Keyboard::Right ) )
		{
			if( playerIndex % levelWidth + 1 < levelWidth && objectMap[playerIndex+1] != 'H' )
			{
				playerIndex=playerIndex + 1;
				playerLocation.x += 1.0f;
				keyPressedLastFrame = true;
				Recalculate();
			}
		}
	}
	else
	{
		keyPressedLastFrame = false;
	}

	
	//Is not lined up correctly by half a unit
	vector3 positionAdjusted = positions[finalPath[zombiePathIndex]];
	positionAdjusted.x += 0.5;
	positionAdjusted.z += 0.5;


	//do some distance^2 stuff to increase performance
	if( glm::distance( vector2( zombiePosition.x, zombiePosition.z ), 
		vector2( positionAdjusted.x,
		positionAdjusted.z ) ) < .2f )
	{
		if( zombiePathIndex < finalPath.size() - 1 )
		{ 
			zombiePathIndex++;
		}

	}
	float speed = 4.0f;
	zombiePosition.x = glm::lerp( zombiePosition.x, positionAdjusted.x, fTimer * speed );
	zombiePosition.z = glm::lerp( zombiePosition.z, positionAdjusted.z, fTimer * speed );
	m_pEntityMngr->GetEntity( zombieID )->SetPosition( zombiePosition ); //thought this would affect the model matrix..
	m_pEntityMngr->SetModelMatrix( glm::translate( zombiePosition.x, zombiePosition.y, zombiePosition.z ) 
		* glm::rotate( IDENTITY_M4, 0.0f, AXIS_Y ), "Zombie" );


	m_pEntityMngr->SetModelMatrix( glm::translate( playerLocation.x, playerLocation.y, playerLocation.z )
		* glm::rotate( IDENTITY_M4, 180.0f, AXIS_Y ), "Steve" );

	if (m_pEntityMngr->GetEntity(steveID)->GetPosition().y < -10.0f)
	{
		//reset
		m_pEntityMngr->GetEntity(steveID)->SetPosition(spawnLocation);

		m_pEntityMngr->GetEntity(steveID)->SetVelocity(vector3(0.0f,0.0f,0.0f));
		//reset level rotation too
	}

	//Add colored planes above cubes
	for (int i = 0; i < positions.size(); i++)
	{
		vector3 color = vector3( 255.0f, 0.0f, 0.0f );

		if( std::find( finalPath.begin(), finalPath.end(),i) != finalPath.end() )
		{
			color = vector3( 0.0f, 255.0f, 0.0f );
		}


		if( objectMap[i] == 'H' )
		{
			//do nothing
		}
		else
		{
			vector3 temp = positions[i];
			temp.y += 1.05f;
			temp.x += 0.5f;
			temp.z += 0.5f;

			m_pMeshMngr->AddPlaneToRenderList( 
				glm::translate( temp) * 
				glm::rotate( IDENTITY_M4, 270.0f, AXIS_X ) * 
				glm::scale( .9f, .9f, .9f ), 
				color, RENDER_SOLID );

		}
	}

	m_pEntityMngr->SetModelMatrix(m_pEntityMngr->GetModelMatrix("Ball_" + std::to_string(0)), "Ball_" + std::to_string(0));
	
	//camera follow
	m_v3CharPos = m_pEntityMngr->GetRigidBody("Steve")->GetCenterGlobal(); //get character's position vector
	m_v3CamPos = m_pEntityMngr->GetRigidBody("Steve")->GetCameraFollow(); //get character's position vector

	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager 
	m_pEntityMngr->Update();

	//Set the model matrix for the main object
	//m_pEntityMngr->SetModelMatrix(m_m4Steve, "Steve");

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	//m_pEntityMngr->AddEntityToRenderList(-1, true);


}
void Application::Display(void)
{
	
	// Clear the screen
	ClearScreen();


	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();


	//release GUI
	ShutdownGUI();
}