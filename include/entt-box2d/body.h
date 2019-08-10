#pragma once

namespace Physics
{

struct Body
{
  b2Body* body = nullptr;
};

bool set_position(entt::registry& r, entt::entity entity, Body& body, b2Vec2 position)
{
  if(body.body)
  {
    body.body->SetTransform(position, body.body->GetAngle());
    return true;
  }
  return false;
}

bool set_position(entt::registry& r, entt::entity entity, b2Vec2 position)
{
  if(Body* body = r.try_get< Body >(entity))
    return set_position(r, entity, *body, position);
  return false;
}

bool get_position(entt::registry& r, entt::entity entity, b2Vec2& position)
{
  if(Body* body = r.try_get< Body >(entity))
  {
    position = body->body->GetPosition();
    return true;
  }
  return false;
}

bool get_transform(entt::registry& r, entt::entity entity, b2Transform& transform)
{
  if(Body* body = r.try_get< Body >(entity))
  {
    transform = body->body->GetTransform();
    return true;
  }
  return false;
}

bool set_linear_velocity(entt::registry& r, entt::entity entity, b2Vec2 velocity)
{
  if(auto body = r.try_get< Body >(entity))
  {
    body->body->SetLinearVelocity(velocity);
    return true;
  }
  return false;
}


void destroy_body(entt::registry& r, entt::entity id)
{
  Body& body = r.get< Body >(id);
  body.body->GetWorld()->DestroyBody( body.body );
  body.body = nullptr;
}


} // ::Physics



#ifdef FOWL_ENTT_MRUBY

template<>
struct MRuby::ComponentInterface< Physics::Body >
: MRuby::DefaultComponentInterface< Physics::Body >
{
  static mrb_value get(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type)
  {
    if(auto world = registry.try_get< Physics::Body >(entity))
    {
      MRuby::HashBuilder hash(state);

      // auto& sf_sprite = sprite->sprite;
      return hash.self;
    }
    return mrb_nil_value();
  }

  static mrb_value set(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type, mrb_int argc, mrb_value* arg)
  {
    if(!argc || ! mrb_hash_p(arg[0]))
      return mrb_nil_value();

    b2BodyDef body_def;
    std::string body_type;
    entt::entity world_id;

    MRuby::HashReader reader(state, arg[0]);
    reader
      ("world", world_id)
      ("type", body_type)
      ("position", body_def.position)
      ("linear-damping", body_def.linearDamping)
      ("angular-damping", body_def.angularDamping)
    ;

    if(!registry.valid(world_id) || !registry.has< Physics::World >(world_id))
      return mrb_nil_value();

    b2BodyType b2_type = b2_dynamicBody;
    if(body_type == "static")
      b2_type = b2_staticBody;
    else if(body_type == "kinematic")
      b2_type = b2_kinematicBody;

    body_def.type = b2_type;

    Physics::World& world = registry.get< Physics::World >(world_id);
    b2Body* b2_body = world.world.CreateBody(&body_def);
    b2_body->SetUserData(reinterpret_cast< void* >(entity));

    auto& body = registry.assign_or_replace< Physics::Body >(entity, b2_body);

    return arg[0];
  }
};

#endif

