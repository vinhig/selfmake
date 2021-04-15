/**
 * selfmake - Script Language
 * Copyright (C) 2021 The selfmake Authors
 * Published under MIT
 */

#undef SM_BEGIN
#undef SM_END
#undef SM_SILENT_END
#undef SM_DIAGNOSTIC_END

#ifdef SM_LOG_FLOW
    #define SM_BEGIN()
#else
    #define SM_BEGIN() 
#endif

#ifdef SM_LOG_FLOW
    #define SM_END(result) {return result;}
#else
    #define SM_END(result) {return result;} 
#endif

#ifdef SM_LOG_FLOW
    #define SM_SILENT_END() {return;}
#else
    #define SM_SILENT_END() {return;} 
#endif

#ifdef SM_LOG_FLOW
    #define SM_DIAGNOSTIC_END(result) {return result;}
#else
    #define SM_DIAGNOSTIC_END(result) {return result;} 
#endif

