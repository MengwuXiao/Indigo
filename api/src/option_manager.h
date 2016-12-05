/****************************************************************************
 * Copyright (C) 2009-2015 EPAM Systems
 * 
 * This file is part of Indigo toolkit.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 3 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 ***************************************************************************/

#ifndef __otion_manager_h__
#define __otion_manager_h__

#include "base_cpp/red_black.h"
#include "base_cpp/os_sync_wrapper.h"

#include <sstream>
#include <map>
#include <tuple>  

using namespace indigo;

#define DECL_SET_OPT_HANDLER(suffix, ftype, type, map)               \
   DLLEXPORT void setOptionHandler##suffix (const char* name, ftype func);     \

#define DEF_SET_OPT_HANDLER(suffix, ftype, type, map)                \
   void setOptionHandler##suffix (const char* name, ftype func) {    \
      if (typeMap.find(name))                                        \
         throw Error("Option \"%s\" already defined", name);         \
      typeMap.insert(name, type);                                    \
      map.insert(name, func);                                        \
   }

#define CHECK_OPT_DEFINED(name) \
      if (!typeMap.find(name)) \
         throw Error("Property \"%s\" not defined", name)

#define CHECK_OPT_TYPE(name, type) \
      if (typeMap.at(name) != type) \
         throw Error("Property type mismatch", name)

class OptionManager {
public:
   typedef void (*optf_string_t) (const char* value);
   typedef void (*optf_int_t) (int value);
   typedef void (*optf_bool_t) (int value);
   typedef void (*optf_float_t) (float value);
   typedef void (*optf_color_t) (float r, float g, float b);
   typedef void (*optf_xy_t) (int x, int y);
   typedef void (*optf_void_t) ();
   OptionManager ();

   DECL_ERROR;
   DEF_SET_OPT_HANDLER(String, optf_string_t, OPTION_STRING, hMapString)
   DEF_SET_OPT_HANDLER(Int, optf_int_t, OPTION_INT, hMapInt)
   DEF_SET_OPT_HANDLER(Bool, optf_bool_t, OPTION_BOOL, hMapBool)
   DEF_SET_OPT_HANDLER(Float, optf_float_t, OPTION_FLOAT, hMapFloat)
   DEF_SET_OPT_HANDLER(Color, optf_color_t, OPTION_COLOR, hMapColor)
   DEF_SET_OPT_HANDLER(XY, optf_xy_t, OPTION_XY, hMapXY)
   DEF_SET_OPT_HANDLER(Void, optf_void_t, OPTION_VOID, hMapVoid)


   bool hasOptionHandler (const char* name);

   void callOptionHandlerInt (const char* name, int value);
   void callOptionHandlerBool (const char* name, int value);
   void callOptionHandlerFloat (const char* name, float value);
   void callOptionHandlerColor (const char* name, float r, float g, float b);
   void callOptionHandlerXY (const char* name, int x, int y);
   void callOptionHandlerVoid(const char* name);
   void callOptionHandler (const char* name, const char* value);
   int nOptions () const;
      
   OsLock lock;
protected:
   enum OPTION_TYPE {OPTION_STRING, OPTION_INT, OPTION_BOOL, OPTION_FLOAT, OPTION_COLOR, OPTION_XY, OPTION_VOID};

   int _parseInt (const char *str, int &val);
   int _parseBool (const char *str, int &val);
   int _parseFloat (const char *str, float& val);
   int _parseColor (const char *str, float& r, float& g, float& b);
   int _parseSize (const char *str, int& w, int& h);

   RedBlackStringMap<OPTION_TYPE, false> typeMap;

   RedBlackStringMap<optf_string_t, false> hMapString;
   RedBlackStringMap<optf_int_t, false> hMapInt;
   RedBlackStringMap<optf_bool_t, false> hMapBool;
   RedBlackStringMap<optf_float_t, false> hMapFloat;
   RedBlackStringMap<optf_color_t, false> hMapColor;
   RedBlackStringMap<optf_xy_t, false> hMapXY;
   RedBlackStringMap<optf_void_t, false> hMapVoid;
      
   template <typename T> 
   void callOptionHandlerT (const char *name, T arg)
   {
      // Convert to string for default string parsing
      std::stringstream ss;
      ss << arg;
      std::string converted = ss.str();
      callOptionHandler(name, converted.c_str());
   }

private:
   OptionManager (const OptionManager&);
};

class OptionCacher {
public:
   OptionCacher ();
   
   void setOption (const char *name, int value);
   void setOption (const char *name, bool value);
   void setOption (const char *name, float value);
   void setOption (const char *name, float r, float g, float b);
   void setOption (const char *name, int x, int y);
   void setOption (const char *name, const char *value);
   
   void getOption (const char *name, int &value) const;
   void getOption (const char *name, bool &value) const;
   void getOption (const char *name, float &value) const;
   void getOption (const char *name, float &r, float &g, float &b) const;
   void getOption (const char *name, int &x, int &y) const;
   void getOption (const char *name, char *value, int size) const;
   
   void resetOptions ();
   
   DECL_ERROR;
   
private:

   typedef std::tuple<float, float, float> color;
   typedef std::tuple<int, int> XY;
   
   std::map<std::string, int> _intCache;
   std::map<std::string, bool> _boolCache;
   std::map<std::string, float> _floatCache;
   std::map<std::string, color> _colorCache;
   std::map<std::string, XY> _xyCache;
   std::map<std::string, std::string> _stringCache;
   
   OptionCacher (const OptionCacher&);
};

#endif //__otion_manager_h__
