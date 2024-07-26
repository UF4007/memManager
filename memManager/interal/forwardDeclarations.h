#define __MEMMNGR_INTERNAL_HEADER_PERMISSION														friend class memUnit;\
																									friend class memManager;\
												template<typename mu2, bool releaseable2>			friend class memPtr;\
												template<typename mu2, bool releaseable2>			friend class softPtr;\
												template<typename mu2, bool releaseable2>			friend class dumbPtr;\
																									friend struct memPara;\
																									friend class ReflectResultKeyValue;\
																									friend struct ReflectResult;\
																									friend class Ingress;\
																									friend class refIngress;\
																									friend class Egress;\
												template<class cast2>								friend struct pEgress;\
																									friend class Subfile;\
												template<class T2>									friend void GWPP_Struct(memUnit* mu3, const char* key1, const char* key2, T2& var, memPara para);\
																									friend class lowlevel;\
												template <typename T2>								friend struct has_save_fetch_struct;\

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

class memUnit;
class memManager;
template<typename mu, bool releaseable = true> class dumbPtr;
template<typename mu, bool releaseable = true> class impPtr;
template<typename mu, bool releaseable = true> class memPtr;
struct memPara;
class ReflectResultKeyValue;
struct ReflectResult;
class Ingress;
class refIngress;
class Egress;
template<class cast> struct pEgress;
class Subfile;
template<typename illg, int ID = 0> class pFunction;

template<class mu, bool releaseable = true> using vector_dumbPtr = std::vector<dumbPtr<mu, releaseable>>;
template<class mu, bool releaseable = true> using vector_impPtr = std::vector<impPtr<mu, releaseable>>;
template<class mu, bool releaseable = true> using vector_memPtr = std::vector<memPtr<mu, releaseable>>;

template<class cast> using vector_pEgress = std::vector<pEgress<cast>>;

template<typename...Args> using pVariant = std::variant<memPtr<Args>...>;

template<typename...Args> using vector_pVariant = std::vector<pVariant<Args...>>;