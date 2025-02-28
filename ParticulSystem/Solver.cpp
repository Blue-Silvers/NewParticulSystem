#include "Solver.h"

void Solver::ApplyGravity()
{
    for (Particle* obj : objects)
    {
        obj->Accelerate(gravity);
    }
}

void Solver::ApplyBorder()
{
    for (Particle* obj : objects)
    {
        const float dampening = 0.75f;
        const sf::Vector2f position = obj->position;
        sf::Vector2f newPosition = obj->position;
        sf::Vector2f velocity = obj->GetVelocity();
        sf::Vector2f dx = { -velocity.x, velocity.y * dampening };
        sf::Vector2f dy = { velocity.x * dampening, -velocity.y };
        // Bounce off left/right
        if (position.x < obj->radius || position.x + obj->radius > windowSize.x) 
        { 
            if (position.x < obj->radius)
            {
                newPosition.x = obj->radius;
            }
            if (position.x + obj->radius > windowSize.x)
            {
                newPosition.x = windowSize.x - obj->radius;
            }
            obj->position = newPosition;
            obj->SetVelocity(dx, 1.0);
        }
        // Bounce off top/bottom
        if (position.y < obj->radius || position.y + obj->radius > windowSize.y) 
        { 
            if (position.y < obj->radius)
            {
                newPosition.y = obj->radius;
            }
            if (position.y + obj->radius > windowSize.y)
            {
                newPosition.y = windowSize.y - obj->radius;
            }
            obj->position = newPosition;
            obj->SetVelocity(dy, 1.0);
        }
    }
}

void Solver::ApplyBoundary()
{
    for (Particle* obj : objects)
    {
        const sf::Vector2f r = boundaryCenter - obj->position;
        const float dist = sqrt(r.x * r.x + r.y * r.y);
        if (dist > boundaryRadius - obj->radius)
        {
            const sf::Vector2f normalize = r / dist;
            const sf::Vector2f perp = { -normalize.y, normalize.x };
            const sf::Vector2f velocity = obj->GetVelocity();
            obj->position = boundaryCenter - normalize * (boundaryRadius - obj->radius);
            obj->SetVelocity(2.0f * (velocity.x * perp.x + velocity.y * perp.y) * perp - velocity, 1.0f);
        }
    }
}

void Solver::CheckCollisions()
{
    //Optimizer V1
    /*int nbCells = windowSize.x / (float)gridSize;
    int dx[] = { 1, 1, 0, 0, -1 };
    int dy[] = { 0, 1, 0, 1, 1 };
    for (int i = 0; i < nbCells; i++)
    {
        if (!grid[i].size())
        {
            continue;
        }
        for (int k = 0; k < 5; k++)
         {
            int nx = i + dx[k];
            if (nx < 0 || nx >= nbCells)
            {
                continue;
            }
            CollideCells(i, 0, nx, 0);
        }
    }*/

    //Optimizer V2
    /*int nbCells = windowSize.x / (float)gridSize;
    int dx[] = { 1, 1, 0, 0, -1 };
    int dy[] = { 0, 1, 0, 1, 1 };
    for (int i = 0; i < nbCells; i++) 
    {
        for (int j = 0; j < nbCells; j++) 
        {
            if (!grid[i][j].size()) 
            {
                continue;
            }
            for (int k = 0; k < 5; k++) 
            {
                int nx = i + dx[k], ny = j + dy[k];
                if (nx < 0 || ny < 0 || nx >= nbCells || ny >= nbCells) 
                {
                    continue;
                }
                CollideCells(i, j, nx, ny);
            }
        }
    }*/


    //No opti
    int nbObjects = objects.size();
    for (int i = 0; i < nbObjects; i++)
    {
        Particle* objFirst = objects[i];
        for (int j = 0; j < nbObjects; j++)
        {
            if (i == j)
            {
                continue;
            }
            Particle* objSecond = objects[j];
            sf::Vector2f v = objFirst->position - objSecond->position;
            float dist = sqrt(v.x * v.x + v.y * v.y);
            float minDist = objFirst->radius + objSecond->radius;
            if (dist < minDist)
            {
                sf::Vector2f n = v / dist; //Normalize distance
                float totalMass = objFirst->radius * objFirst->radius + objSecond->radius * objSecond->radius;
                float massRatio = (objFirst->radius * objFirst->radius) / totalMass;
                float delta = 0.5f * (minDist - dist);
                //larger particle moves less
                objFirst->position += n * (1 - massRatio) * delta;
                objSecond->position -= n * massRatio * delta;
            }
        }
    }
}

