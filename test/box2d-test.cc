#include "entt-box2d/entt-box2d.h"
#include <iostream>

struct TestRegistry
: entt::registry,
  Physics::RegistryMixin< TestRegistry >
{
  TestRegistry()
  {
    physics_init();
  }

};

int main()
{
  TestRegistry registry;

  auto& world = registry.physics_world();
  world.world.SetGravity(b2Vec2(0,10));

  auto entity = registry.create();
  auto& body = registry.emplace< Physics::Body >(entity);
  {
    b2BodyDef body_def;
    body_def.type = b2_dynamicBody;
    body_def.position = b2Vec2(0,0);

    body.body = world.world.CreateBody(&body_def);
  }

  {
    b2CircleShape circle_shape;
    circle_shape.m_radius = 1.0;

    b2FixtureDef fixture_def;
    fixture_def.shape = &circle_shape;
    fixture_def.density = 1.0;
    

    registry.emplace< Physics::Fixture >(entity, body.body->CreateFixture(&fixture_def));
  }

  for(int i = 0; i < 10; ++i)
  {
    // world.step(100);
    body.body->GetWorld()->Step(1.0, 6,2);
    auto position = body.body->GetPosition();
    std::cout << "(" << position.x << ", " << position.y << ")" << std::endl;
  }

  return 0;
}
