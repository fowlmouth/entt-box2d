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

  auto entity1 = registry.create();
  auto& world = registry.assign< Physics::World >(entity1);
  world.world.SetGravity(b2Vec2(0,10));

  auto entity2 = registry.create();
  auto& body = registry.assign< Physics::Body >(entity2);
  {
  	auto& fixture = registry.assign< Physics::Fixture >(entity2);

  	b2BodyDef body_def;
  	body_def.type = b2_dynamicBody;
  	body_def.position = b2Vec2(0,0);

  	b2CircleShape circle_shape;
  	circle_shape.m_radius = 1.0;

  	b2FixtureDef fixture_def;
  	fixture_def.shape = &circle_shape;

  	body.body = world.world.CreateBody(&body_def);
  	fixture.fixture = body.body->CreateFixture(&fixture_def);

  }

  for(int i = 0; i < 10; ++i)
  {
  	world.step(1);
  	auto position = body.body->GetPosition();
  	std::cout << "(" << position.x << ", " << position.y << ")" << std::endl;
  }

  return 0;
}
