#ifndef ITU_RESOURCE_STORAGE_HPP
#define ITU_RESOURCE_STORAGE_HPP



// TODO next
// - [ ] rename `ResourceIndex` in `ResourceIndex`
// - [ ] create new `ResourceIndex` that contains variable sized data, and it is accessed
//       through `ResourceIndex`



#include <itu_engine.hpp>

typedef Uint64 ResourceIndexId;

template<typename T>
class ResourceIndex
{
// TMP singleton
    static ResourceIndex<T>* singleton_instance;
public:
    static void Init(EngineContext* context);
    static bool Get(ResourceIndexId id, T& out_resource);
    static void LoadAll(rapidjson::Value& config);

    // NOTE generic template implementations, will just open the file and copy the content into T
    static ResourceIndexId Load(rapidjson::Value& config);
    static ResourceIndexId Load(const char* path);

private:
    ResourceIndex() {};
    bool DoGet(ResourceIndexId id, T& out_resource);

    // NOTE generic template implementation, will just open the final and copy the content into T
    ResourceIndexId DoLoad(rapidjson::Value& config);
    ResourceIndexId DoLoad(const char* path);

    // NOTE generic template implementation, will just open the final and copy the content into T
    void DoLoadAll(rapidjson::Value& config);

    T DoLoad_Impl(const char* path)
    {
        SDL_Log("DO_LOAD::GENERIC: %s", path);
        SDL_IOStream* fp = SDL_IOFromFile(path, "r");
        size_t size = SDL_SeekIO(fp, 0, SDL_IO_SEEK_END);
        SDL_SeekIO(fp, 0, SDL_IO_SEEK_SET);

        // FIXME split paths for value resource (SDL_Texture*) and reference resources (data of an entire texture)
        // T* data = (T*)SDL_malloc(size);
        T data;
        SDL_ReadIO(fp, &data, size);

        SDL_CloseIO(fp);

        return data;
    }

    ResourceIndexId procedurally_create_id_last = 0;
    EngineContext* context;
    stbds_hm(ResourceIndexId, T) hm_resources;
};

template<typename T>
ResourceIndex<T>* ResourceIndex<T>::singleton_instance = NULL;

// template implementations

template<typename T>
void ResourceIndex<T>::Init(EngineContext* context)
{
    SDL_assert(!singleton_instance);
    SDL_assert(context);
    singleton_instance = new ResourceIndex<T>();
    singleton_instance->context = context;
}

template<typename T>
bool ResourceIndex<T>::Get(ResourceIndexId id, T& out_resource)
{
    SDL_assert(singleton_instance);
    return singleton_instance->DoGet(id, out_resource);
}

template<typename T>
ResourceIndexId ResourceIndex<T>::Load(rapidjson::Value& config)
{
    SDL_assert(singleton_instance);
    return singleton_instance->DoLoad(config);
}

template<typename T>
ResourceIndexId ResourceIndex<T>::Load(const char* path)
{
    SDL_assert(singleton_instance);
    return singleton_instance->DoLoad(path);
}


template<typename T>
void ResourceIndex<T>::LoadAll(rapidjson::Value& config)
{
    SDL_assert(singleton_instance);
    singleton_instance->DoLoadAll(config);
}

template<typename T>
bool ResourceIndex<T>::DoGet(ResourceIndexId id, T& out_resource)
{
    int loc = stbds_hmgeti(hm_resources, id);
    if(loc == -1)
        return false;

    out_resource = hm_resources[loc].value;
    return true;
}

template<typename T>
ResourceIndexId ResourceIndex<T>::DoLoad(rapidjson::Value& config)
{
    ResourceIndexId id = config["id"].GetUint64();
    const char* path = config["path"].GetString();

    T data = DoLoad_Impl(path);
    stbds_hmput(hm_resources, id, data);

    return id;
}

template<typename T>
ResourceIndexId ResourceIndex<T>::DoLoad(const char* path)
{
    // hacky way to handle resources not loaded from file
    // this is a terrible way to do this!
    ResourceIndexId id = --procedurally_create_id_last;

    T data = DoLoad_Impl(path);
    stbds_hmput(hm_resources, id, data);

    return id;
}

template<typename T>
void ResourceIndex<T>::DoLoadAll(rapidjson::Value& config)
{
    for(rapidjson::Value& res : config.GetArray())
        DoLoad(res);
}



#endif // ITU_RESOURCE_STORAGE_HPP