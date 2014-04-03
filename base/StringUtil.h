#ifndef _THEFOX_BASE_STRING_UTIL_H_
#define _THEFOX_BASE_STRING_UTIL_H_

#include <stdlib.h>
#include <stdarg.h>
#include <base/Types.h>

namespace thefox
{

class StringUtil
{
public:
    static const int kMaxStringLen = 100 * 1024;

    static int stoi(const String &str) 
    { 
        return (0 == str.length()) ? 0 : atoi(str.c_str());
    }

    static unsigned int stoui(const String &str) 
    { 
        return (0 == str.length()) ? 0 : static_cast<unsigned int>(atoi(str.c_str()));
    } 

    static long stol(const String &str) 
    { 
        return (0 == str.length()) ? 0L : atol(str.c_str()); 
    }

    static float stof(const String &str)
    { 
        return (0 == str.length()) ? 0.0f : static_cast<float>(atof(str.c_str())); 
    }

    static double stod(const String &str) 
    { 
        return (0 == str.length()) ? 0.0 : atof(str.c_str()); 
    }

    static bool stob(const String &str) 
    { 
        return (0 == str.length() || str == "0" || str == "false" || str == "FALSE") ? false : true; 
    }

    static String toString(const int val)
    {
        char buf[32] = {0};
		snprintf(buf, sizeof(buf), "%d", val);
		return buf;
    }

    static String toString(const unsigned int val)
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%u", val);
        return buf;
    }

    static String toString(const long val)
    {
        char buf[32] = {0};
		snprintf(buf, sizeof(buf), "%ld", val);
		return buf;
    }

    static String toString(const long long val)
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%lld", val);
        return buf;
    }

    static String toString(const double val)
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%f", val);
        return buf;
    }

    static String toString(const bool val)
    {
        return val ? "1" : "0";
    }

    static String &format(String &str, const char *format, ...)
    {
        va_list ap;
        va_start(ap, format);

        char *buf = (char *)malloc(kMaxStringLen);
        if (buf) {
            vsnprintf(buf, kMaxStringLen, format, ap);
            str = buf;
            free(buf);
            va_end(ap);
        }
        return str;
    }

    static String trimLeft(const String &str)
    {
        String::size_type index = str.find_first_not_of("\n\r\t");
        if (index != String::npos)
            return str.substr(index);
        return str;
    }

    static String trimRight(const String &str)
    {
        String::size_type index = str.find_last_not_of("\n\r\t");
        if (index != String::npos)
            return str.substr(0, index + 1);
        return str;
    }

    static String trim(const String &str)
    {
        return trimRight(trimLeft(str));
    }

    static String toLower(String &str)
    {
        for (String::size_type i = 0; i < str.length(); ++i)
            if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] += 0x20;
        return str;
    }

    static String toUpper(String &str)
    {
        for (String::size_type i = 0; i < str.length(); ++i)
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] -= 0x20;
        return str;
    }

    static bool charIsLetter(char c)
    {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
            return true;
        return false;
    }

    static bool equalsIgnoreCase(const String &s1, const String &s2)
    {
        if (s1.length() != s2.length())
            return false;
        
        for (String::size_type i = 0; i < s1.length(); ++i) {
            if (s1[i] == s2[i])
                continue;
            if (!charIsLetter(s1[i]) || !charIsLetter(s2[i]))
                return false;
            if (0x20 != abs(s1[i] - s2[i]))
                return false;
        }
        return true;
    }

    static String replace(String &str, const String &from, const String &to)
    {
        String::size_type pos = 0;
        while ((pos = str.find(from, pos)) != -1) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
        return str;
    }

    static void swap(String &ls, String &rs)
    {
        String temp(ls);
        ls = rs;
        rs = temp;
    }
};

inline String operator+(const char *ls, const String &rs)
{
    String buf(ls);
    buf += rs;
    return buf;
}

inline bool operator==(const char *ls, const String &rs)
{
    return !rs.compare(ls);
}

inline bool operator==(const String &ls, const char *rs)
{
    return !ls.compare(rs);
}
    
} // namespace thefox

#endif // _THEFOX_BASE_STRING_UTIL_H_
