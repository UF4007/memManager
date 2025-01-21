#define __MEMMNGR_INTERNAL_HEADER_PERMISSION														friend class eb::base;\
																									friend class eb::manager;\
												template<typename mu2, bool releaseable2>			friend class eb::memPtr;\
												template<typename mu2, bool releaseable2>			friend class eb::impPtr;\
												template<typename mu2, bool releaseable2>			friend class eb::dumbPtr;\
																									friend struct eb::para;\
																									friend class eb::ReflectResultKeyValue;\
																									friend struct eb::ReflectResult;\
																									friend class eb::Ingress;\
																									friend class eb::refIngress;\
																									friend class eb::Egress;\
												template<class cast2>								friend struct eb::pEgress;\
																									friend class eb::Subfile;\
																									friend class eb::lowlevel;\
												template <typename T2>								friend struct eb::has_save_fetch_struct;\
												template <typename T2>								friend void GWPP_sub(eb::base* mem, const char* key1, const char* key2, T2& var, eb::para& param);\

#define MEM_PERMISSION __MEMMNGR_INTERNAL_HEADER_PERMISSION

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

class base;
class manager;
template<typename mu, bool releaseable = true> class dumbPtr;
template<typename mu, bool releaseable = true> class impPtr;
template<typename mu, bool releaseable = true> class memPtr;
struct para;
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