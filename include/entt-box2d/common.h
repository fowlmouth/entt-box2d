#pragma once

#ifdef FOWL_ENTT_MRUBY

bool b2vec2(mrb_state* mrb, mrb_value value, b2Vec2& vec)
{
  if(mrb_array_p(value))
  {
    if(ARY_LEN(mrb_ary_ptr(value)) != 2)
      return false;

    vec.x = mrb_to_flo(mrb, mrb_ary_entry(value, 0));
    vec.y = mrb_to_flo(mrb, mrb_ary_entry(value, 1));

    return true;
  }
  return false;
}

namespace MRuby
{
  bool read_hash(HashReader& reader, const char* symbol, b2Vec2& value)
  {
    mrb_value val = mrb_hash_get(
      reader.state,
      reader.self,
      mrb_symbol_value(mrb_intern_cstr(reader.state, symbol))
    );
    return b2vec2(reader.state, val, value);
  }

  bool read_hash(HashReader& reader, const char* symbol, entt::entity& entity)
  {
    mrb_value val = mrb_hash_get(
      reader.state,
      reader.self,
      mrb_symbol_value(mrb_intern_cstr(reader.state, symbol))
    );
    if(mrb_fixnum_p(val))
    {
      entity = static_cast< entt::entity >(mrb_fixnum(val));
      return true;
    }
    return false;
  }
}

#endif