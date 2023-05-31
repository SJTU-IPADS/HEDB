#include <kv.h>

buffer_map* f_map_p = (buffer_map*)(new BufferMap<float, EncFloat>());
buffer_map* t_map_p = (buffer_map*)(new BufferMap<Str, EncStr>());

constexpr bool operator==(const EncFloat& lhs, const EncFloat& rhs)
{
    return memcmp(&lhs, &rhs, sizeof(EncFloat)) == 0;
}

template <typename PlainType, typename EncType>
void BufferMap<PlainType, EncType>::insert(const EncType* enc_val, const PlainType* plain_val)
{
    size_t bucket = enc_hash()(*enc_val);
    arr_map[bucket] = std::pair<EncType, PlainType>(*enc_val, *plain_val);
}

template <typename PlainType, typename EncType>
PlainType BufferMap<PlainType, EncType>::find(const EncType* enc_val, bool* found)
{
    size_t bucket = enc_hash()(*enc_val);
    int memcmp_result;
    if (typeid(EncType) == typeid(EncStr)) {
        const EncStr* enc_str = reinterpret_cast<const EncStr*>(enc_val);
        memcmp_result = memcmp(enc_str, &arr_map[bucket].first, enc_str->len + 4);
    } else {
        memcmp_result = memcmp(enc_val, &arr_map[bucket].first, sizeof(EncType));
    }
    if (memcmp_result == 0) {
        // printf("FOUND!!!!!");
        *found = true;
        return arr_map[bucket].second;
    }
    *found = false;
    return PlainType {};
}

void float_map_insert(buffer_map* m, const EncFloat* enc_val, const float* plain_val)
{
#ifdef ENABLE_KV
    ((BufferMap<float, EncFloat>*)m)->insert(enc_val, plain_val);
#endif
}

float float_map_find(buffer_map* m, const EncFloat* enc_val, bool* found)
{
#ifdef ENABLE_KV
    return ((BufferMap<float, EncFloat>*)m)->find(enc_val, found);
#else
    *found = false;
    return 0;
#endif
}

void text_map_insert(buffer_map* m, const EncStr* enc_val, const Str* plain_val)
{
#ifdef ENABLE_KV
    ((BufferMap<Str, EncStr>*)m)->insert(enc_val, plain_val);
#endif
}

Str text_map_find(buffer_map* m, const EncStr* enc_val, bool* found)
{
#ifdef ENABLE_KV
    return ((BufferMap<Str, EncStr>*)m)->find(enc_val, found);
#else
    Str tmp;
    *found = false;
    return tmp;
#endif
}
