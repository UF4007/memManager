#pragma once
#ifndef DECLARE_VARS
#define DECLARE_VARS _
#define DECLARE_VAR1(type, name) type name; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
    }

#define DECLARE_VAR2(type, name, type2, name2) type name; \
    type2 name2; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
    }

#define DECLARE_VAR3(type, name, type2, name2, type3, name3) type name; \
    type2 name2; \
    type3 name3; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
    }

#define DECLARE_VAR4(type, name, type2, name2, type3, name3, type4, name4) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
    }

#define DECLARE_VAR5(type, name, type2, name2, type3, name3, type4, name4, type5, name5) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
    }

#define DECLARE_VAR6(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
    }

#define DECLARE_VAR7(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
    }

#define DECLARE_VAR8(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
    }

#define DECLARE_VAR9(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
    }

#define DECLARE_VAR10(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
    }

#define DECLARE_VAR11(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
    }

#define DECLARE_VAR12(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
    }

#define DECLARE_VAR13(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
    }

#define DECLARE_VAR14(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
    }

#define DECLARE_VAR15(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
        GWPP(#name15, name15, para); \
    }

#define DECLARE_VAR16(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
        GWPP(#name15, name15, para); \
        GWPP(#name16, name16, para); \
    }
#define DECLARE_VAR17(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
        GWPP(#name15, name15, para); \
        GWPP(#name16, name16, para); \
        GWPP(#name17, name17, para); \
    }
#define DECLARE_VAR18(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
        GWPP(#name15, name15, para); \
        GWPP(#name16, name16, para); \
        GWPP(#name17, name17, para); \
        GWPP(#name18, name18, para); \
    }
#define DECLARE_VAR19(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    type19 name19; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
        GWPP(#name15, name15, para); \
        GWPP(#name16, name16, para); \
        GWPP(#name17, name17, para); \
        GWPP(#name18, name18, para); \
        GWPP(#name19, name19, para); \
    }
#define DECLARE_VAR20(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19, type20, name20) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    type19 name19; \
    type20 name20; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
        GWPP(#name15, name15, para); \
        GWPP(#name16, name16, para); \
        GWPP(#name17, name17, para); \
        GWPP(#name18, name18, para); \
        GWPP(#name19, name19, para); \
        GWPP(#name20, name20, para); \
    }
#define DECLARE_VAR21(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19, type20, name20, type21, name21) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    type19 name19; \
    type20 name20; \
    type21 name21; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
        GWPP(#name15, name15, para); \
        GWPP(#name16, name16, para); \
        GWPP(#name17, name17, para); \
        GWPP(#name18, name18, para); \
        GWPP(#name19, name19, para); \
        GWPP(#name20, name20, para); \
        GWPP(#name21, name21, para); \
    }
#define DECLARE_VAR22(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19, type20, name20, type21, name21, type22, name22) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    type19 name19; \
    type20 name20; \
    type21 name21; \
    type22 name22; \
    __MEMMNGR_INTERNAL_HEADER_PERMISSION \
    private:private:void save_fetch(memPara para) { \
        GWPP(#name, name, para); \
        GWPP(#name2, name2, para); \
        GWPP(#name3, name3, para); \
        GWPP(#name4, name4, para); \
        GWPP(#name5, name5, para); \
        GWPP(#name6, name6, para); \
        GWPP(#name7, name7, para); \
        GWPP(#name8, name8, para); \
        GWPP(#name9, name9, para); \
        GWPP(#name10, name10, para); \
        GWPP(#name11, name11, para); \
        GWPP(#name12, name12, para); \
        GWPP(#name13, name13, para); \
        GWPP(#name14, name14, para); \
        GWPP(#name15, name15, para); \
        GWPP(#name16, name16, para); \
        GWPP(#name17, name17, para); \
        GWPP(#name18, name18, para); \
        GWPP(#name19, name19, para); \
        GWPP(#name20, name20, para); \
        GWPP(#name21, name21, para); \
        GWPP(#name22, name22, para); \
    }

#endif // !DECLARE_VARS