#pragma once

namespace Physics
{

  // struct ContactListener : public b2ContactListener
  // {

  //   entt::registry* registry;
  //   entt::entity world;

  //   ContactListener(entt::registry* r, entt::entity world)
  //   : b2ContactListener(), registry(r), world(world)
  //   {
  //   }

  //   ContactListener(ContactListener&& cl)
  //   : registry(cl.registry), world(cl.world)
  //   {
  //     cl.registry = nullptr;
  //     cl.world = entt::null;
  //   }

  //   ContactListener& operator= (ContactListener&& cl)
  //   {
  //     registry = cl.registry;
  //     world = cl.world;

  //     cl.registry = nullptr;
  //     cl.world = entt::null;

  //     return *this;
  //   }

  //   void BeginContact(b2Contact* contact);
  //   // {
  //   //   std::cout << "begin contact" << std::endl;
  //   // }
  //   void EndContact(b2Contact* contact);
  //   // {
  //   //   std::cout << "end contact" << std::endl;
  //   // }

  //   void PreSolve(b2Contact* contact, const b2Manifold* old_manifold);
  //   // {
  //   //   std::cout << "pre solve" << std::endl;
  //   // }
  //   void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse); // old_manifold)
  //   // {
  //   //   std::cout << "post solve" << std::endl;
  //   // }
  // };

  struct World
  {
    b2World world; // = nullptr;
    int32 velocityIterations = 6;
    int32 positionIterations = 2;
    float PPM = 32.f;

    // ContactListener contact_listener;

    inline void step(float dt)
    {
      // if(world) 
      world.Step(dt, velocityIterations, positionIterations);
    }

    World(World&& w)
    :  world(std::move(w.world))
    // ,
    //   contact_listener(std::move(w.contact_listener)) //std::move(w.world))
    {
    }

    World& operator= (World&& w)
    {
      world = std::move(w.world);
      velocityIterations = w.velocityIterations;
      positionIterations = w.positionIterations;
      // contact_listener = std::move(w.contact_listener);
      return *this;
    }

    World()
    : world(b2Vec2(0,0))//, contact_listener(nullptr, entt::null)
    {
    }

    ~World()
    {
      // std::cout << " World destroyed! " << this << std::endl;
    }

  };

  // void publish_resources(entt::registry& r, entt::entity id, )

  void construct_world(entt::registry& r, entt::entity id, World& w)
  {
    // w.contact_listener = ContactListener(&r, id);
    // w.world.SetContactListener(& w.contact_listener);
    // std::cout << " set up world contact listener" << std::endl;
  }


  struct RayCastCallback : b2RayCastCallback
  {
    entt::registry& r;
    b2Vec2 initial_point;
    std::vector< std::pair< float32, b2Fixture* >> hits; 
    RayCastCallback(entt::registry& r, const b2Vec2& initial_point)
    :
      r(r),
      initial_point(initial_point)
    {
    }

