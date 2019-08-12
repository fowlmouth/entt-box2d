
#include "entt-mruby/entt-mruby.h"
#include "entt-box2d/entt-box2d.h"
#include <iostream>


struct TestRegistry
: entt::registry,
  MRuby::RegistryMixin< TestRegistry >,
  Physics::RegistryMixin< TestRegistry >
{
  static const int max_static_components;
  int next_dynamic_component_id;

  mrb_state* mrb;

  TestRegistry()
  {
    physics_init();

    mrb = mrb_open();
    mrb_init< Physics::World, Physics::Body, Physics::Fixture >(mrb);
  }

};

const int TestRegistry::max_static_components = 8;

int main()
{
  TestRegistry registry;

  mrb_load_string(registry.mrb, R"MRUBY(
    world = $registry.create_entity
    world.set 'Physics::World', { gravity: [0, 10] }
    @world_id = world.id

    ground_entity = $registry.create_entity
    ground_entity.set 'Physics::Body', { world: world.id, type: 'static' }
    ground_entity.set 'Physics::Fixture', {
      body: ground_entity.id,
      shape: 'chain',
      points: [ [0, 10], [20, 10] ]
    }

    entity2 = $registry.create_entity
    entity2.set 'Physics::Body', { world: world.id, type: 'dynamic', position: [0,0] }
    entity2.set 'Physics::Fixture', { body: entity2.id, shape: 'circle', radius: 1.0 }
    @entity2_id = entity2.id
  )MRUBY");

  auto entity1 = static_cast< entt::entity >(
    mrb_fixnum(
      mrb_iv_get(
        registry.mrb,
        mrb_top_self(registry.mrb),
        mrb_intern_lit(registry.mrb, "@world_id")
      )
    )
  );
  auto& world = registry.get< Physics::World >(entity1);

  auto entity2 = static_cast< entt::entity >(
    mrb_fixnum(
      mrb_iv_get(
        registry.mrb,
        mrb_top_self(registry.mrb),
        mrb_intern_lit(registry.mrb, "@entity2_id")
      )
    )
  );
  auto& body = registry.get< Physics::Body >(entity2);

  for(int i = 0; i < 10; ++i)
  {
    world.step(1);
    auto position = body.body->GetPosition();
    std::cout << "(" << position.x << ", " << position.y << ")" << std::endl;
  }

  return 0;
}
