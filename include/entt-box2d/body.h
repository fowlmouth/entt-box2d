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

MRUBY_COMPONENT_INTERFACE_BEGIN(Physics::Body)


  MRUBY_COMPONENT_GET
  {
    if(auto body = registry.try_get< Physics::Body >(entity))
    {
      if(auto b2body = body->body)
      {
        b2BodyType body_type = b2body->GetType();
        std::string body_type_str;
        if(body_type == b2_dynamicBody)
          body_type_str = "dynamic";
        else if(body_type == b2_staticBody)
          body_type_str = "static";
        else
          body_type_str = "kinematic";
        
        MRuby::HashBuilder builder(state);
        builder
          ("type", body_type_str)
          ("position", b2body->GetPosition())
          ("linear-damping", b2body->GetLinearDamping())
          ("angular-damping", b2body->GetAngularDamping())
        ;
        return builder.self;
      }
    }
    return mrb_nil_value();
  }

  MRUBY_COMPONENT_SET
  {
    if(!argc || ! mrb_hash_p(argv[0]))
      return mrb_nil_value();

    b2BodyDef body_def;
    std::string body_type;
    entt::entity world_id;

    MRuby::HashReader reader(state, argv[0]);
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

    auto& body = registry.emplace_or_replace< Physics::Body >(entity, b2_body);

    return argv[0];
  }

MRUBY_COMPONENT_INTERFACE_END

#endif

