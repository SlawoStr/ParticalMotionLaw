#include "ParticleManager.h"
#include "RNGGenerator.h"
#include <iostream>
#define PI 3.14159265

/// <summary>
/// Calculate signum
/// </summary>
/// <typeparam name="T">Precision</typeparam>
/// <param name="val">Value</param>
/// <returns>Signum result</returns>
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

/// <summary>
/// Convert radians to degrees
/// </summary>
/// <param name="a"></param>
/// <returns></returns>
inline float degree(float a)
{
	return static_cast<float>(a * (180 / PI));
}

/// <summary>
/// Convert degree to radians
/// </summary>
/// <param name="a"></param>
/// <returns></returns>
inline float radians(float a)
{
	return static_cast<float>(0.017453292 * a);
}

/// <summary>
/// Get distance between two points
/// </summary>
/// <param name="fp">First point</param>
/// <param name="sp">Second point</param>
/// <returns>Distance between points</returns>
float getDistance(sf::Vector2f fp, sf::Vector2f sp)
{
	return static_cast<float>(sqrt(pow(sp.x - fp.x, 2) + pow(sp.y - fp.y, 2)));
}

/// <summary>
/// Check if point is on the right side of line
/// </summary>
/// <param name="a">First point of line</param>
/// <param name="b">Second point of line</param>
/// <param name="c">Point</param>
/// <returns>Is point on the right side</returns>
bool isOnRight(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c)
{
	return((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) > 0;
}

////////////////////////////////////////////////////////////
ParticleManager::ParticleManager(float particleSpeed, float alpha, float beta, float reactRadius, sf::Vector2f simulationBound,int threadNumber) 
	: m_particleSpeed{ particleSpeed }, m_alpha{ radians(alpha) }, m_beta{ radians(beta) }, m_reactRadius{ reactRadius }, m_simulationBound{simulationBound},m_threadNumber{threadNumber}
{}

////////////////////////////////////////////////////////////
void ParticleManager::draw(sf::RenderWindow& window)
{
	sf::CircleShape shape;
	shape.setRadius(1.0f);
	shape.setOrigin(shape.getRadius(), shape.getRadius());

	sf::Vertex line[2];
	line[0].color = sf::Color::Red;
	line[1].color = sf::Color::Red;

	for (const auto& particle : m_particles)
	{
		shape.setFillColor(sf::Color::Green);
		int n = particle.neighbors;
		if (n > 35)
		{
			shape.setFillColor(sf::Color::Yellow);
		}
		else if (n > 15 && n <= 35)
		{
			shape.setFillColor(sf::Color::Blue);
		}
		else if (n >= 13 && n <= 15)
		{
			shape.setFillColor(sf::Color(136, 69, 19));
		}
		if (particle.closeNeighbors > 15)
		{
			shape.setFillColor(sf::Color::Magenta);
		}
		sf::Vector2f particlePostion = particle.position;
		shape.setPosition(particlePostion);
		line[0].position = particlePostion;
		line[1].position.x = particlePostion.x + cos(particle.angle);
		line[1].position.y = particlePostion.y + sin(particle.angle);
		window.draw(shape);
		window.draw(line, 2, sf::Lines);
	}
}

////////////////////////////////////////////////////////////
void ParticleManager::spawnCells(sf::Vector2f position, int cellNumber)
{
	for (int i = 0; i < cellNumber; ++i)
	{
		m_particles.emplace_back(position, RNGGenerator::randFloat(0.0f, 6.2831f));
	}
}

////////////////////////////////////////////////////////////
void ParticleManager::update()
{
	#pragma omp parallel for num_threads(m_threadNumber)
	for (int i = 0; i < m_particles.size(); ++i)
	{
		int Nt{};
		int Rt{};
		int Lt{};	
		int Ct{};
		// Angle line coordinates
		sf::Vector2f posStart{ m_particles[i].position };
		sf::Vector2f posEnd{ m_particles[i].position.x + cos(m_particles[i].angle),m_particles[i].position.y + sin(m_particles[i].angle) };
		for (int j = 0; j < m_particles.size(); ++j)
		{
			if (i == j)
				continue;
			// If particle is in reaction radius
			float distance{ getDistance(m_particles[i].position, m_particles[j].position) };
			if (distance < m_reactRadius)
			{
				Nt++;
				if (isOnRight(posStart, posEnd, m_particles[j].position))
				{
					Rt++;
				}
				else
				{
					Nt++;
				}
				if (distance < 1.3f)
				{
					Ct++;
				}
			}
		}
		// Update particle data
		float rotationAngle = m_alpha + m_beta * Nt * sgn(Rt - Lt);
		m_particles[i].neighbors = Nt;
		m_particles[i].closeNeighbors = Ct;
		m_particles[i].angle += rotationAngle;
		m_particles[i].nextPosition = sf::Vector2f{ posStart.x + m_particleSpeed * cos(m_particles[i].angle),posStart.y + m_particleSpeed * sin(m_particles[i].angle) };
		if (m_particles[i].nextPosition.x < 0.0f || m_particles[i].nextPosition.x > m_simulationBound.x || m_particles[i].nextPosition.y < 0.0f || m_particles[i].nextPosition.y > m_simulationBound.y)
		{
			m_particles[i].nextPosition = m_particles[i].position;
		}
	}
	// Update particle positions
	for (int i = 0; i < m_particles.size(); ++i)
	{
		m_particles[i].position = m_particles[i].nextPosition;
	}
}

////////////////////////////////////////////////////////////
void ParticleManager::reduceSimulationBound()
{
	m_simulationBound.x -= 1;
	m_simulationBound.y -= 1;

	for (int i = 0; i < m_particles.size(); ++i)
	{
		if (m_particles[i].position.x >= m_simulationBound.x)
		{
			m_particles[i].position.x -= 1;
		}
		if (m_particles[i].position.y >= m_simulationBound.y)
		{
			m_particles[i].position.y -= 1;
		}
	}
}

////////////////////////////////////////////////////////////
void ParticleManager::increaseSimulationBound()
{
	m_simulationBound.x += 1;
	m_simulationBound.y += 1;
}
