//serialize recursive level limit(no used)
#ifndef MEM_RECURSIVE_LEVEL
#define MEM_RECURSIVE_LEVEL 1000
#endif

//turn on the reflection relative code
#ifndef MEM_REFLECTION_ON
#define MEM_REFLECTION_ON 0
#endif

//turn on the rapid json relative code
#ifndef MEM_RJSON_ON
#define MEM_RJSON_ON 1
#endif

//turn on the mysql support code
#ifndef MEM_MYSQL_ON
#define MEM_MYSQL_ON 0
#endif
//MySQL <---bind---> memUnit
// adapted type:
// TINYINT ---------------------------------> int8
// SMALLINT --------------------------------> int16
// INT -------------------------------------> int32
// BIGINT ----------------------------------> int64
// FLOAT -----------------------------------> float
// DOUBLE ----------------------------------> double
// BINARY ----------------------------------> char[]
// MEDIUMBLOB ------------------------------> std::string series
// BINARY ----------------------------------> via GWPP_Any (memory copy directly)
// DATETIME --------------------------------> MYSQL_TIME, via GWPP_SQL_TIME