void Solver::CollideCells(int x1, int y1, int x2, int y2)
{
    //Optimizer V1
    for (int idFirst : grid[x1])
    {
        Particle* objFirst = objects[idFirst];

        for (int idSecond : grid[x2])
        {
            if (idFirst == idSecond)
            {
                continue;
            }

            Particle* objSecond = objects[idSecond];
            sf::Vector2f velocity = objFirst->position - objSecond->position;
            float dist = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            float minDist = objFirst->radius + objSecond->radius;

            if (dist < minDist) {
                sf::Vector2f normalize = velocity / dist; // Normalize
                float delta = 0.5f * (minDist - dist);
                // Larger particle moves less
                objFirst->position += normalize * 0.5f * delta;
                objSecond->position -= normalize * 0.5f * delta;
            }
        }
    }

    //Optimizer V2
    /*for (int idFirst : grid[x1][y1])
    {
        Particle*& objFirst = objects[idFirst];

        for (int idSecond : grid[x2][y2]) 
        {
            if (idFirst == idSecond) 
            {
                continue;
            }

            Particle*& objSecond = objects[idSecond];
            sf::Vector2f velocity = objFirst->position - objSecond->position;
            float dist = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            float minDist = objFirst->radius + objSecond->radius;

            if (dist < minDist) {
                sf::Vector2f normalize = velocity / dist; // Normalize
                float delta = 0.5f * (minDist - dist);
                // Larger particle moves less
                objFirst->position += normalize * 0.5f * delta;
                objSecond->position -= normalize * 0.5f * delta;
            }
        }
    }*/
}

void Solver::UpdateObject(float dt)
{
    for (Particle* obj : objects)
    {
        obj->Update(dt);

        int currentGridX = obj->gridX, currentGridY = obj->gridY;
        obj->gridX = obj->position.x / gridSize;
        obj->gridY = obj->position.y / gridSize;

        //Optimizer V1
        /*if (currentGridX != obj->gridX || currentGridY != obj->gridY)
        {
            //auto pos = find(grid[currentGridX].begin(), grid[currentGridX].end(), obj->id);
            //grid[currentGridX].erase(pos);
            grid[obj->gridX].push_back(obj->id);
        }*/
        //Optimizer V2
        /*if (currentGridX != obj->gridX || currentGridY != obj->gridY) 
        {
            auto pos = find(grid[currentGridX][currentGridY].begin(), grid[currentGridX][currentGridY].end(), obj->id);
            grid[currentGridX][currentGridY].erase(pos);
            grid[obj->gridX][obj->gridY].push_back(obj->id);
        }*/
    }
}

Particle& Solver::addObject(sf::Vector2f pPosition, float pRadius)
{
    //Particle* newParticle = new Particle(pPosition, pRadius);
    //objects.emplace_back(newParticle);
    int gridx = pPosition.x / gridSize, gridy = pPosition.x / gridSize;
    Particle* newParticle = new Particle(pPosition, pRadius, gridx, gridy, objects.size());
    grid[gridx].push_back(newParticle->id);

    return *objects.emplace_back(newParticle);
}

void Solver::MousePull(sf::Vector2f pPosition)
{
    for (Particle*& obj : objects) {
        sf::Vector2f direction = pPosition - obj->position;
        float dist = sqrt(direction.x * direction.x + direction.y * direction.y);
        obj->Accelerate(direction * std::max(0.0f, 5 * (200/*radius*/ - dist)));
    }
}

void Solver::MousePush(sf::Vector2f pPosition)
{
    for (Particle*& obj : objects) {
        sf::Vector2f direction = pPosition - obj->position;
        float dist = sqrt(direction.x * direction.x + direction.y * direction.y);
        obj->Accelerate(direction * std::min(0.0f, -5 * (200/*radius*/ - dist)));
    }
}

void Solver::SetWindowSize(sf::Vector2f pWindowSize)
{
    windowSize = pWindowSize;
}

void Solver::Update()
{
    float subStepDt = stepDt / subSteps;
    for (int i = 0; i < subSteps; i++) 
    {
        ApplyGravity();
        //ApplyBoundary();
        ApplyBorder();
        CheckCollisions();
        UpdateObject(subStepDt);
        UpdateGrid();
    }
}

std::vector<Particle*> Solver::GetObjects()
{
    return objects;
}

void Solver::UpdateGrid()
{
    int num_cells_width = windowSize.x / gridSize;
    int num_cells_height = windowSize.y / gridSize;
    for (int i = 0; i < num_cells_width; i++)
    {
        grid[i].clear();
    }
    for (Particle* obj : objects) 
    {
        if (obj->gridX < 0 || obj->gridX >= num_cells_width) continue;
        grid[obj->gridX].push_back(obj->id);
    }
}

void Solver::SetBoundary(sf::Vector2f pPosition, float pRadius)
{
    boundaryCenter = pPosition;
    boundaryRadius = pRadius;
}

sf::Vector3f Solver::GetBoundary() const
{
    return { boundaryCenter.x, boundaryCenter.y, boundaryRadius };
}

float Solver::GetStepDt()
{
    return stepDt;
}

void Solver::SetObjectVelocity(Particle& pObject, sf::Vector2f pVelocity)
{
    pObject.SetVelocity(pVelocity, GetStepDt());
}
