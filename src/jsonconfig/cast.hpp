#ifndef JSOCONFIG_CAST_HPP_
#define JSOCONFIG_CAST_HPP_

#include "config.hpp"

#include <pficommon/data/unordered_map.h>

namespace jsonconfig {

class JsonConfigIarchiveCast {
 public:
  JsonConfigIarchiveCast(const Config &js): js(js) {}
  const Config& Get() const { return js; }
 private:
  const Config &js;
};

template <typename T>
void FromConfig(const Config& conf, T& v);

template <typename T>
inline void serialize(JsonConfigIarchiveCast &js, T &v) {
  // TODO: insert typecheck
  pfi::data::serialization::access::serialize(js, v);
}

////////

inline void CheckType(JsonConfigIarchiveCast& js, pfi::text::json::json::json_type_t t) {
  if (js.Get().Get().type() != t) {
    throw TypeError(js.Get().GetPath(), t, js.Get().Get().type());
  }
}

#define GENERATE_CONFIG_SERIALIZE_DEF(typ, json_typ)                \
  template <>                                                       \
  inline void serialize(JsonConfigIarchiveCast &js, typ &v) {       \
    CheckType(js, pfi::text::json::json::json_typ);                 \
    v = pfi::text::json::json_cast<typ>(js.Get().Get());            \
  }

GENERATE_CONFIG_SERIALIZE_DEF(bool, Bool)
GENERATE_CONFIG_SERIALIZE_DEF(int, Integer)
GENERATE_CONFIG_SERIALIZE_DEF(long, Integer)
GENERATE_CONFIG_SERIALIZE_DEF(float, Float)
GENERATE_CONFIG_SERIALIZE_DEF(double, Float)
GENERATE_CONFIG_SERIALIZE_DEF(std::string, String)

template <typename T>
inline void serialize(JsonConfigIarchiveCast& js, std::vector<T>& vs) {
  size_t size = js.Get().Size();
  std::vector<T> v(size);
  for (size_t i = 0; i < size; ++i) {
    // TODO: merge all errors
    FromConfig(js.Get()[i], v[i]);
  }
  vs.swap(v);
}

template <typename K, typename V>
inline void serialize(JsonConfigIarchiveCast& js, std::map<K, V>& m) {
  std::map<K, V> tmp;
  typedef Config::Iterator iter_t;
  for (iter_t it = js.Get().GetIterator(); it.HasNext(); it.Next()) {
    // TODO: merge all errors
    V value;
    FromConfig(it.GetValue(), value);
    tmp[it.GetKey()] = value;
  }
  tmp.swap(m);
}

template <typename K, typename V>
inline void serialize(JsonConfigIarchiveCast& js, pfi::data::unordered_map<K, V>& m) {
  pfi::data::unordered_map<K, V> tmp;
  typedef Config::Iterator iter_t;
  for (iter_t it = js.Get().GetIterator(); it.HasNext(); it.Next()) {
    // TODO: merge all errors
    V value;
    FromConfig(it.GetValue(), value);
    tmp[it.GetKey()] = value;
  }
  tmp.swap(m);
}

template <typename T>
inline void serialize(JsonConfigIarchiveCast &js, pfi::data::serialization::named_value<pfi::data::optional<T> >& v) {
  if (js.Get().Include(v.name)) {
    T t;
    FromConfig(js.Get()[v.name], t);
    v.v = t;
  } else {
    v.v = pfi::data::optional<T>();
  }
}

template <typename T>
inline void serialize(JsonConfigIarchiveCast &js, pfi::data::serialization::named_value<T>& v) {
  FromConfig(js.Get()[v.name], v.v);
}

////////

template <typename T>
void FromConfig(const Config& conf, T& v) {
  JsonConfigIarchiveCast cast(conf);
  serialize(cast, v);
}

template <class T>
T ConfigCast(const Config& c) {
  T v;
  FromConfig(c, v);
  return v;
}

}

#endif // JSOCONFIG_CAST_HPP_