    float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction)
    {
      float32 distance = b2DistanceSquared(initial_point, point);
      hits.push_back(std::make_pair(distance, fixture));
      return 1.f;
    }
  };

  template< typename Callback >
  void ray_cast(
    entt::registry& r,
    entt::entity entity,
    World& world,
    const Callback& callback,
    const b2Vec2& a, const b2Vec2& b)
  {
    // LogDebug("Ray cast " << a.x << "," << a.y << " to " << b.x << "," << b.y);
    using hit_t = std::pair< float32, b2Fixture* >;
    RayCastCallback ray_cast_cb(r, a);
    world.world.RayCast(&ray_cast_cb, a, b);
    std::sort(
      ray_cast_cb.hits.begin(),
      ray_cast_cb.hits.end(),
      [](const hit_t& a, const hit_t& b)
      {
        return a.first < b.first;
      }
    );
    for(const auto& item : ray_cast_cb.hits)
    {
      if(callback(item.second))
        return;
    }
  }

  // using ResourceBeginContactFn = void(*)(entt::registry&, entt::entity, entt::entity, b2Contact* contact);
  // using ResourceEndContactFn = void(*)(entt::registry&, entt::entity, entt::entity, b2Contact* contact);
  // using ResourcePreSolveFn = void(*)(entt::registry&, entt::entity, entt::entity, b2Contact*, const b2Manifold*);
  // using ResourcePostSolveFn = void(*)(entt::registry&, entt::entity, entt::entity, b2Contact*, const b2ContactImpulse*);
  
  // void ContactListener::BeginContact(b2Contact* contact)
  // {
  //   // std::cout << "begin contact" << std::endl;
  //   auto a = contact->GetFixtureA();
  //   auto b = contact->GetFixtureB();
  //   auto entity_a = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(a->GetUserData()));
  //   auto entity_b = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(b->GetUserData()));

  //   auto handler = Resources::get< ResourceBeginContactFn >(*registry, entity_a, "b2-begin-contact");
  //   if(handler && *handler) (*handler)(*registry, entity_a, entity_b, contact);
    
  //   handler = Resources::get< ResourceBeginContactFn >(*registry, entity_b, "b2-begin-contact");
  //   if(handler && *handler) (*handler)(*registry, entity_b, entity_a, contact);

  // }

  // void ContactListener::EndContact(b2Contact* contact)
  // {
  //   auto a = contact->GetFixtureA();
  //   auto b = contact->GetFixtureB();
  //   auto entity_a = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(a->GetUserData()));
  //   auto entity_b = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(b->GetUserData()));

  //   auto handler = Resources::get< ResourceEndContactFn >(*registry, entity_a, "b2-end-contact");
  //   if(handler && *handler) (*handler)(*registry, entity_a, entity_b, contact);
    
  //   handler = Resources::get< ResourceEndContactFn >(*registry, entity_b, "b2-end-contact");
  //   if(handler && *handler) (*handler)(*registry, entity_b, entity_a, contact);
  // }

  // void ContactListener::PreSolve(b2Contact* contact, const b2Manifold* old_manifold)
  // {
  //   auto a = contact->GetFixtureA();
  //   auto b = contact->GetFixtureB();
  //   auto entity_a = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(a->GetUserData()));
  //   auto entity_b = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(b->GetUserData()));

  //   auto handler = Resources::get< ResourcePreSolveFn >(*registry, entity_a, "b2-pre-solve");
  //   if(handler && *handler) (*handler)(*registry, entity_a, entity_b, contact, old_manifold);
    
  //   handler = Resources::get< ResourcePreSolveFn >(*registry, entity_b, "b2-pre-solve");
  //   if(handler && *handler) (*handler)(*registry, entity_b, entity_a, contact, old_manifold);
  // }

  // using ResourcePostSolveFn = void(*)(entt::registry&, entt::entity, entt::entity, b2Contact*, const b2ContactImpulse*);
  // void ContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) // old_manifold)
  // {
  //   auto a = contact->GetFixtureA();
  //   auto b = contact->GetFixtureB();
  //   auto entity_a = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(a->GetUserData()));
  //   auto entity_b = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(b->GetUserData()));

  //   auto handler = Resources::get< ResourcePostSolveFn >(*registry, entity_a, "b2-post-solve");
  //   if(handler && *handler) (*handler)(*registry, entity_a, entity_b, contact, impulse);
    
  //   handler = Resources::get< ResourcePostSolveFn >(*registry, entity_b, "b2-post-solve");
  //   if(handler && *handler) (*handler)(*registry, entity_b, entity_a, contact, impulse);
  // }


  template< typename Registry >
  struct QueryCallback : b2QueryCallback
  {
    Registry& registry;
    std::function<
      void(entt::registry&, entt::entity)
    > fn;

    QueryCallback(Registry& r, std::function< void(entt::registry&, entt::entity) > fn)
    : b2QueryCallback(), registry(r), fn(fn)
    {
    }

    bool ReportFixture(b2Fixture* fixture)
    {
      entt::entity id = static_cast< entt::registry::entity_type >(reinterpret_cast< uintptr_t >(fixture->GetUserData()));
      if(registry.valid(id) && fn)
      {
        fn(registry, id);
      }

      return true; // to keep going
    }
  };

  template<
    typename Registry,
    typename Callback
  >
  void query_aabb(
    Registry& registry,
    entt::entity world_entity,
    World& world,
    Callback& callback,
    const b2AABB& aabb)
  {
    QueryCallback<Registry> cb(registry, callback);
    world.world.QueryAABB(&cb, aabb);
  }


} // ::Physics


template<>
struct MRuby::ComponentInterface< Physics::World >
: MRuby::DefaultComponentInterface< Physics::World >
{
  static mrb_value get(mrb_state* state, entt::registry& registry, entt::entity entity, entt::registry::component_type type)
  {
    if(auto world = registry.try_get< Physics::World >(entity))
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

    b2Vec2 gravity;

    MRuby::HashReader reader(state, arg[0]);
    reader("gravity", gravity);

    auto& world = registry.assign_or_replace< Physics::World >(entity);
    world.world.SetGravity(gravity);

    mrb_int iterations;
    if(MRuby::read_hash(reader, "position-iterations", iterations))
      world.positionIterations = iterations;
    if(MRuby::read_hash(reader, "velocity-iterations", iterations))
      world.velocityIterations = iterations;

    return arg[0];
  }
};


// namespace Data
// {


//   template<typename Manager>
//   struct Loader< json11::Json, Physics::World, Manager >
//   {
//     static bool load(const json11::Json& json, Physics::World& component, Manager& manager, entt::entity entity)
//     {
//       component.world.SetGravity(b2vec2(json["gravity"]));
//       conditional_set(component.PPM, json["pixels-per-meter"]);
//       const json11::Json& borders = json["borders"];
//       if(borders.is_object())
//       {
//         float width = borders["width"].number_value() / component.PPM;
//         float height = borders["height"].number_value() / component.PPM;
//         float margin = borders["margin"].number_value() / component.PPM;

//         b2BodyDef border_body;
//         border_body.type = b2_staticBody;
//         border_body.position.Set(0,0);

//         std::array< b2Vec2, 4 > points{
//           b2Vec2(margin, margin),
//           b2Vec2(width-margin, margin),
//           b2Vec2(width-margin, height-margin),
//           b2Vec2(margin, height-margin)
//         };

//         b2Body* body = component.world.CreateBody(&border_body);
//         body->SetUserData(reinterpret_cast<void*>(entity));

//         for(int i = 0; i < 4; ++i)
//         {
//           b2EdgeShape shape;
//           shape.Set(points[i], points[(i+1) % points.size()]);

//           b2FixtureDef fixture_def;
//           fixture_def.shape = &shape;
//           fixture_def.density = 0.4f;
//           fixture_def.friction = 0.3f;

//           b2Fixture* fixture = body->CreateFixture(&fixture_def);
//           fixture->SetUserData(reinterpret_cast<void*>(entity));
//         }
//       }

//       return true;
//     }
//   };

// } // ::Data


