#include"Chunk.hpp"
#include"World.h"

bool Chunk::hasNBlockLeft(int x, int y) {
	int chunk = 0;
	if (x - 1 < 0) {
		x = chunkSizeX;
		chunk = -1;
	}

	return (chunk+m_id >= 0 && Settings::world->getChunk(m_id+chunk)->GetBlock(x - 1,y)->getId() == 0);
}
bool Chunk::hasNBlockRight(int x, int y) {
	int chunk = 0;
	if (x + 1 >= chunkSizeX) {
		x = -1;
		chunk = 1;
	}

	return (chunk + m_id < Settings::worldSize && Settings::world->getChunk(m_id + chunk)->GetBlock(x + 1, y)->getId() == 0);
}


void Chunk::OnChange() {
	m_vertexArray = sf::VertexArray(sf::PrimitiveType::Quads);

	m_currentCollisions.clear();

	for (int y = 0; y < chunkSizeY; y++) {
		for (int x = 0; x < chunkSizeX; x++) {
			if (m_blocks[y][x].getId() == 0) {
				if(x==chunkSizeX-1 && m_id+1<Settings::worldSize)
					Settings::world->getChunk(m_id + 1)->ChangeCol();
				continue;
			}

			//add to vertex aray for drawing
			sf::Vertex vert1, vert2, vert3, vert4;

			//left top corner
			vert1.position = sf::Vector2f(x * Settings::blockSize.x, y * Settings::blockSize.y);
			vert1.texCoords = sf::Vector2f(Settings::textureResolution.x * (m_blocks[y][x].getId() % Settings::atlasSize.x), Settings::textureResolution.y * std::floor(m_blocks[y][x].getId() / Settings::atlasSize.y));
			vert1.color = sf::Color::White;

			//right top corner
			vert2.position = sf::Vector2f((x + 1) * Settings::blockSize.x, y * Settings::blockSize.y);
			vert2.texCoords = sf::Vector2f(Settings::textureResolution.x * (m_blocks[y][x].getId() % Settings::atlasSize.x + 1), Settings::textureResolution.y * (std::floor(m_blocks[y][x].getId() / Settings::atlasSize.y)));
			vert2.color = sf::Color::White;

			//left bottom corner
			vert3.position = sf::Vector2f(x * Settings::blockSize.x, (y + 1) * Settings::blockSize.y);
			vert3.texCoords = sf::Vector2f(Settings::textureResolution.x * (m_blocks[y][x].getId() % Settings::atlasSize.x), Settings::textureResolution.y * (std::floor(m_blocks[y][x].getId() / Settings::atlasSize.y + 1)));
			vert3.color = sf::Color::White;

			//right bottom corner
			vert4.position = sf::Vector2f((x + 1) * Settings::blockSize.x, (y + 1) * Settings::blockSize.y);
			vert4.texCoords = sf::Vector2f(Settings::textureResolution.x * (m_blocks[y][x].getId() % Settings::atlasSize.x + 1), Settings::textureResolution.y * (std::floor(m_blocks[y][x].getId() / Settings::atlasSize.y + 1)));
			vert4.color = sf::Color::White;

			//append to array
			m_vertexArray.append(vert1);
			m_vertexArray.append(vert3);
			m_vertexArray.append(vert4);
			m_vertexArray.append(vert2);

			//creat colliders
			bool wasCollison = false;

			BlockCollision col;
			col.block = &m_blocks[y][x];
			col.up = hasNBlockUp(x, y);
			col.down = hasNBlockDown(x, y);
			col.left = hasNBlockLeft(x, y);
			col.right = hasNBlockRight(x, y);

			if (col.up || col.down || col.left || col.right)
				m_currentCollisions.push_back(col);
		}

	}

	ChangeCol();
}

void Chunk::OnCollisionChange() {
	//destroy old colliders
	for (auto i : m_bodyList) {
		i->GetWorld()->DestroyBody(i);
	}

	m_bodyDefs.clear();
	m_bodyList.clear();
	m_physicShapes.clear();

	//generate edges
	for (auto col : m_currentCollisions) {
		if (col.up) {
			m_bodyDefs.push_back(CreatBodyDef(col.block));
			m_bodyList.push_back(Settings::world->creatBody(*(m_bodyDefs.end() - 1)));
			m_physicShapes.push_back(b2EdgeShape());
			(m_physicShapes.end() - 1)->Set({ 0, 0 }, toGlPosition({Settings::blockSize.x,0}));
			(*(m_bodyList.end() - 1))->CreateFixture(&*(m_physicShapes.end() - 1), 1.f);
			(*(m_bodyList.end() - 1))->SetUserData(new CollisionData{ Settings::GetColId("blockUpEdge"),"Block",col.block });
		}
		if (col.down) {
			m_bodyDefs.push_back(CreatBodyDef(col.block));
			m_bodyList.push_back(Settings::world->creatBody(*(m_bodyDefs.end() - 1)));
			m_physicShapes.push_back(b2EdgeShape());
			(m_physicShapes.end() - 1)->Set(toGlPosition({ 0,Settings::blockSize.y }), toGlPosition({ Settings::blockSize.x,Settings::blockSize.y }));
			(*(m_bodyList.end() - 1))->CreateFixture(&*(m_physicShapes.end() - 1), 1.f);
			(*(m_bodyList.end() - 1))->SetUserData(new CollisionData{ Settings::GetColId("blockDownEdge"),"Block",col.block });
		}
		if (col.left) {
			m_bodyDefs.push_back(CreatBodyDef(col.block));
			m_bodyList.push_back(Settings::world->creatBody(*(m_bodyDefs.end() - 1)));
			m_physicShapes.push_back(b2EdgeShape());
			(m_physicShapes.end() - 1)->Set(toGlPosition({ 0,0 }), toGlPosition({ 0,Settings::blockSize.y }));
			(*(m_bodyList.end() - 1))->CreateFixture(&*(m_physicShapes.end() - 1), 1.f);
			(*(m_bodyList.end() - 1))->SetUserData(new CollisionData{ Settings::GetColId("blockLeftEdge"),"Block",col.block });
		}
		if (col.right) {
			m_bodyDefs.push_back(CreatBodyDef(col.block));
			m_bodyList.push_back(Settings::world->creatBody(*(m_bodyDefs.end() - 1)));
			m_physicShapes.push_back(b2EdgeShape());
			(m_physicShapes.end() - 1)->Set(toGlPosition({ Settings::blockSize.x,0 }), toGlPosition({ Settings::blockSize.x,Settings::blockSize.y }));
			(*(m_bodyList.end() - 1))->CreateFixture(&*(m_physicShapes.end() - 1), 1.f);
			(*(m_bodyList.end() - 1))->SetUserData(new CollisionData{ Settings::GetColId("blockRightEdge"),"Block",col.block });
		}
	}
}