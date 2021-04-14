/**
 * netzhaut - Web Browser Engine
 * Copyright (C) 2020 The netzhaut Authors
 * Published under LGPLv3
 */

#undef SELFMAKE_BEGIN
#undef SELFMAKE_END
#undef SELFMAKE_SILENT_END
#undef SELFMAKE_DIAGNOSTIC_END

#ifdef NH_LOG_FLOW
    #define SELFMAKE_BEGIN()
#else
    #define SELFMAKE_BEGIN() 
#endif

#ifdef NH_LOG_FLOW
    #define SELFMAKE_END(result) {return result;}
#else
    #define SELFMAKE_END(result) {return result;} 
#endif

#ifdef NH_LOG_FLOW
    #define SELFMAKE_SILENT_END() {return;}
#else
    #define SELFMAKE_SILENT_END() {return;} 
#endif

#ifdef NH_LOG_FLOW
    #define SELFMAKE_DIAGNOSTIC_END(result) {return result;}
#else
    #define SELFMAKE_DIAGNOSTIC_END(result) {return result;} 
#endif

