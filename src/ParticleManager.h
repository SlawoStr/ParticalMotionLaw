#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class ParticleManager
{
	struct Particle
	{
		Particle(sf::Vector2f position, float angle)
		{
			this->position = position;
			this->angle = angle;
		}
		sf::Vector2f position;
		sf::Vector2f nextPosition{ 0.0f,0.0f };
		float		 angle;
		int			 neighbors{ 0 };
		int			 closeNeighbors{ 0 };
	};
public:
	ParticleManager(float particleSpeed = 0.0f, float alpha = 0.0f, float beta = 0.0f, float reactRadius = 0.0f, sf::Vector2f simulationBound = sf::Vector2f{ 0.0f,0.0f }, int threadNumber = 1);

	void draw(sf::RenderWindow& window);

	void spawnCells(sf::Vector2f position, int cellNumber);

	void update();

	void reduceSimulationBound();

	void increaseSimulationBound();

	sf::Vector2f getSimulationBound() { return m_simulationBound; }
private:
	std::vector<Particle>	m_particles;
	float					m_particleSpeed;
	float					m_alpha;
	float					m_beta;
	float					m_reactRadius;
	sf::Vector2f			m_simulationBound;
	int						m_threadNumber;
};