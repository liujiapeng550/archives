#ifdef RESOURCE_EXPORTS
#define RESOURCE_EXPORT __declspec(dllexport)
#else
#define RESOURCE_EXPORT __declspec(dllimport)
#endif // RESOURCE_EXPORTS
